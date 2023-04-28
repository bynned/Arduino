# THIS IS A EMBEDDED PROJECTS COURSE WORK
![ArduinoCAR](https://user-images.githubusercontent.com/99414068/235167137-bd5eecf7-2390-449f-8e47-9786a0cf8cd6.jpeg)

## How it works:

### The Arduino mega controls the lego car. The Arduino reads commands from the ESP's serial monitor:
```
    while (Serial.available() > 0) {
    message = Serial.readStringUntil('\n');
    command = message.substring(0, message.indexOf(":"));
    value = message.substring(message.indexOf(":") + 1).toInt();
    }
    if(command == "Move"){
      Move(value);
      command = "";
    }
    else if(command == "Turn" && value > 0){
      TurnRight(value);
      command = "";
    }
    else if(command == "Turn" && value < 0){
      TurnLeft(value);
      command = "";
    }
```
### The Arduino mega also sends the lidar and compass value to the ESP in JSON:
```
void readValues() {
  Wire.beginTransmission(CMPS14_address);
  Wire.write(1);
  Wire.endTransmission(false);
  Wire.requestFrom(CMPS14_address, 1, true);
  if(Wire.available() >= 1)
    raw = Wire.read();
   
  vehicle_direction = map(raw, 0, 255, 0, 360);

  int lidarValue = myLidarLite.distance();
  char resp[80];
  int n = sprintf(resp, "Lid=%d Com=%d", lidarValue, vehicle_direction);
  Serial.println(resp);
  delay(100);
}
```
### The ESP is connected to an MQTT Mosquitto broker, and the ESP recieves commands from the topic "ESPDES":

```
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.write(payload, length);
}
```
### The ESP sends the Lidar and compass data to the broker, to a topic "LIDCOM"
```
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
} 
 // Read Lid and Com values from Arduino via Serial
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
```
## How to send commands to the car:
You can send a message e.g. "Move:1" to the car so it moves 1cm forward:
```
mosquitto_pub -t "ESPDES" -m "Move:1"
```
You can also see the Lidar and Compass data by subscribing to the LIDCOM topic:
```
mosquitto_sub -h <broker_ip> -t "LIDCOM"
```

