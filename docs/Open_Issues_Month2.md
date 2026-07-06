# Registre des Évolutions et Améliorations (Backlog Mois 2) - AirCarePlus

Ce document recense les axes d'amélioration techniques et les innovations stratégiques identifiées à la clôture du Mois 1 pour la solution AirCarePlus.

---

## 1. Plan de Priorisation (Feuille de Route Unique - Mois 2)

| ID | Catégorie | Amélioration Spécifique | Priorité | Impact Produit & Métier |
| :--- | :--- | :--- | :--- | :--- |
| **REQ-01** | **Stockage** | Migrer du fichier CSV vers une base de données locale (SQLite). | **Haute** | Permet de stocker de grands volumes de données de manière robuste et structurée. |
| **REQ-02** | **Interface** | Développer une application de contrôle graphique en pur Python (GUI). | **Haute** | Permet aux techniciens de visualiser les courbes de l'historique en temps réel. |
| **REQ-03** | **Sécurité & Accès** | Activer le chiffrement des messages et imposer un mot de passe sur le Broker MQTT. | **Haute** | Sécurise les flux de l'entreprise et empêche l'interception des données environnementales. |
| **REQ-04** | **Intelligence** | Implémenter le stockage local sur l'ESP32 en cas de coupure (Mode Boîte Noire). | **Haute** | Garantie de zéro perte de données lors des pannes de réseau Wi-Fi de l'entreprise. |
| **REQ-05** | **Automatisation** | Intégrer des relais pour le contrôle automatique (Ventilateur / Chauffage / Humidificateur). | **Haute** | Rend le système autonome capable d'agir en temps réel selon les seuils critiques détectés. |
| **REQ-06** | **Gestion de Flotte** | Développer la configuration initiale du Wi-Fi par Bluetooth (BLE). | **Moyenne** | Permet à n'importe quel opérateur de déployer le boîtier sur le terrain sans toucher au code. |
| **REQ-07** | **Optimisation Énergie** | Activer le mode veille de l'ESP32 entre deux mesures (Deep Sleep). | **Moyenne** | Réduit drastiquement la consommation pour un futur fonctionnement autonome sur pile. |
| **REQ-08** | **Système d'Alerte** | Automatiser l'envoi d'e-mails ou de notifications en cas de dépassement de seuil. | **Moyenne** | Alerte immédiatement les équipes en cas d'anomalie critique dans les locaux. |
| **REQ-09** | **Gestion de Flotte** | Intégrer la mise à jour logicielle centralisée et à distance (Secure OTA). | **Basse** | Permet de mettre à jour des centaines de capteurs simultanément via le réseau. |

---

## 2. Spécifications des Objectifs Techniques

### 2.1. Objectifs de Priorité Haute (Fondations, Automatisation & Sécurité)
* **REQ-01 (SQLite Storage) :** Remplacer l'écriture brute dans le fichier CSV par un moteur de base de données relationnelle léger côté serveur Python afin d'éviter la corruption de données.
* **REQ-02 (Application Python GUI) :** Création d'une interface graphique logicielle en Python pour afficher l'état de la pièce, les graphiques d'évolution et l'état des actionneurs.
* **REQ-03 (MQTT Hardening) :** Sécurisation de la couche transport réseau en forçant l'authentification par couple `Username/Password` et en préparant le chiffrement TLS sur le Broker Mosquitto.
* **REQ-04 (Mode Boîte Noire) :** Programmation de la mémoire flash interne de l'ESP32 (`LittleFS`) pour sauvegarder localement les mesures si le Wi-Fi est coupé, avec renvoi automatique des données dès le retour de la connexion.
* **REQ-05 (Contrôle par Relais) :** Connexion de modules relais physiques aux pins de l'ESP32 pour piloter des équipements industriels de régulation d'ambiance selon des seuils stricts :
  * *Température élevée ($> 26^\circ\text{C}$)* $\rightarrow$ Activation d'un Ventilateur.
  * *Température basse ($< 19^\circ\text{C}$)* $\rightarrow$ Activation d'un Chauffage.
  * *Humidité critique ($< 35\%$ ou $> 65\%$)* $\rightarrow$ Activation d'un Humidificateur ou extracteur d'air.

### 2.2. Objectifs de Priorité Moyenne (Autonomie & Déploiement Terrain)
* **REQ-06 (Configuration Bluetooth BLE) :** Utilisation du module Bluetooth de l’ESP32 au premier démarrage pour configurer les accès réseaux de manière sécurisée sans flasher le code.
* **REQ-07 (Deep Sleep Mode) :** Coupure logicielle temporaire du processeur de l'ESP32 entre chaque intervalle de relevé pour économiser l'énergie.
* **REQ-08 (Alerting Module) :** Script Python d'écoute capable de déclencher l'envoi de mails automatisés en cas de panne ou d'anomalie persistante.

### 2.3. Objectifs de Priorité Basse (Évolutions Haut de Gamme)
* **REQ-09 (Mises à jour OTA) :** Architecture permettant de déployer à distance les nouvelles versions de code sur l'intégralité du parc de capteurs de l'entreprise.