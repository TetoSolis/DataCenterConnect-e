# Système de surveillance IoT pour datacenters

## Description

Ce projet implémente un système IoT (Internet of Things) permettant la surveillance et la gestion d'accès d'un datacenter à l'aide de divers capteurs et technologies modernes. Les données collectées sont transmises, analysées et visualisées en temps réel grâce à des outils comme Node-RED, InfluxDB et Grafana.

## Fonctionnalités

- **Surveillance de l'air** : Détection des gaz nocifs (CO2) avec le capteur MQ135.
- **Contrôle des conditions environnementales** : Surveillance de la température et de l'humidité via DHT11.
- **Détection de présence** : Gestion de la sécurité et optimisation énergétique avec des capteurs de mouvement (HC-SR501).
- **Gestion des accès sécurisés** : Authentification basée sur le module RFID/NFC.
- **Visualisation des données** : Tableaux de bord dynamiques via Grafana.

## Matériel requis

- 3 x Capteurs de fumée MQ135  
- 3 x Capteurs de température et d'humidité DHT11  
- 2 x Capteurs de présence HC-SR501  
- 1 x Module RFID MFRC522  
- 2 x Raspberry Pi 4 (avec cartes SD)  
- 3 x ESP8266  
- 1 x Switch réseau  
- 1 x Routeur Linksys  
- Autres : câbles, LED, résistances, plaques de connexion, alimentation.

## Logiciels utilisés

- **Node-RED** : Programmation visuelle pour intégrer capteurs et systèmes.
- **InfluxDB** : Base de données pour la gestion des séries temporelles.
- **Grafana** : Outil de visualisation pour les tableaux de bord.
- **Protocole MQTT** : Communication entre les dispositifs.

## Installation

### 1. Configuration matérielle
Connectez les capteurs et modules aux microcontrôleurs (ESP8266) et Raspberry Pi en suivant les schémas fournis dans la documentation.

### 2. Configuration logicielle
1. Clonez ce dépôt :
   ```bash
   git clone https://github.com/username/repository.git
   cd repository
   ```
2. Installez et configurez Mosquitto pour le protocole MQTT avec des certificats SSL :
 ```bash
   ./scripts/mosquitto-install.sh
   ```
3. Déployez les flux Node-RED disponibles dans le dossier `flows/`.

### 3. Téléversement des codes

Téléversez les fichiers `.ino` sur les ESP8266 via l'IDE Arduino, en configurant les identifiants Wi-Fi et MQTT.

### 4. Base de données et visualisation

1. Configurez InfluxDB avec les schémas fournis.
2. Importez le tableau de bord Grafana depuis le fichier `dashboards/datacenter.json`.

## Schéma de communication

1. Les capteurs transmettent les données aux ESP8266.
2. Ces données sont envoyées au serveur MQTT (Mosquitto).
3. Node-RED reçoit les données via MQTT et les transmet à InfluxDB.
4. Grafana récupère les données d'InfluxDB pour les visualiser.

## Aperçu du tableau de bord

## Annexes

- **Code Arduino** : Voir `codes/`.
- **Scripts d'installation** : Voir `scripts/`.

## Contributeurs

- Guillaume et l'équipe IoT.

## Licence

Ce projet est sous licence MIT. Consultez le fichier `LICENSE` pour plus de détails.

N'hésitez pas à me dire si vous souhaitez personnaliser davantage ce fichier.
