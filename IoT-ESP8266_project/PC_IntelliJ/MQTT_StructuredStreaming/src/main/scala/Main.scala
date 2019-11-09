import org.apache.spark.sql.streaming.StreamingQuery
import org.apache.spark.sql.{DataFrame, Dataset, SparkSession}

object Main extends App {

  val brokerURL = "tcp://localhost:1883"
  val subTopicName = "/arbeitszimmer/temperatur"
  val pubTopicName = "/arbeitszimmer/temperatur/ergebnis"

  val spark: SparkSession = SparkSession
    .builder
    .appName("MQTT_StructStreaming")
    .master("local[*]")
    .config("spark.sql.streaming.checkpointLocation", "/home/michael/sparkCheckpoint")
    .getOrCreate

  spark.sparkContext.setLogLevel("ERROR")

  import spark.implicits._

  val lines: Dataset[String] = spark.readStream
    .format("org.apache.bahir.sql.streaming.mqtt.MQTTStreamSourceProvider")
    .option("topic", subTopicName)
    .option("clientId", "some-client-id")
    .option("persistence", "memory")
    .load(brokerURL)
    .selectExpr("CAST(payload AS STRING)").as[String]

  // Split the lines into words
  val words: Dataset[String] = lines.as[String].flatMap(_.split(";"))

  // Generate running word count
  val wordCounts: DataFrame = words.groupBy("value").count()

  // Start running the query that prints the running counts to the console
  val query: StreamingQuery = wordCounts.writeStream
    .format("org.apache.bahir.sql.streaming.mqtt.MQTTStreamSinkProvider")
    .outputMode("complete")
    .option("topic", pubTopicName)
    .option("brokerURL", brokerURL)
    .start

  query.awaitTermination()



}
