#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "wifissid";
const char* password = "Wifipassword";
const char* mqtt_server = "BrokerIP";
const char* in_topic = "ESPDES"; // topic for messages received from the broker
const char* out_topic = "LIDCOM"; // topic for publishing Lid and Com values

const size_t JSON_BUFFER_SIZE = JSON_OBJECT_SIZE(2);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(in_topic);
    } else {
      Serial.println("Failed to connect to broker");
      delay(5000);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe(in_topic);
    } else {
      delay(5000);
    }
  }
}

void loop() {
  values
    DynamicJsonDocument lidarJsonDoc(JSON_BUFFER_SIZE);
    lidarJsonDoc["Lid"] = lidar_value;
    String lidarJsonString;
    serializeJson(lidarJsonDoc, lidarJsonString);

    DynamicJsonDocument compassJsonDoc(JSON_BUFFER_SIZE);
    compassJsonDoc["Com"] = compass_value;
    String compassJsonString;
    serializeJson(compassJsonDoc, compassJsonString);

    // Publish the JSON strings over MQTT
    client.publish(out_topic, lidarJsonString.c_str());
    client.publish(out_topic, compassJsonString.c_str());
  } // Read Lid and Com values from Arduino via Serial
if (Serial.available() > 0) {
  String message = Serial.readStringUntil('\n');

  // Find Lid and Com values in the message string
  int lid_pos = message.indexOf("Lid=");
  int com_pos = message.indexOf("Com=");
  if (lid_pos != -1 && com_pos != -1) {
    int lidar_value = message.substring(lid_pos + 4, com_pos - 1).toInt();
    int compass_value = message.substring(com_pos + 4).toInt();
}

  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.write(payload, length);
}