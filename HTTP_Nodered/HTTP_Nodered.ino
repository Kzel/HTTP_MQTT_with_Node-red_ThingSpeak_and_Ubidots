// Libraire
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Arduino_JSON.h>

// Definir le DHT11
#define DHTPIN 27 //Pin de DHT11
#define DHTTYPE DHT11 //Type de DHT

const char* ssid = ""; // Remplacer avec votre ssid de Wifi
const char* password = ""; // Remplacer avec votre mot de passe de Wifi
String api_cle ="Remplacer que vous voulez";

const char* serveurNom = "http://192.168.1.1:1880/update-sensor"; // maj les donnees de temperature et humidite
const char* serveurNom2 = "http://192.168.1.1:1880/get-sensor"; // obtenir le status de switch LED

// Le timer en default est 5s(5000ms) et le temp de duree est 0s, vous pouvez changer l'intervalle du temps
unsigned long dureeTemp = 0;
unsigned long timerDelai = 5000;

//La declaraction du capteur DHT11 et switch du LED
DHT dht(DHTPIN, DHTTYPE);
String switRead;
int swit;

// Pour lire la temperature par capteur DHT11
float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Echec a lire par DHT!");
    return NULL;
  }
  else {
    Serial.println("Temprature est")+ Serial.println(t);
    return t;
  }
}

// Pour lire la temperature par capteur DHT11
float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Echec a lire par DHT!");
    return NULL;
  }
  else {
    Serial.println("Humidite est") + Serial.println(h);
    return h;
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Le nom de domaine avec URL ou l'adresse de IP
  http.begin(client, serverName);
  
  // Envoyer HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Reponse code pour GET: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Erreur code: ");
    Serial.println(httpResponseCode);
  }
  // Liberer les resources
  http.end();

  return payload;
}

void setup() {
  // Le port serie est de 115200 bit/s, le LED en output et lancer DHT
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();

  // Connecter avec Wifi
  WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     Serial.println("En cours de connecter a Wifi\n");
   }
   if(WiFi.status() == WL_CONNECTED){
     Serial.println("Succes!\n");
   }
   Serial.println(WiFi.localIP());
  
}

void loop() {

    if ((millis() - dureeTemp) > timerDelai) {
    // Verifier WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        // La temperature et l'humidite par le capteur DHT11
        float temp = readDHTTemperature();
        float humd = readDHTHumidity();
        WiFiClient client;
        HTTPClient http;

        // Le nom de domaine avec URL ou l'adresse IP
        http.begin(client, serveurNom);
  
        // Specifier le header du type de contenue
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        // Envoyer donnees de temperature et humidite avec HTTP POST
        String httpRequestData = "api_key=" + api_cle + "&sensor=dht11"+ "&temperature=" + String(temp) + "&humidite=" + String(humd);
                  
        // Envoyer HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);
        Serial.print("HTTP Rsponse code pour POST: ");
        Serial.println(httpResponseCode);
  
        switRead = httpGETRequest(serveurNom2);
        
        JSONVar myObject = JSON.parse(switRead);

        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Echec de parser input!");
          return;
        }

        Serial.print("JSON object = ");
        Serial.println(myObject);
        // myObject.keys() peut utiliser a obternir un array de tous les keys dans l'objet
        JSONVar keys = myObject.keys();
      
        // Pour lire la valeur de switch LED
        for (int i = 0; i < keys.length(); i++) {
          JSONVar value = myObject[keys[i]];
          Serial.print(keys[i]);
          Serial.print(" = ");
          Serial.println(value);
          int swit = (int)value;

          // Si le switch du LED est 1, allumer le LED sinon eteindre
          if(swit == 1){
              digitalWrite(LED_BUILTIN, HIGH);
            }else{
              digitalWrite(LED_BUILTIN, LOW); 
          }        
        }

      }else {

      Serial.println("WiFi Disconnecte");

    }
      dureeTemp = millis();  
  }
}
