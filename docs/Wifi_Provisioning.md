# Méthodes de Provisionnement Wi-Fi

## 1. Introduction
Dans le cadre du développement de notre système IoT **AirCarePlus**, la connectivité réseau est un élément critique. L'ESP32 doit pouvoir s'associer de manière fiable à un point d'accès Wi-Fi local pour publier ses messages MQTT. Nous étudions ici deux méthodes distinctes pour configurer ces identifiants réseau (SSID et mot de passe).

---

## 2. Analyse de la méthode "Hardcoded" (Codage en dur)
Actuellement, notre prototype utilise cette approche. Les identifiants réseau sont définis directement dans les variables globales du code source (`main.cpp`) sous forme de chaînes de caractères claires.

### Forces
* Implémentation logicielle immédiate et triviale (quelques lignes de code avec la fonction `WiFi.begin()`).
* Temps de démarrage et de connexion au réseau minime.

### Faiblesses
* **Risque de sécurité majeur :** L'exposition du code source (par exemple sur un dépôt public GitHub) divulgue instantanément les clés d'accès de l'infrastructure réseau.
* **Aucune flexibilité opérationnelle :** Si le produit est déplacé ou si la clé Wi-Fi change, l'appareil devient inutilisable. L'intervention d'un technicien est requise pour modifier le code et reflasher le firmware via un câble physique.

---

## 3. Analyse de la méthode "AP Config Portal" (Portail Captif)
Cette technique, largement utilisée sur les produits commerciaux, configure l'ESP32 pour qu'il agisse dynamiquement. S'il ne parvient pas à se connecter à un réseau enregistré, il bascule en mode Point d'Accès (AP) et héberge un mini serveur Web informatique.

### Forces
* **Excellente expérience utilisateur (UX) :** L'utilisateur se connecte au Wi-Fi temporaire de l'appareil avec son smartphone, et une interface web intuitive s'ouvre pour saisir les identifiants locaux.
* **Sécurisation des données privées :** Le développeur n'a jamais accès aux identifiants. Les clés saisies par l'utilisateur sont directement enregistrées dans la mémoire Flash non volatile (EEPROM/NVS) de la puce.

### Faiblesses
* Complexité logicielle accrue (nécessite l'intégration de bibliothèques de gestion d'état réseau comme `WiFiManager` et la gestion d'un serveur HTTP embarqué).

---

## 4. Tableau Comparatif des Compromis (Tradeoffs)

| Critères d'évaluation | Méthode Hardcoded  | Portail de Configuration (AP)  |
| :--- | :--- | :--- |
| **Complexité du code** | Très faible (Idéal en phase de R&D) | Modérée à élevée |
| **Sécurité des clés** | Critique (Stockage en texte clair) | Forte (Stockage en mémoire non-volatile) |
| **Expérience Client** | Très contraignante | Fluide et standardisée (Type Smart Home) |
| **Cycle de vie du produit** | Limité à un seul environnement fixe | Mobile, s'adapte à toutes les infrastructures |

---

## 5. Conclusion et Recommandation
Pour notre phase de prototypage actuelle sur le simulateur Wokwi, la méthode **Hardcoded** est parfaitement adaptée car elle accélère le déploiement. Cependant, pour une industrialisation ou un déploiement réel du boîtier **AirCarePlus**, l'intégration d'un **Portail Captif via AP** est impérative pour garantir la sécurité et l'autonomie de configuration par l'utilisateur final.