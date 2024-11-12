#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <MQ135.h>
#include <DHT11.h>

// Déclaration des constantes pour les broches et les paramètres du capteur DHT11
#define DHTPIN 4 // Broche du capteur DHT11 (GPIO2 ou D4 pour ESP8266)
const int GAS_PIN = 33; // Broche analogique à laquelle le capteur MQ135 est connecté
const int MOTION_PIN = 34;

// Création d'instances des capteurs
MQ135 gasSensor = MQ135(GAS_PIN);
DHT11 dht11(DHTPIN);

int lastMotionState = LOW;
unsigned long lastMotionTime = 0;
const unsigned long MOTION_TIMEOUT = 1 * 15 * 1000; // 5 minutes en millisecondes
bool motionMessageSent = false; // Variable pour garder une trace de l'envoi du message "0"
bool motionDetectedPreviously = false; // Variable pour garder une trace de l'état précédent du capteur
unsigned long lastChangeTime = 0; // Variable pour garder une trace du dernier changement d'état

// Certificat pour la connexion sécurisée MQTT
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDXzCCAkegAwIBAgIUUqVmTigzEdD+eM3cbSmzJSAjDVYwDQYJKoZIhvcNAQEL
BQAwPzELMAkGA1UEBhMCRlIxDDAKBgNVBAgMA0JGQzEUMBIGA1UEBwwLTW9udGJl
bGlhcmQxDDAKBgNVBAoMA0lVVDAeFw0yNDA0MDIwODUwNTBaFw0yOTA0MDIwODUw
NTBaMD8xCzAJBgNVBAYTAkZSMQwwCgYDVQQIDANCRkMxFDASBgNVBAcMC01vbnRi
ZWxpYXJkMQwwCgYDVQQKDANJVVQwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK
AoIBAQCyB9umJs1hPCWJ3ARI2XN8R+dBYuLK8HPV+NRTt9AtOMh98GOddvc7OQFK
lHYERBRksKotzLvV9PPMKsqP+1cfyOUSAj/yPVgIJoBkCIsT5yfWtYlX2ZpIp11G
MwaGvG2j8ws28IZXwfMyGwYQ1eMT9pFY3ukcKAC/nLs9/sk5ja9LXLUaaKQVJD9r
elBaqLAEFFE8bmJi9gQNqXGnO5w2e8veemP7d1Sgv3qyjNM+aJRhq8Kq+1GlM4Hs
Hgt/4tCJdaaWfkv9IoJy9HbKOlg3kNB2MGab2CZCR0dMRuWeFOtgKW6dAvRx8E/h
M40oNqe1ZbZARwOeQHZckKPzF9sxAgMBAAGjUzBRMB0GA1UdDgQWBBR5BM1A0mzb
M4ZYXX76by6HHoN9CjAfBgNVHSMEGDAWgBR5BM1A0mzbM4ZYXX76by6HHoN9CjAP
BgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAUMXELqYKkawUiPwrU
DafNbBnggWUGAuU4oiQe8dAGk6/usLq5hx64E4SU3jECRLiK2ifzE2wlEMBeVv2e
QGZIeL6k7JTDFuS+fTvokD3SQVn5qy0VhalqQF3dmtvg7m+c6I50ZMVEBsXdniMB
tA3XTyJeSaGWGtN2SMhIYrhJjpx/NcsLDqfh07B9txKJaKtfMiMlGI5Z/3PQ/7gr
ohxPIjHvomi03C/YA7PP1CuZdzerT9z6WReXtRcG40ddsspKg6MqnxeybtP+qoT2
POZe/GCOAGGsJS6MpLYmsM3XzG3kEJggJrpAQWUvl59b9m9G1ePJ+FfE/oHyb4PE
H6gS
-----END CERTIFICATE-----
)EOF";

// Paramètres du réseau WiFi
const char* ssid = "GRP-1-IOM";
const char* password = "Porygon-Z#1";

// Paramètres MQTT
const char* MqttUser = "GRP-1-IOM";
const char* MqttPass = "Porygon-Z#1";
const char* mqtt_server = "192.168.1.50";

// Définition des topics MQTT pour chaque type de données
const char* ESPName = "ESP-Green";
char mqtt_topic_temp[50];
char mqtt_topic_hum[50];
char mqtt_topic_gas[50];
char mqtt_topic_motion[50];

