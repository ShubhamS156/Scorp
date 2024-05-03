#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_SHT45.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#define MQTT_EN 1

ArtronShop_SHT45 sht45(&Wire, 0x44); // SHT45-AD1B => 0x44

#ifdef MQTT_EN
// WiFi credentials
const char* ssid = "NeilSoft-Guest";
const char* password = "Neil$oft@2023";

const char* mqtt_broker = "10.1.1.177";
const int mqtt_port = 1883;
const char* mqtt_username = "mqtt_user_iiot";  
const char* mqtt_password = "Neilsoft@1";  
const char* mqtt_topic = "admin/01/serverRoom/SHT45";

StaticJsonDocument<100> doc;
WiFiClient espClient;
PubSubClient client(espClient);
#endif

void setup() {
  Serial.begin(115200);
  Wire.begin();

  while (!sht45.begin()) {
    Serial.println("SHT45 not found !");
    delay(1000);
  }

#ifdef MQTT_EN
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
       delay(500);
       Serial.println("Connecting to WiFi..");
       Serial.println(WiFi.status());
   }
   Serial.println("Connected to the WiFi network");
   Serial.print("IP: ");
   Serial.println(WiFi.localIP());

   client.setServer(mqtt_broker, mqtt_port);
   while (!client.connected()) {
       Serial.println("Connecting to MQTT...");
       String client_id = "esp32-client-";
       client_id += String(WiFi.macAddress());
       if (client.connect(client_id.c_str(), mqtt_username, mqtt_password )) {
           Serial.println("connected");  
       } else {
           Serial.print("failed with state "); Serial.println(client.state());
           delay(2000);
       }
   }
#endif
}

#ifdef MQTT_EN
void publishData(float temp, float hum){
    // char tempStr[8];
    // char humStr[8];
    // dtostrf(temp, 1, 2, tempStr);
    // dtostrf(hum, 1, 2, humStr);

    // // Create data string
    // String data = String("{\"temperature\": ") + tempStr + " C, " + "Humidity: " + humStr + "%";
    // client.publish(mqtt_topic, data.c_str());

    doc["temperature"] = temp;
    doc["humidity"] = hum;
    char jsonbuffer[512];
    serializeJson(doc,jsonbuffer);
    doc.clear();
    client.publish(mqtt_topic,jsonbuffer);
}
void checkConnections() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Restarting...");
        ESP.restart();
    }
  
    if (!client.connected()) {
        Serial.println("MQTT connection lost. Reconnecting...");
        connectToMQTT();
    }
}
#endif

void loop() {
#ifdef MQTT_EN
  client.loop();
  checkConnections();
#endif
  if (sht45.measure()) {
    float temp = sht45.temperature();
    float hum = sht45.humidity();
    Serial.print("Temperature: ");
    Serial.print(temp, 2);
    Serial.print(" *C\tHumidity: ");
    Serial.print(hum, 2);
    Serial.println("%RH");
#ifdef MQTT_EN
    publishData(temp, hum);
#endif
  } else {
    Serial.println("SHT45 read error");
  }
  delay(1000);
}
