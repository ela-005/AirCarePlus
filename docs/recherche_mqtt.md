# Étude Technique : Protocole MQTT & Architecture Réseau

## 1. Introduction et Contexte
Dans le cadre du projet **AirCare+**, l'objectif est de concevoir un pipeline de données fiable, capable de transporter des métriques environnementales (température et humidité issues d'un capteur DHT22) depuis un microcontrôleur ESP32 vers une interface de supervision (Dashboard).

Pour répondre aux contraintes d'efficacité énergétique, de bande passante limitée et de latence réseau propre aux systèmes embarqués, le protocole **MQTT (Message Queuing Telemetry Transport)** a été sélectionné comme couche applicative de transport.

---

## 2. Principes Fondamentaux du Protocole MQTT
MQTT est un protocole de messagerie léger basé sur le modèle conceptuel **Publication/Abonnement (Pub/Sub)**, fonctionnant au-dessus de la pile TCP/IP. Contrairement à l'architecture classique Client/Serveur (HTTP Request/Response), MQTT découple totalement les producteurs de données des consommateurs.

### 2.1 Les Acteurs de la Topologie
* **Le Client (Publisher) :** Dans notre architecture, l'ESP32 agit en tant que client éditeur. Il n'attend pas de requêtes, il pousse les données de télémétrie dès qu'elles sont échantillonnées.
* **Le Client (Subscriber) :** Le Dashboard (ou l'application de collecte) agit en tant que client abonné. Il maintient une écoute passive et traite les données asynchrones dès leur réception.
* **Le Broker (Le Serveur Central) :** Pivot central du réseau (ex: *Eclipse Mosquitto*). Ses rôles incluent la gestion des sessions TCP, le filtrage des messages par sujet, l'authentification des clients et le routage des charges utiles (*payloads*).

### 2.2 Structuration des données : Les Topics
Le routage des messages repose sur des **Topics** (sujets), structurés de manière hiérarchique à l'aide du séparateur de niveau `/`. Cette arborescence permet un filtrage granulaire et l'utilisation de caractères génériques (*wildcards* : `+` pour un niveau unique, `#` pour tous les sous-niveaux).

---

## 3. Mécanismes Avancés de Fiabilité et Robustesse
Pour garantir l'intégrité de la solution AirCare+ face aux instabilités des réseaux sans fil (Wi-Fi), MQTT intègre trois mécanismes fondamentaux indispensables à documenter :

### 3.1 Qualité de Service (QoS - Quality of Service)
MQTT propose trois niveaux de compromis entre la fiabilité de livraison et la consommation de ressources réseau :

| Niveau | Désignation | Mécanisme technique | Cas d'usage typique |
| :--- | :--- | :--- | :--- |
| **QoS 0** | *At most once* (Au plus une fois) | Le message est envoyé sans accusé de réception (Fire-and-forget). Risque de perte si coupure. | Télémétrie haute fréquence non critique. |
| **QoS 1** | *At least once* (Au moins une fois) | Le broker accuse réception (*PUBACK*). L'ESP32 réémet tant qu'il n'a pas le reçu. Doublons possibles. | **Retenu pour AirCare+** (Garantit qu'aucune mesure n'est perdue). |
| **QoS 2** | *Exactly once* (Exactement une fois) | Handshake en 4 étapes (*PUBLISH*, *PUBREC*, *PUBREL*, *PUBCOMP*). Aucun doublon. | Transactions critiques ou commandes industrielles critiques. |

### 3.2 Gestion des Ruptures de Connexion : Retain & LWT
* **Retained Messages (Messages persistants) :** En publiant une configuration avec le drapeau `retain=true`, le Broker mémorise la dernière valeur connue. Lorsqu'un nouveau Dashboard se connecte, il reçoit instantanément l'état actuel sans attendre le prochain cycle d'envoi de l'ESP32.
* **LWT (Last Will and Testament) :** Permet de détecter la défaillance d'un nœud. Lors de sa connexion, l'ESP32 enregistre un "testament" auprès du Broker (ex: `AIRCAREPLUS/device01/status = "offline"`). Si l'ESP32 subit une panne de batterie ou une coupure Wi-Fi brutale, le Broker publie automatiquement ce message d'erreur aux abonnés.

---

## 4. Diagramme de Connexion et Flux de Données
L'architecture logique du pipeline de données AirCare+ est structurée en trois couches : **Collecte matérielle**, **Routage intermédiaire**, et **Restitution applicative**.
![Architecture Réseau et Pipeline MQTT](../docs/mqtt_architecture.png)

### Description cinématique du flux d'information :
1. **Échantillonnage :** L'ESP32 interroge le composant DHT22 via une liaison numérique série toutes les 5 secondes (selon les spécifications du firmware).
2. **Sérialisation :** Les variables de température et d'humidité sont encapsulées dans une charge utile formatée (texte optimisé ou objet JSON).
3. **Transit Amont (Ingestion) :** L'ESP32 publie le payload vers l'adresse IP du Broker sur le port par défaut non sécurisé `1883` via le protocole d'application MQTT, sous le topic ciblé : `AIRCAREPLUS/device01/telemetry`.
4. **Aiguillage :** Le Broker Mosquitto analyse la structure du topic, vérifie les tables d'abonnements actives, et isole le message.
5. **Transit Aval (Restitution) :** Le Broker pousse de manière asynchrone la nouvelle charge utile vers le Dashboard connecté ayant souscrit au topic de télémétrie.

---

## 5. Analyse Comparative : Pourquoi MQTT plutôt que HTTP ?
Pour justifier le choix d'ingénierie d'AirCare+, le tableau suivant synthétise les écarts de performance par rapport au protocole Web standard HTTP/REST :

| Critères d'évaluation | HTTP (REST / API) | MQTT (Protocole IoT) |
| :--- | :--- | :--- |
| **Modèle d'échange** | Synchrone (Request / Response) | Asynchrone (Publish / Subscribe) |
| **Poids de l'en-tête (Overhead)** | Conséquent (Plusieurs centaines d'octets de chaînes de caractères) | Très faible (En-tête fixe de 2 octets au minimum) |
| **Consommation énergétique** | Élevée (Obligation d'ouvrir/fermer une session TCP à chaque requête) | Optimisée (Session TCP persistante maintenue par *Keep-Alive*) |
| **Mode de mise à jour** | *Polling* régulier (Le client doit interroger en boucle le serveur) | En temps réel (*Push* instantané initié par le Broker) |