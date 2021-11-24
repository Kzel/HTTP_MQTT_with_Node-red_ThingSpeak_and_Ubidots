#include <WiFi.h>
#include "UbidotsEsp32Mqtt.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>


#define DHTPIN 27   //Pin of DHT11
#define DHTTYPE   DHT11

const char* ssid = "your_WiFi_ssid";
const char* password = "your_WiFi_password";
const char *UBIDOTS_TOKEN = "your_Ubidots_token";

const char *DEVICE_LABEL = "esp32"; 
const char *VARIABLE_LABEL1 = "temp";
const char *VARIABLE_LABEL2 = "humd";

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds
unsigned long timer;
Ubidots ubidots(UBIDOTS_TOKEN);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

DHT dht(DHTPIN, DHTTYPE);

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read DHT!");
    return NULL;
  }
  else {
    Serial.println("Temprature is");
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read DHT!");
    return NULL;
  }
  else {
    Serial.println("Humidity is");
    Serial.println(h);
    return h;
  }
}

void setup() {
  Serial.begin(115200);
  ubidots.connectToWifi(ssid, password);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  timer = millis();
  dht.begin();
}

void loop() {
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float temp = readDHTTemperature();
    float humd = readDHTHumidity();
    ubidots.add(VARIABLE_LABEL1, temp);
    ubidots.add(VARIABLE_LABEL2, humd);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
 
}
