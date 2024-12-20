/*
  ArduinoMqttClient - WiFi Simple Receive Callback

  This example connects to a MQTT broker and subscribes to a single topic.
  When a message is received it prints the message to the Serial Monitor,
  it uses the callback functionality of the library.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board

  This example code is in the public domain.
*/

#include <ArduinoMqttClient.h>
//解析json
//#include <ArduinoJson.h>

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
  #include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#endif

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
char VALVE_R;
char VALVE_B;

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.2";
int        port     = 1883;
const char topic[]  = "esp32/dev1";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  //初始化输入输出引脚
  initAll();
  turnOffAll();
  
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  delay(200);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(500);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keep alives which avoids being disconnected by the broker
  mqttClient.poll();
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  
  /*ArduinoJson库实现json解析
  ***************************
  StaticJsonDocument<256> doc;
  Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, mqttClient);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do doc["time"].as<long>();
  const char* sensor = doc["sensor"];

  // Print values.
  //
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  ***************************
  */

  //read
  //***************************
  char sensor[20];
  int i = 0;
  while (mqttClient.available()) {
    sensor[i++] = (char)mqttClient.read();
    }
  sensor[i] = 0;
  //Serial.println(sensor);
  if(strcmp(sensor,"制水")==0) {
      turnOffLed();
      digitalWrite(LED_B, HIGH);
      controlValve(1,1);
      controlValve(2,0);
    }
    else if(strcmp(sensor,"冲洗")==0) {
      turnOffLed();
      digitalWrite(LED_G, HIGH);
      controlValve(1,0);
      controlValve(2,0);    
    }
    else if(strcmp(sensor,"停机")==0) {
      turnOffLed();
      controlValve(1,0);
      controlValve(2,1);    
    }
  //***************************
  
  /* readBytes
  ***************************  
  if(mqttClient.available()) {
    char sensor[100]={0};
    mqttClient.readBytes(sensor,100);
    Serial.println(sensor);
    if(strcmp(sensor,"制水")==0) {
      turnOffLed();
      digitalWrite(LED_B, HIGH);
      controlValve(1,1);
      controlValve(2,0);
    }
    else if(strcmp(sensor,"冲洗")==0) {
      turnOffLed();
      digitalWrite(LED_G, HIGH);
      controlValve(1,0);
      controlValve(2,0);    
    }
    else if(strcmp(sensor,"停机")==0) {
      turnOffLed();
      controlValve(1,0);
      controlValve(2,1);    
    }
  }
  ***************************
  */
  
  /* readStringUntil
  ***************************  
  String sensor;
  if(mqttClient.available()) {
    sensor=mqttClient.readStringUntil('\n');
  }
  Serial.println(sensor);
  Serial.println("冲洗");
  if(sensor.compareTo("冲洗")==0) {
    turnOffLed();
    digitalWrite(LED_G, HIGH);
    controlValve(1,0);
    controlValve(2,0); 
  }
  ***************************  
  */
  
}

void initAll() {
  //LED初始化
  //pinMode(BUTTON, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  
  //电磁阀初始化
  pinMode(VALVE1_R, OUTPUT);
  pinMode(VALVE1_B, OUTPUT);
  pinMode(VALVE2_R, OUTPUT);
  pinMode(VALVE2_B, OUTPUT);
}

void turnOffAll() {
  //关闭LED
  //pinMode(BUTTON, INPUT);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  
  //关闭电磁阀
  digitalWrite(VALVE1_R, LOW);
  digitalWrite(VALVE1_B, LOW);
  digitalWrite(VALVE2_R, LOW);
  digitalWrite(VALVE2_B, LOW);
}

void turnOffLed() {
  //关闭LED
  //pinMode(BUTTON, INPUT);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void controlValve(char valve,bool revFlag) {

  if (valve==1) {
    VALVE_R=VALVE1_R;
    VALVE_B=VALVE1_B;
  }
  else if (valve==2) {
    VALVE_R=VALVE2_R;
    VALVE_B=VALVE2_B;    
  }

  if(revFlag==0) {
    digitalWrite(VALVE_R,HIGH);
    delay(100);
    digitalWrite(VALVE_R,LOW);
  }
  else {
    digitalWrite(VALVE_B,HIGH);
    delay(100);
    digitalWrite(VALVE_B,LOW);
  }
}
