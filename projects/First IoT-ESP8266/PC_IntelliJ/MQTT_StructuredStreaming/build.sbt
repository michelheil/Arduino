name := "MQTT_StructuredStreaming"

version := "0.1"

// https://mvnrepository.com/artifact/org.apache.spark/spark-core
libraryDependencies += "org.apache.spark" % "spark-core_2.12" % "2.4.4"

// https://mvnrepository.com/artifact/org.apache.spark/spark-sql
libraryDependencies += "org.apache.spark" % "spark-sql_2.12" % "2.4.4"

// https://mvnrepository.com/artifact/org.apache.spark/spark-streaming
libraryDependencies += "org.apache.spark" % "spark-streaming_2.12" % "2.4.4" % "provided"

// https://mvnrepository.com/artifact/org.apache.spark/spark-streaming-mqtt
//libraryDependencies += "org.apache.spark" % "spark-streaming-mqtt_2.12" % "1.6.3"

// https://mvnrepository.com/artifact/org.apache.bahir/spark-sql-streaming-mqtt
libraryDependencies += "org.apache.bahir" % "spark-sql-streaming-mqtt_2.12" % "2.4.0"



