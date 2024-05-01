#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_SHT45.h>
#include <PubSubClient.h>
#include <WiFi.h>
#define MQTT_EN 1


ArtronShop_SHT45 sht45(&Wire, 0x44); // SHT45-AD1B => 0x44
#ifdef MQTT_EN
// WiFi credentials
const char* ssid = "ACC";
const char* password = "Neilsoft@123";

const char* mqtt_broker = "10.1.1.177";
const char* mqtt_username = "mqtt-user";  
const char* mqtt_password = "Neilsoft@123";  
const char* mqtt_topic = "SHT45/sensorData";
#endif

WiFiClient espClient;
PubSubClient client(espClient);

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
    }
    Serial.println("Connected to the WiFi network");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_broker, 1883);
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
        char tempStr[8];
        char humStr[8];
        dtostrf(temp, 1, 2, tempStr);
        dtostrf(hum, 1, 2, humStr);

        // Create data string
        String data = String("Temperature: ") + tempStr + " C, " + "Humidity: " + humStr + "%";
        client.publish(mqtt_topic, data.c_str());
}
#endif

void loop() {
#ifdef MQTT_EN
  client.loop();
#endif
  if (sht45.measure()) {
    Serial.print("Temperature: ");
    Serial.print(sht45.temperature(), 2);
    Serial.print(" *C\tHumidity: ");
    Serial.print(sht45.humidity(), 2);
    Serial.print(" %RH");
    Serial.println();
#ifdef MQTT_EN
    publishData(sht45.temperature(),sht45.humidity());
#endif
  } else {
    Serial.println("SHT45 read error");
  }
  delay(1000);
}

