#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Started!");
  Serial.println("Initialisation du capteur DHT22...");
  dht.begin();
}

void loop() {
  delay(5000); // Espace de 5 secondes entre chaque mesure
  
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  
  if (isnan(hum) || isnan(temp)){
    Serial.println("ERROR!");
    return; // On évite d'envoyer des données fausses si le capteur bugge
  }

  // Code corrigé avec les doubles espaces ("  |  ") pour ton script Python
  // Format strict pour ton script Python d'origine
  Serial.print("Humidite: ");
  Serial.print(hum);
  Serial.print("%  |  Temperature: ");
  Serial.print(temp);
  Serial.println("°C");
}
