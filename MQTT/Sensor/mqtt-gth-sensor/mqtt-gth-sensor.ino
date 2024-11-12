static const char ca_cert[] PROGMEM = R"EOF(
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

// Inclusion des bibliothèques nécessaires
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MQ135.h>
#include <DHT11.h>

// Déclaration des constantes pour les broches et les paramètres du capteur DHT11
#define DHTPIN 4 // Broche du capteur DHT11 (GPIO2 ou D4 pour ESP8266)
const int ANALOG_PIN = A0; // Broche analogique à laquelle le capteur MQ135 est connecté

// Création d'instances des capteurs
MQ135 gasSensor = MQ135(ANALOG_PIN);
DHT11 dht11(DHTPIN);

// Paramètres de connexion WiFi
const char* ssid = "GRP-1-IOM"; // SSID du réseau WiFi
const char* password = "Porygon-Z#1"; // Mot de passe du réseau WiFi
const char* mqtt_server = "192.168.1.50"; // Adresse IP du serveur MQTT
const char* MqttUser = "GRP-1-IOM"; // Nom d'utilisateur MQTT
const char* MqttPass = "Porygon-Z#1"; // Mot de passe MQTT

// Définition des topics MQTT pour chaque type de données
const char* mqtt_topic_temp = "/Sensor/Temperature/"; // Topic pour la température
const char* mqtt_topic_humid = "/Sensor/Humidity/"; // Topic pour l'humidité
const char* mqtt_topic_gas = "/Sensor/Gas/"; // Topic pour le CO2
const char* ESPName = "ESP-GTH";

// Déclaration des clients MQTT et WiFi
BearSSL::WiFiClientSecure espClient; // Client WiFi sécurisé
PubSubClient client(espClient); // Client MQTT

// Déclaration de variables pour la gestion des messages
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// Fonction de configuration de l'horloge pour la validation X.509
void setClock() {
  // Configuration de la synchronisation NTP
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  // Attente de la synchronisation NTP
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  // Affichage de l'heure actuelle
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

// Fonction de configuration du réseau WiFi
void setup_wifi() {
  // Attente pour la connexion au réseau WiFi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connexion au réseau WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Attente de la connexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialisation du générateur de nombres aléatoires
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

  // Comparer la chaîne de caractères du payload avec "true" ou "false"
  if(payloadStr == "true"){
    // Allumer la LED
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("L'");
    Serial.print(ESPName);
    Serial.println(" a allumé sa LED.");
  } else if(payloadStr == "false"){
    // Eteindre la LED
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("L'");
    Serial.print(ESPName);
    Serial.println(" a éteint sa LED.");
  }
}

// Fonction de reconnexion au broker MQTT
void reconnect() {
  char err_buf[256];

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
      // Affichage des erreurs SSL
      espClient.getLastSSLError(err_buf, sizeof(err_buf));
      Serial.print("SSL error: ");
      Serial.println(err_buf);
      Serial.println(" try again in 5 seconds");
      // Attente avant nouvelle tentative
      delay(5000);
    }
  }
}

// Fonction d'initialisation du système
void setup() {
  // Initialisation de la communication série
  Serial.begin(115200);
  // Création d'une liste de certificats pour le client SSL
  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  espClient.setTrustAnchors(serverTrustedCA);
  espClient.setInsecure();
  // Configuration du réseau WiFi
  setup_wifi();
  // Configuration de l'horloge pour la validation X.509
  setClock();
  // Configuration du client MQTT
  client.setServer(mqtt_server, 8883);
  client.setCallback(onMqttMessage);

  // Configuration de la led interne
  pinMode(LED_BUILTIN, OUTPUT);
}

// Fonction principale, exécutée en boucle
void loop() {
  // Vérification de la connexion MQTT
  if (!client.connected()) {
    reconnect();
  }
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
    client.publish(mqtt_topic_humid, String(humidity).c_str());
  } else {
    // Affichage du message d'erreur
    Serial.println(DHT11::getErrorString(result));
  }

  // Lecture de la valeur corrigée de CO2
  float ppm = gasSensor.getCorrectedPPM(temperature, humidity);

  // Affichage de la valeur sur le moniteur série
  Serial.print("Valeur CO2 corrigée: ");
  Serial.println(ppm);

  // Publication de la valeur de CO2 sur MQTT
  client.loop();
  client.publish(mqtt_topic_gas, String(ppm).c_str());

  // Pause avant la prochaine lecture
  // 1 minute
  for(int i =0; i < 300; i++){
    delay(200);
    client.loop();
  }
}
