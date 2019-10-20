name := "MQTT_SparkStreaming"

version := "0.1"

scalaVersion := "2.11.12"

libraryDependencies += "org.apache.spark" %% "spark-core" % "2.3.1"
libraryDependencies += "org.apache.spark" %% "spark-streaming" % "2.3.1"

// https://mvnrepository.com/artifact/org.apache.bahir/spark-streaming-mqtt
libraryDependencies += "org.apache.bahir" %% "spark-streaming-mqtt" % "2.1.0"