// Client MQTT et connexion sécurisée
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Configuration de la connexion WiFi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Fonction de réception des messages MQTT
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message reçu [");
  Serial.print(topic);
  //Serial.print("] ");
  
  // Convertir le payload en une chaîne de caractères
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
    char buffer[40];
  // Comparer la chaîne de caractères du payload avec "true" ou "false"
  if(payloadStr == "true"){
    // Allumer la LED
    digitalWrite(LED_BUILTIN, LOW);
    sprintf(buffer, "L'%s a allumé sa LED.", ESPName);
  } else if(payloadStr == "false"){
    // Eteindre la LED
    digitalWrite(LED_BUILTIN, HIGH);
    sprintf(buffer, "L'%s a éteint sa LED.", ESPName);
  }
  Serial.println(buffer);
}

void reconnect() {
  // Tentative de reconnexion
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Génération d'un ID client aléatoire
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Tentative de connexion
    if (client.connect(clientId.c_str(), MqttUser, MqttPass)) {
      char SubTopic[50];
      sprintf(SubTopic, "/%s/led/", ESPName);
      client.subscribe(SubTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      Serial.println(" try again in 5 seconds");
      // Attente avant nouvelle tentative
      delay(5000);
    }
  }
}

void setup() {
  // Initialisation de la communication série
  Serial.begin(115200);
  // Configuration du réseau WiFi
  setup_wifi();

  //Configuration du certificat
  espClient.setCACert(ca_cert);

  // Configuration du client MQTT
  client.setServer(mqtt_server, 8883);
  client.setCallback(onMqttMessage);

  // Configuration des topics MQTT
  sprintf(mqtt_topic_temp, "/%s/Temperature/", ESPName); // Topic pour la température
  sprintf(mqtt_topic_hum, "/%s/Humidity/", ESPName); // Topic pour l'humidité
  sprintf(mqtt_topic_gas, "/%s/Gas/", ESPName); // Topic pour le CO2
  sprintf(mqtt_topic_motion, "/%s/Motion/", ESPName); // Topic pour le Mouvement

  // Configuration de la led interne
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Boucle principale du programme
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sensorGTH();
  // Pause avant la prochaine lecture
  // 1 minute
  for(int i =0; i < 300; i++){
    client.loop();

  // Lecture de la valeur du capteur de mouvement
  int motionDetected = (analogRead(MOTION_PIN) > 900) ? HIGH : LOW;
  unsigned long currentTime = millis();

  if (motionDetected != lastMotionState) {
    lastMotionState = motionDetected;
    lastMotionTime = currentTime;
    if (motionDetected == HIGH) {
      // Si un mouvement est détecté
      Serial.println("Motion detected");
      if (!motionDetectedPreviously) {
        // Si c'est le premier mouvement détecté depuis la dernière détection
        client.publish(mqtt_topic_motion, "1"); // Envoyer un message MQTT
        motionDetectedPreviously = true;
      }
    }
  } else if (motionDetected == LOW && currentTime - lastMotionTime >= MOTION_TIMEOUT && !motionMessageSent) {
    // Si aucun mouvement n'est détecté depuis un certain temps et qu'aucun message "0" n'a été envoyé
    Serial.println("No motion detected for 5 minutes.");
    client.publish(mqtt_topic_motion, "0"); // Envoyer un message MQTT
    motionDetectedPreviously = false;
    motionMessageSent = true;
  } else if (motionDetected == HIGH && currentTime - lastMotionTime >= MOTION_TIMEOUT) {
    // Si un mouvement est détecté depuis un certain temps
    Serial.println("Du mouvement depuis 5 minutes");
    client.publish(mqtt_topic_motion, "1"); // Envoyer un message MQTT
    lastMotionTime = currentTime;
  } else if (motionDetected == HIGH) {
    // Si un mouvement est détecté
    motionMessageSent = false;
  }

  delay(100);
  }
}

void sensorGTH(){
  delay(100);
  client.loop();
  // Lecture de la température et de l'humidité
  int temperature = 0;
  int humidity = 0;
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  // Vérification de la lecture du capteur DHT11
  if (result == 0) {
    // Affichage des valeurs sur le moniteur série
    Serial.print("Température: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumidité: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Publication des valeurs de température et d'humidité sur MQTT
    client.loop();
    client.publish(mqtt_topic_temp, String(temperature).c_str());
    client.loop();
    client.publish(mqtt_topic_hum, String(humidity).c_str());
  } else {
    // Affichage du message d'erreur
    Serial.println(DHT11::getErrorString(result));
  }

  // Lecture de la valeur corrigée de CO2
  float ppm = gasSensor.getCorrectedPPM(temperature, humidity);
  ppm = ppm/100;
  // Affichage de la valeur sur le moniteur série
  Serial.print("Valeur CO2 corrigée: ");
  Serial.println(ppm);

  // Publication de la valeur de CO2 sur MQTT
  client.loop();
  client.publish(mqtt_topic_gas, String(ppm).c_str());
}
