import org.apache.log4j.{Level, LogManager}
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
  LogManager.getRootLogger.setLevel(Level.WARN)
  // throws Warning because Spark cannot replicate data (https://stackoverflow.com/questions/32583273/spark-streaming-get-warn-replicated-to-only-0-peers-instead-of-1-peers)

  val mqttConsumer: ReceiverInputDStream[String] = MQTTUtils.createStream(ssc, brokerURL, subTopicName)

  processAndTransferMessage(mqttConsumer)

  ssc.start()
  ssc.awaitTermination()


  def processAndTransferMessage(dStream: ReceiverInputDStream[String]): Unit = {
    //val pub = dStream.map(word => (word, 1)).reduceByKey(_ + _)
    //pub.print()
    //val modDStream = dStream.map(line => line.split(" "))

    try {
      dStream.foreachRDD(rdd => {

        if (!rdd.isEmpty()) {
          rdd.foreachPartition(partition => {
            partition.foreach(msg => {
              mqttPublisher.connect()
              val msgContent: String = msg.length.toString() // processing individual messages
              val message: MqttMessage = new MqttMessage(msgContent.getBytes("utf-8"))
              val pubTopic: MqttTopic = mqttPublisher.getTopic(pubTopicName)

              // sending message to MQTT including error handling
              try {
                pubTopic.publish(message)
              } catch {
                case e: MqttException if e.getReasonCode == MqttException.REASON_CODE_MAX_INFLIGHT => Thread.sleep(10)
              } finally {
                if (mqttPublisher.isConnected()) {
                  mqttPublisher.disconnect()
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
