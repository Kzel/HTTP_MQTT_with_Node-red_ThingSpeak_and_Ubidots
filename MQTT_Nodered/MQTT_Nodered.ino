// Libraire
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Definir le DHT11
#define DHTPIN 27 //Pin de DHT11
#define DHTTYPE DHT11 //Type de DHT

// Replace the next variables with your SSID/Password combination

const char* ssid = "Kzels"; // Remplacer avec votre ssid de Wifi
const char* password = "paopao12"; // Remplacer avec votre mot de passe de Wifi

/*Le nom du domaine avec url complet pour MQTT
  Le serveur est en Cloud, vous pouvez acceder directement 
  par http://20.91.130.117:1880 pour regareder les workflows deja configures
  Pour regarder dashboard acceder http://20.91.130.117:1880/ui
*/
const char* mqtt_serveur = "20.91.130.117"; // L'adresse IP du Cloud

// Le client et le dernier message
WiFiClient espClient;
PubSubClient client(espClient);
long dernier_mesg = 0;

//La declaraction du capteur DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidite = 0;


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
  
  // Positionner le serveur avec le port 1883 et le callback
  client.setServer(mqtt_serveur, 1883);
  client.setCallback(callback);
}


void callback(char* topic, byte* message, unsigned int length) {

  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "esp32/led") {
    Serial.print("Changer le switch du LED a ");
    // Allumer le LED si le switch est en ON
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    // Eteindre le LED si le switch est en OFF
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(LED_BUILTIN, LOW);

    }
  }
}

void reconnect() {
  // Boucle jusqu'a reconncter
  while (!client.connected()) {
    Serial.print("Tenter la connection de MQTT..");
    // Tenter a reconnecter

    if (client.connect("ESP32Client")) {
      Serial.println("Connecte!");
      // Subscribe le topic du LED
      client.subscribe("esp32/led");

    } else {
      Serial.print("Echec, Return Code=");
      Serial.print(client.state());
      Serial.println(" Reessayer apres 5s");
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long actuel = millis();
  if (actuel - dernier_mesg > 5000) {
    dernier_mesg = actuel;
    
    // Lire la temperature par DHT11
    temperature = readDHTTemperature();

    // Convertir la temperature en array de char et Publish dans le topic de temperature
    char tempChar[8];
    dtostrf(temperature, 1, 2, tempChar);
    client.publish("esp32/temperature", tempChar);

    humidite = readDHTHumidity();
    
    // Convertir l'humidite en array de char et Publish dans le topic de temperature
    char humdChar[8];
    dtostrf(humidite, 1, 2, humdChar);
    client.publish("esp32/humidite", humdChar);
  }
}
