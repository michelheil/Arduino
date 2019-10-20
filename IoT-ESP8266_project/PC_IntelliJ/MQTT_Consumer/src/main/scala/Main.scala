import org.apache.log4j.{Level, LogManager, Logger}
import org.apache.spark.SparkConf
import org.apache.spark.streaming.StreamingContext
import org.apache.spark.streaming.mqtt.MQTTUtils
import org.apache.spark.streaming.Milliseconds
import org.eclipse.paho.client.mqttv3._
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence

object Main extends App {
  val sparkConf = new SparkConf(true).setAppName("MqttWordCount").setMaster("local[*]")
  val ssc = new StreamingContext(sparkConf, Milliseconds(10000))

  // Set root Log Level to warning
  LogManager.getRootLogger.setLevel(Level.WARN)
  // throws Warning because Spark cannot replicate data (https://stackoverflow.com/questions/32583273/spark-streaming-get-warn-replicated-to-only-0-peers-instead-of-1-peers)

  val brokerURL = "tcp://localhost:1883"
  val subTopicName = "/arbeitszimmer/temperatur"
  val pubTopicName = "/arbeitszimmer/temperatur/ergebnis"

  val lines = MQTTUtils.createStream(ssc, brokerURL, subTopicName)

  val words = lines.flatMap(line => line.split(" "))
  val wordCount = words.map(word => (word, 1)).reduceByKey(_ + _)

  wordCount.print()

  var publisher: MqttClient = null

  // Publishing back to MQTT
  try {
    val publisher: MqttClient = new MqttClient(brokerURL, MqttClient.generateClientId(), new MemoryPersistence())
    publisher.connect()

    val pubTopic: MqttTopic = publisher.getTopic(pubTopicName)
    val msgContent = "ScalaToArduino"
    val message = new MqttMessage(msgContent.getBytes("utf-8"))

    try {
      pubTopic.publish(message)
      println(s"Published data. topic: ${pubTopic.getName()}; Message: $message")
    } catch {
      case e: MqttException if e.getReasonCode == MqttException.REASON_CODE_MAX_INFLIGHT => Thread.sleep(10)
        println("Queue is full, wait for to consume data from the message queue")
    }
  } catch {
      case e: MqttException => println("Exception Caught: " + e)
  } finally {
    if (publisher != null) {
      publisher.disconnect()
    }
  }

  ssc.start()
  ssc.awaitTermination()
}
