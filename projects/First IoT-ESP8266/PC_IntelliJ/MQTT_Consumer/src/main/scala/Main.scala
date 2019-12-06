import org.apache.log4j.{Level, LogManager, Logger}
import org.apache.spark.SparkConf
import org.apache.spark.streaming.dstream.{DStream, ReceiverInputDStream}
import org.apache.spark.streaming.mqtt.MQTTUtils
import org.apache.spark.streaming.{Milliseconds, StreamingContext}
import org.eclipse.paho.client.mqttv3._
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence

object Main extends App {

  val brokerURL = "tcp://localhost:1883"
  val subTopicName = "/arbeitszimmer/temperatur"
  val pubTopicName = "/arbeitszimmer/temperatur/ergebnis"
  val windowDuration = Milliseconds(500);
  val slideDuration = Milliseconds(500);

  val sparkConf = new SparkConf(true).setAppName("MqttWordCount").setMaster("local[*]")
  val ssc = new StreamingContext(sparkConf, Milliseconds(1))

  // Set root Log Level to warning
  val log: Logger = LogManager.getRootLogger()
  log.setLevel(Level.WARN)
  // throws Warning because Spark cannot replicate data (https://stackoverflow.com/questions/32583273/spark-streaming-get-warn-replicated-to-only-0-peers-instead-of-1-peers)

  val mqttConsumer: ReceiverInputDStream[String] = MQTTUtils.createStream(ssc, brokerURL, subTopicName)

  processAndTransferMessage(mqttConsumer)

  ssc.start()
  ssc.awaitTermination()


  def processAndTransferMessage(dStream: ReceiverInputDStream[String]): Unit = {

    val pub: DStream[String] = dStream
      //.window(windowDuration, slideDuration)
      //.flatMap(_.split(";"))
      .filter(word => word.contains("."))
      //.reduceByWindow((x, y) => if(x.toDouble > y.toDouble) x else y, windowDuration, slideDuration)

    try {
      pub.foreachRDD(rdd => {
        if (!rdd.isEmpty()) {
          rdd.foreachPartition(partition => {
            partition.foreach(msg => {
              val mqttPublisher: MqttClient = new MqttClient(brokerURL, MqttClient.generateClientId(), new MemoryPersistence())
              log.warn(s"Attempting to connect to broker ${brokerURL} and topic ${pubTopicName}.")
              mqttPublisher.connect()
              val pubTopic: MqttTopic = mqttPublisher.getTopic(pubTopicName)
              log.warn(s"Connected to broker ${brokerURL} and topic ${pubTopicName}.")

              // sending message to MQTT including error handling
              try {
                log.warn(s"Preparing message to be sent.")
                val maxValue: String = msg//.split(";")
                  //.filterNot(word => word.contains("Send"))
                  //.filterNot(word => word.contains(","))
                  //.reduce((x, y) => if(x.toDouble > y.toDouble) x else y)
                val message: MqttMessage = new MqttMessage(maxValue.getBytes("utf-8"))
                log.warn(s"Attempting to publish message ${message}.")
                pubTopic.publish(message)
                log.warn(s"Published message ${message}.")
              } catch {
                case e: MqttException if e.getReasonCode == MqttException.REASON_CODE_MAX_INFLIGHT => Thread.sleep(10)
              } finally {
                if (mqttPublisher.isConnected()) {
                  mqttPublisher.disconnect()
                  log.warn("Disconnected from broker.")
                }
              }
            }) // foreach msg
          }) // foreach partition
        } // if RDD not empty
      }) // foreachRDD
    } catch {
      case e: MqttException => println("Exception Caught: " + e)
    }
  }


}
