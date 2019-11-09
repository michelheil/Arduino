import org.apache.log4j.{Level, LogManager, Logger}
import org.apache.spark.SparkConf
import org.apache.spark.streaming.StreamingContext
import org.apache.spark.streaming.mqtt.MQTTUtils
import org.apache.spark.streaming.Milliseconds
import org.apache.spark.streaming.dstream.{DStream, ReceiverInputDStream}
import org.eclipse.paho.client.mqttv3._
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence

object Main extends App {

  val brokerURL = "tcp://localhost:1883"
  val subTopicName = "/arbeitszimmer/temperatur"
  val pubTopicName = "/arbeitszimmer/temperatur/ergebnis"

  lazy val mqttPublisher: MqttClient = new MqttClient(brokerURL, MqttClient.generateClientId(), new MemoryPersistence())

  val sparkConf = new SparkConf(true).setAppName("MqttWordCount").setMaster("local[*]")
  val ssc = new StreamingContext(sparkConf, Milliseconds(1))

  // Set root Log Level to warning
  val log: Logger = LogManager.getRootLogger()
  log.setLevel(Level.ERROR)
  // throws Warning because Spark cannot replicate data (https://stackoverflow.com/questions/32583273/spark-streaming-get-warn-replicated-to-only-0-peers-instead-of-1-peers)

  val mqttConsumer: ReceiverInputDStream[String] = MQTTUtils.createStream(ssc, brokerURL, subTopicName)

  processAndTransferMessage(mqttConsumer)

  ssc.start()
  ssc.awaitTermination()


  def processAndTransferMessage(dStream: ReceiverInputDStream[String]): Unit = {
    //val pub = dStream.map(word => (word, 1)).reduceByKey(_ + _)
    //pub.print()

    //val modDStream: DStream[Array[String]] = dStream.map(line => line.split(";"))


    try {
      dStream.foreachRDD(rdd => {
        if (!rdd.isEmpty()) {
          rdd.foreachPartition(partition => {
            partition.foreach(msg => {
              log.warn(s"Attempting to connect to broker ${brokerURL} and topic ${pubTopicName}.")
              mqttPublisher.connect()
              val pubTopic: MqttTopic = mqttPublisher.getTopic(pubTopicName)
              log.warn(s"Connected to broker ${brokerURL} and topic ${pubTopicName}.")

              // sending message to MQTT including error handling
              try {
                log.warn(s"Preparing message to be sent.")
                val maxValue: String = msg.split(";").reduce((x, y) => if(x.toDouble > y.toDouble) x else y)
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
