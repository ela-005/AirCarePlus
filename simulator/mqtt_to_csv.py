import os
import csv
import json
from datetime import datetime
import paho.mqtt.client as mqtt

# --- CONFIGURATION ---
MQTT_BROKER = "192.168.56.1"  
MQTT_PORT = 1883
MQTT_TOPIC = "aircare/sensors"

# Détection automatique des dossiers du projet
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE_DIR, "data")
CSV_FILE_PATH = os.path.join(DATA_DIR, "sensor_data.csv")

def initialiser_csv():
    """Crée le dossier data et le fichier CSV avec ses entêtes s'ils n'existent pas."""
    os.makedirs(DATA_DIR, exist_ok=True)
    if not os.path.exists(CSV_FILE_PATH):
        with open(CSV_FILE_PATH, mode='w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file)
            writer.writerow(["Timestamp", "Temperature_C", "Humidity_Percent"])
        print(f"Fichier CSV créé avec succès : {CSV_FILE_PATH}")

def on_connect(client, userdata, flags, rc, properties=None):
    """S'exécute automatiquement lorsque le script se connecte à Mosquitto."""
    if rc == 0:
        print("Script Python connecté au Broker Mosquitto !")
        # On s'abonne au canal de l'ESP32
        client.subscribe(MQTT_TOPIC)
        print(f"Écoute active sur le topic : '{MQTT_TOPIC}'...")
    else:
        print(f"Échec de connexion au broker, code retour : {rc}")

def on_message(client, userdata, msg):
    """S'exécute automatiquement à chaque fois qu'un message arrive sur aircare/sensors."""
    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    try:
        # 1. Décodage du texte JSON reçu
        payload = json.loads(msg.payload.decode('utf-8'))
        temp = payload.get("temperature")
        hum = payload.get("humidity")
        
        # 2. Écriture immédiate dans le fichier CSV
        with open(CSV_FILE_PATH, mode='a', newline='', encoding='utf-8') as file:
            writer = csv.writer(file)
            writer.writerow([current_time, temp, hum])
            
        print(f"[Donnée Reçue] {current_time} -> Temp: {temp}°C | Hum: {hum}% -> Sauvegardée !")
        
    except Exception as e:
        print(f"Erreur lors de la réception ou de l'écriture : {e}")

if __name__ == "__main__":
    # Préparation du fichier CSV
    initialiser_csv()
    
    # Configuration du client MQTT
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    
    print("Connexion au broker local en cours...")
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    
    # Lancement de l'écoute en boucle infinie
    try:
        client.loop_forever()
    except KeyboardInterrupt:
        print("\n Arrêt du script Python Subscriber.")