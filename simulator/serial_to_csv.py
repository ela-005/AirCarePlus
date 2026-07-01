import csv
import os
import time
from datetime import datetime
import serial
import serial.tools.list_ports

# --- CONFIGURATION ---
SERIAL_PORT = 'COM5'  # Port relié au pont virtuel (Wokwi enverra sur COM6)
BAUD_RATE = 115200

# Détermination dynamique de la racine du projet
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE_DIR, "data")
CSV_FILE_PATH = os.path.join(DATA_DIR, "sensor_data.csv")

def initialiser_environnement():
    """Crée le dossier data et le fichier CSV avec ses en-têtes officiels."""
    os.makedirs(DATA_DIR, exist_ok=True)
    if not os.path.exists(CSV_FILE_PATH):
        with open(CSV_FILE_PATH, mode='w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file)
            writer.writerow(["Timestamp", "Temperature_C", "Humidity_Percent"])
        print(f"Fichier CSV créé : {CSV_FILE_PATH}")

def parser_et_enregistrer(ligne_brute):
    """Découpe proprement la chaîne reçue et l'ajoute au CSV."""
    # Format attendu : "Humidite:42.50% | Temperature:35.00°C"
    if "Humidite" in ligne_brute and "Temperature" in ligne_brute:
        current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        try:
            # Séparation par le pipe '|'
            elements = ligne_brute.split("|")
            
            # Nettoyage strict des chaînes de caractères
            h_str = elements[0].replace("Humidite:", "").replace("%", "").strip()
            t_str = elements[1].replace("Temperature:", "").replace("°C", "").strip()
            
            # Conversion de sécurité pour valider que ce sont bien des nombres
            humidity = float(h_str)
            temperature = float(t_str)
            
            # Écriture dans le CSV
            with open(CSV_FILE_PATH, mode='a', newline='', encoding='utf-8') as file:
                writer = csv.writer(file)
                writer.writerow([current_time, temperature, humidity])
                
            print(f"[Enregistré] {current_time} -> Temp: {temperature}°C | Hum: {humidity}%")
            
        except ValueError:
            print(f"Données corrompues ignorées : {ligne_brute}")
        except Exception as e:
            print(f"Erreur lors du traitement : {e}")

def ecouter_liaison_serie():
    """Gère la connexion et la lecture du port série de manière robuste."""
    print(f"Tentative d'ouverture du port {SERIAL_PORT}...")
    
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Temps de stabilisation de la liaison
        print(f"Connecté à {SERIAL_PORT}. Écoute active du flux ESP32...")
        
        while True:
            if ser.in_waiting > 0:
                # Lecture de la ligne, décodage et suppression des caractères invisibles (\r, \n)
                ligne = ser.readline().decode('utf-8', errors='ignore').strip()
                if ligne:
                    parser_et_enregistrer(ligne)
                    
            time.sleep(0.05)  # Pause légère pour ne pas surcharger le processeur
            
    except serial.SerialException:
        print(f"\n Impossible d'accéder au port {SERIAL_PORT}.")
        print("Vérifie que ton logiciel de port virtuel est activé (ON) et que Wokwi utilise l'autre port.")
        ports_dispos = [p.device for p in serial.tools.list_ports.comports()]
        print(f"Ports COM actuellement détectés sur ton PC : {ports_dispos}")

if __name__ == "__main__":
    try:
        initialiser_environnement()
        ecouter_liaison_serie()
    except KeyboardInterrupt:
        print("\n Script arrêté par l'utilisateur. Sortie propre.")