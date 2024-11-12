***
# MQTT

Ce répertoire contient des programmes et des fichiers associés à MQTT, un protocole de messagerie machine à machine largement utilisé dans l'Internet des objets (IoT).
***
## Contenu

- **MQTT/** : Ce répertoire contient des programmes et des fichiers liés à MQTT.
  - **Sensor/** : Ce répertoire contient des exemples de programmes pour des capteurs MQTT.
    - **mqtts-motion-sensor.ino** : Programme Arduino pour un capteur de mouvement MQTT.
    - **mqtts-gth-sensor** : Programme Arduino pour un capteur de gaz, température et humidité MQTT basé sur ESP8266.
  - **mosquitto-broker** : Permet l'éxecution du broker Mosquitto, un serveur MQTT open source.
  - **mosquitto-certs** : Générateur de certificats pour la sécurisation du broker Mosquitto avec SSL/TLS.
  - **mosquitto-client-pub** : Client MQTT simplifié pour publier des messages sur un broker.
  - **mosquitto-client-sub** : Client MQTT simplifié pour s'abonner à des topics sur un broker.
  - **mosquitto-install** : Script d'installation de Mosquitto pour faciliter le déploiement.
***
## Description

Ce répertoire vise à faciliter le déploiement et le développement de systèmes utilisant MQTT pour la communication entre capteurs et autres appareils IoT. Il offre une variété de ressources, y compris des exemples de programmes et des outils de configuration pour un broker Mosquitto.
***
## Utilisation

1. **Clonage du répertoire** : Clonez ce répertoire sur votre machine locale pour accéder à tous les fichiers et programmes.
2. **Exploration des répertoires** : Explorez les différents répertoires pour trouver les programmes et fichiers pertinents en fonction de vos besoins.
3. **Configuration des capteurs MQTT** : Utilisez les programmes Arduino fournis dans le répertoire `sensor/` pour configurer vos propres capteurs MQTT en fonction de vos besoins spécifiques.
4. **Installation de Mosquitto** : Consultez le script `mosquitto-install` pour installer et configurer un broker Mosquitto sur votre système.

Plus de détails ci-dessous.

### Capteur de mouvement MQTT (mqtt-motion-sensor.ino)

Ce programme est destiné à être utilisé avec un capteur de mouvement connecté à un microcontrôleur ESP8266. Il envoie des messages MQTT lorsqu'il détecte un mouvement ou lorsque aucun mouvement n'est détecté pendant une certaine période. Avant d'exécuter ce script, assurez-vous de :

- Avoir configuré votre microcontrôleur ESP8266 avec les bibliothèques requises.
- Avoir correctement configuré les paramètres WiFi, les identifiants MQTT et l'adresse IP du serveur MQTT dans le script.
- Assurez-vous également que votre broker MQTT est opérationnel et accessible depuis votre réseau.

### Capteur de gaz, température et humidité MQTT (mqtt-gth-sensor.ino)

Ce programme est conçu pour être utilisé avec un capteur combiné de gaz, de température et d'humidité connecté à un microcontrôleur ESP8266. Il envoie des messages MQTT avec les lectures de température, d'humidité et de gaz à intervalles réguliers. Avant d'exécuter ce script, assurez-vous de :

- Avoir configuré votre microcontrôleur ESP8266 avec les bibliothèques requises.
- Avoir correctement configuré les paramètres WiFi, les identifiants MQTT et l'adresse IP du serveur MQTT dans le script.
- Avoir connecté le capteur de gaz, température et humidité à votre microcontrôleur selon les spécifications du fabricant.
- Assurez-vous également que votre broker MQTT est opérationnel et accessible depuis votre réseau.

### Broker Mosquitto (mosquitto-broker)

Les fichiers de configuration contenus dans ce répertoire sont destinés à être utilisés avec le broker Mosquitto, un serveur MQTT open source. Vous pouvez les utiliser pour configurer et personnaliser votre propre broker Mosquitto en fonction de vos besoins spécifiques.

### Certificats SSL/TLS pour Mosquitto (mosquitto-certs)

Ces certificats sont utilisés pour sécuriser le broker Mosquitto en utilisant SSL/TLS. Vous pouvez les générer et les configurer pour garantir des communications sécurisées entre les clients MQTT et le broker.

### Client MQTT pour la publication de messages (mosquitto-client-pub)

Ce client MQTT est utilisé pour publier des messages sur un broker MQTT. Vous pouvez l'utiliser pour envoyer des données depuis vos appareils IoT vers le broker MQTT, où elles peuvent être consommées par d'autres appareils ou applications.

### Client MQTT pour l'abonnement à des topics (mosquitto-client-sub)

Ce client MQTT est utilisé pour s'abonner à des topics sur un broker MQTT. Vous pouvez l'utiliser pour écouter les données publiées sur des topics spécifiques et réagir en conséquence dans vos propres applications ou appareils IoT.

### Script d'installation de Mosquitto (mosquitto-install)

Ce script facilite l'installation et la configuration de Mosquitto sur votre système. Vous pouvez l'utiliser pour déployer rapidement un broker MQTT fonctionnel et prêt à l'emploi, avec les options de configuration et de sécurité appropriées.
***
## Contribution

Les contributions sont les bienvenues ! Si vous souhaitez ajouter des exemples de programmes supplémentaires, des améliorations de documentation ou tout autre apport, n'hésitez pas à ouvrir une pull request.
***
## Licence

Ce projet est sous licence MIT, ce qui signifie que vous êtes libre de l'utiliser, de le modifier et de le distribuer selon vos besoins.
***
