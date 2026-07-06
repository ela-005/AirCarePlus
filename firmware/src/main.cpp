#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>          
#include <PubSubClient.h>  
#include <ArduinoJson.h>   
#include <esp_task_wdt.h> 

#define DHTPIN 15
#define DHTTYPE DHT22
#define WDT_TIMEOUT 10 // watchdog configuré à 10 secondes

DHT dht(DHTPIN, DHTTYPE);

// Configuration Réseau & MQTT
const char* ssid = "Wokwi-GUEST";             
const char* password = "";                    
const char* mqtt_server = "192.168.56.1";      
const int mqtt_port = 1883;                   

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;                    

// connecter l'ESP32 au Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println("\n--- Connexion initiale au Wi-Fi ---");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi connecté !");
}

// reconnect on Wi-Fi drop
void reconnect() {
  // 1. GESTION DE LA PERTE DE WI-FI
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[ERREUR] Wi-Fi perdu ! Tentative de reconnexion...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    
    int retries = 0;
    // On tente de se reconnecter pendant 5 secondes
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      delay(500);
      Serial.print(".");
      esp_task_wdt_reset(); 
      retries++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n[SUCCÈS] Wi-Fi retrouvé !");
    } else {
      Serial.println("\n[ÉCHEC] Wi-Fi indisponible, nouvel essai au prochain cycle.");
      return; 
    }
  }

  // 2. GESTION DE LA PERTE DE BROKER MQTT 
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    static unsigned long lastReconnectAttempt = 0;
    unsigned long now = millis();
    
    //  accès toutes les 5s max
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      
      Serial.print("Tentative de connexion MQTT...");
      String clientId = "ESP32-AirCare-";
      clientId += String(random(0xffff), HEX); 
      
      if (client.connect(clientId.c_str())) {
        Serial.println("Connecté au Broker MQTT !");
      } else {
        Serial.print("Échec, code erreur = ");
        Serial.print(client.state());
        Serial.println(" -> Prochaine tentative dans 5 secondes.");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Started!");
  Serial.println("Initialisation du capteur DHT22...");
  dht.begin();
  
  setup_wifi();                             
  client.setServer(mqtt_server, mqtt_port); 

  // INITIALISATION DU WATCHDOG TIMER
  Serial.println("Activation du Watchdog Timer...");
  esp_task_wdt_init(WDT_TIMEOUT, true); 
  esp_task_wdt_add(NULL);               
}

void loop() {
  
  reconnect();
  
  if (client.connected()) {
    client.loop(); // Garde la connexion MQTT active seulement si on est connecté.
  }

  // chrono de 5s pour le DHT22
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now; 

    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    
    if (isnan(hum) || isnan(temp)){
      Serial.println("ERROR: Impossible de lire le capteur DHT22 !");
      return; 
    }

    JsonDocument doc;
    doc["temperature"] = temp;
    doc["humidity"] = hum;

    char buffer[256];
    serializeJson(doc, buffer);

    if (client.connected()) {
      Serial.print(" Envoi au Broker MQTT : ");
      Serial.println(buffer);
      client.publish("aircare/sensors", buffer); 
    } else {
      Serial.println(" Données prêtes mais MQTT déconnecté. Envoi annulé.");
    }
  }

  // Si le code tourne normalement sans freezer, on réinitialise le compte à rebours de 10s
  esp_task_wdt_reset();
}