#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>          
#include <PubSubClient.h>  
#include <ArduinoJson.h>   

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Configuration Réseau & MQTT
const char* ssid = "Wokwi-GUEST";             // Wi-Fi gratuit et virtuel de Wokwi
const char* password = "";                    // Pas de mot de passe sur Wokwi
const char* mqtt_server = "192.168.56.1";      // L'ADRESSE IP DE mon PC 
const int mqtt_port = 1883;                   // Port standard de Mosquitto

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;                    // Nouveau chrono pour remplacer le delay()

// Fonction pour connecter l'ESP32 au Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println("\n--- Connexion au Wi-Fi ---");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi connecté !");
}

// Fonction pour se connecter (ou se reconnecter) au Broker Mosquitto
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    String clientId = "ESP32-AirCare-";
    clientId += String(random(0xffff), HEX); // Génère un nom unique pour ESP32
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Connecté au Broker MQTT !");
    } else {
      Serial.print("Échec, code erreur = ");
      Serial.print(client.state());
      Serial.println(" -> Nouvelle tentative dans 5 secondes.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Started!");
  Serial.println("Initialisation du capteur DHT22...");
  dht.begin();
  
  setup_wifi();                           // Active le Wi-Fi
  client.setServer(mqtt_server, mqtt_port); // Dit à l'ESP32 où est mon PC sur le réseau
}

void loop() {
  // Vérifie à chaque instant que l'ESP32 est bien connecté au broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Garde la connexion MQTT active

  // Notre chrono : est-ce que 5000 millisecondes (5s) se sont écoulées ?
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now; // On réinitialise le chrono

    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    
    if (isnan(hum) || isnan(temp)){
      Serial.println("ERROR!");
      return; 
    }

    // Au lieu d'écrire du texte brut sur le port Série...
    // On crée un petit fichier JSON : {"temperature": XX.X, "humidity": YY.Y}
    JsonDocument doc;
    doc["temperature"] = temp;
    doc["humidity"] = hum;

    // On transforme ce JSON en texte transmissible
    char buffer[256];
    serializeJson(doc, buffer);

    // ON ENVOIE DANS LE CANAL MQTT !
    Serial.print(" Envoi au Broker MQTT : ");
    Serial.println(buffer);
    client.publish("aircare/sensors", buffer); // Envoi officiel sur le groupe "aircare/sensors"
  }
}