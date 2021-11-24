#include "WiFi.h"
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 27 //Pin of DHT11
#define DHTTYPE DHT11

const char* ssid = "your_wifi_ssid"; 
const char* password = "your_wifi_password"; 

// The domain name with URL path for HTTP POST
const char* serveurNom = "http://api.thingspeak.com/update";

// Your ThingSpeak write API key
String api_cle = "6YDWJ1AMR5AKF8DO";

// By default, the timer is 5s and last time is 0s, you can change this timer
unsigned long dureeTemp = 0;
unsigned long timerDelai = 10000;


DHT dht(DHTPIN, DHTTYPE);


float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read DHT!");
    return NULL;
  }
  else {
    Serial.println("Temprature is")+ Serial.println(t);
    return t;
  }
}

// Pour lire la temperature par capteur DHT11
float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read DHT!");
    return NULL;
  }
  else {
    Serial.println("Humidity is") + Serial.println(h);
    return h;
  }
}

void setup() {
 
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     Serial.println("Connecting to your Wifi\n");
   }
   if(WiFi.status() == WL_CONNECTED){
     Serial.println("Successful!\n");
   }
   Serial.println(WiFi.localIP());
  dht.begin();
}

void loop() {
    

    if ((millis() - dureeTemp) > timerDelai) {
    // Verify WiFi connexion status
      if(WiFi.status()== WL_CONNECTED){
        
        float temp = readDHTTemperature();
        float humd = readDHTHumidity();
        WiFiClient client;
        HTTPClient http;
  
        
        http.begin(client, serveurNom);
  
        
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        
        String httpRequestData = "api_key=" + api_cle + "&field1=" + String(temp) + "&field2=" + String(humd);
                  
  
        int httpResponseCode = http.POST(httpRequestData);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
  
        http.end();
        
      }else {
      Serial.println("WiFi Disconnected");
    }
    dureeTemp = millis();
  }
}
