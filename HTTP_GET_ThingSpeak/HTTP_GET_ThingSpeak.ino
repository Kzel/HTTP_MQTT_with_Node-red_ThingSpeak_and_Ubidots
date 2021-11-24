// Library
#include <WiFi.h>
#include "ThingSpeak.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

WiFiServer server(80); 
const char* ssid = "your_WiFi_ssid"; 
const char* password = "your_WiFi_password"; 
WiFiClient  client;

const int httpPort = 80;
const unsigned long HTTP_TIMEOUT = 10000;  
// Your ThingSpeak API Read Key here
String api_cle = "read-key";

// Your channel ID
// If you use ThingSpeak API ThingSpeak.readFloatField uncomment below
//unsigned long  channel_ID = 1111111; 
// If you decode json manuel
String  channel_ID = "1111111"; // 

// The domain name with URL path and read API key
String serveurNom = "http://api.thingspeak.com/";

// By default, the timer is 5s and last time is 0s, you can change this timer
unsigned long dureeTemp = 0;
unsigned long timerDelai = 5000;

void setup() {
  
  Serial.begin(115200);
  //ThingSpeak.begin(client); // uncomment to use ThingSpeak APi
  pinMode(LED_BUILTIN, OUTPUT);

    WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting your Wifi\n");
     }
     if(WiFi.status() == WL_CONNECTED){
       Serial.println("Successful!\n");
     }
    Serial.println(WiFi.localIP());
}

void loop() {
  
    if ((millis() - dureeTemp) > timerDelai) {
    // Verify WiFi connexion status
      if(WiFi.status()== WL_CONNECTED){
        client = server.available();
        static char responseBuffer[1024]; 
        int swit;

        String url = "/channels/" + channel_ID; // Start building API request string
        url += "/fields/3.json?api_key"+api_cle+"&results=1";  // 5 is the results request number, so 5 are returned, 1 woudl return the last result received
  
        //swit = ThingSpeak.readFloatField(channel_ID, 3, api_cle); //uncomment to switch LED built in by GET
        while (!skipResponseHeaders());
        while (client.available()) {                         // Now receive the data
        String line = client.readStringUntil('\n');
        if ( line.indexOf('{',0) >= 0 ) {                  // Ignore data that is not likely to be JSON formatted, so must contain a '{'
          Serial.println(line);                            // Show the text received
          line.toCharArray(responseBuffer, line.length()); // Convert to char array for the JSON decoder
          swit = decodeJSON(responseBuffer);                      // Decode the JSON text
          }
        }
        Serial.println(swit);
        if(swit) {
          Serial.print("LED is ON\n");
          digitalWrite(LED_BUILTIN, HIGH);
         }else{
          Serial.print("LED is OFF\n");
          digitalWrite(LED_BUILTIN, LOW);
         }
          
      }else {
      Serial.println("WiFi Disconnected");
    }
   dureeTemp = millis();
  }
}

bool skipResponseHeaders() { 
  char endOfHeaders[] = "\r\n\r\n"; // HTTP headers end with an empty line 
  client.setTimeout(HTTP_TIMEOUT); 
  bool ok = client.find(endOfHeaders); 
  if (!ok) { Serial.println("No response or invalid response!"); } 
  return ok; 
} 

int decodeJSON(char *json) {
  StaticJsonBuffer <1024> jsonBuffer;
  char *jsonstart = strchr(json, '{'); // Skip characters until first '{' found and ignore length, if present
  if (jsonstart == NULL) {
    Serial.println("JSON data missing");
    return false;
  }
  json = jsonstart;
  JsonObject& root = jsonBuffer.parseObject(json); // Parse JSON
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }
  JsonObject& root_data = root["channel"]; // Begins and ends within first set of { }
  String id   = root_data["id"];
  String name = root_data["name"];
  String field1_name = root_data["field3"];
  String datetime    = root_data["updated_at"];
  Serial.println("\n\n Channel id: "+id+" Name: "+ name);
  Serial.println(" Readings last updated at: "+datetime);
  
  
  JsonObject& channel = root["feeds"][0]; // Now we can read 'feeds' values and so-on
  String entry_id     = channel["entry_id"];
  String field3value  = channel["field3"];
  Serial.print(" Field3 entry number ["+entry_id+"] had a value of: ");
  Serial.println(field3value);
  return field3value.toInt();
}
