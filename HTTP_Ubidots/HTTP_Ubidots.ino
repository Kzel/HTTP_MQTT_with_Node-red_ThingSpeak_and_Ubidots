// Libraire
#include "Ubidots.h"
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Definir le DHT
#define DHTPIN 27   //Pin de DHT11
#define DHTTYPE    DHT11

const char* ssid = "your_WiFi_ssid";
const char* password = "your_WiFi_password";
const char *UBIDOTS_TOKEN = "your_token_here";

const char *DEVICE_LABEL = "esp32http"; 
const char *VARIABLE_LABEL1 = "temp";
const char *VARIABLE_LABEL2 = "humd";

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
// Ubidots ubidots(UBIDOTS_TOKEN, UBI_TCP); // Uncomment to use TCP
// Ubidots ubidots(UBIDOTS_TOKEN, UBI_UDP); // Uncomment to use UDP

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
    Serial.println("Humidity est");
    Serial.println(h);
    return h;
  }
}

void setup() {
  Serial.begin(115200);
  
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting your Wifi\n");
  // }
  // if(WiFi.status() == WL_CONNECTED){
  //   Serial.println("Successful!\n");
  // }
  // Serial.println(WiFi.localIP());
  ubidots.wifiConnect(ssid, password);
  dht.begin();
}

void loop() {

 
    float temp = readDHTTemperature();
    float humd = readDHTHumidity();
    ubidots.add(VARIABLE_LABEL1, temp);
    ubidots.add(VARIABLE_LABEL2, humd);
    bool bufferSent = false;
    bufferSent = ubidots.send();
    if (bufferSent){
    // Do something if values were sent properly
      Serial.println("Values sent by the device");
    }
    delay(5000);
}
