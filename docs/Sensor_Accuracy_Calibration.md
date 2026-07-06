# Spécifications, Précision et Dérive du Capteur DHT22

## 1. Contexte et Objectif
Dans le cadre de la fiabilisation du projet **AirCarePlus**, il est crucial de qualifier la qualité des mesures remontées par notre matériel à la périphérie (Edge). Ce document liste les spécifications d'exactitude (accuracy) et analyse la dérive temporelle (drift) du capteur DHT22 face à un appareil de référence, garantissant l'intégrité des futures données de notre dashboard.

---

## 2. Spécifications Techniques Exactes (Datasheet Constructeur)

Le DHT22 (AM2302) intègre un capteur capacitif pour l'humidité et une thermistance de haute précision pour la température, couplés à un microcontrôleur 8-bit convertissant le signal en numérique.

### Mesure de la Température
* **Plage de fonctionnement :** -40°C à +80°C
* **Précision typique (Accuracy) :** **±0.5°C** (jusqu'à ±1°C en conditions extrêmes)
* **Résolution :** 0.1°C
* **Répétabilité :** ±0.2°C

### Mesure de l'Humidité Relative
* **Plage de fonctionnement :** 0% à 100% RH
* **Précision typique (Accuracy) :** **±2% RH** (à une température optimale de 25°C), pouvant dériver jusqu'à **±5% RH** aux extrêmes (proche de 0% ou 100% RH)
* **Résolution :** 0.1% RH
* **Répétabilité :** ±1% RH
* **Hystérésis :** ±0.3% RH

> **Fréquence d'interrogation :** La fiche technique indique un temps de réponse et de rafraîchissement minimal de **2 secondes**. Notre firmware interrogeant le composant toutes les 5 secondes (via `millis()`), nous respectons parfaitement cette contrainte matérielle pour éviter l'auto-échauffement de la sonde.

---

## 3. Analyse de la Dérive Temporelle (Drift)

La dérive désigne la perte de précision progressive due à la dégradation des composants internes au fil du temps :
* **Dérive Thermique :** Pratiquement nulle (< 0.1°C par an).
* **Dérive d'Humidité :** Estimée à **< 0.5% RH par an** en environnement normal. Cependant, une exposition à de fortes pollutions ou à une humidité saturée continue (> 90% RH) peut induire un décalage permanent de +3% RH.

---

## 4. Protocole Théorique de Calibration sur Matériel Réel

Puisque le prototypage actuel s'effectue sur le simulateur Wokwi (environnement numérique parfait sans distorsion), voici la procédure industrielle prévue lors du passage sur cible physique :

1. **Test de Comparaison :** Placer l'ESP32 et son DHT22 à côté d'un thermo-hygromètre étalonné de référence pendant 24 heures.
2. **Identification de l'Écart (Offset) :** Calculer la différence constante. Par exemple, si l'appareil étalon indique 22.0°C et le DHT22 transmet 23.2°C, un décalage de `+1.2°C` est identifié.
3. **Compensation Logicielle (Firmware) :** Appliquer l'ajustement dans le fichier `main.cpp` lors de la lecture :
   ```cpp
   float temp_calibree = dht.readTemperature() - 1.2; // Correction de l'offset