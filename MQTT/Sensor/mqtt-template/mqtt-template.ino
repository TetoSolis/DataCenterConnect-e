// Certificat pour la connexion sécurisée MQTT
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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Paramètres du réseau WiFi
const char* ssid = "GRP-1-IOM";
const char* password = "Porygon-Z#1";

// Paramètres MQTT
const char* MqttUser = "GRP-1-IOM";
const char* MqttPass = "Porygon-Z#1";
const char* mqtt_server = "192.168.1.50";
const char* mqtt_topic = "/Sensor/Template/";
const char* ESPName = "ESP-Template";

// Client MQTT et connexion sécurisée
BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Configuration de la connexion WiFi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
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

void reconnect() {
  // Reconnexion au broker MQTT
  char err_buf[256];
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MqttUser, MqttPass)) {
      char SubTopic[50];
      sprintf(SubTopic, "/%s/led/", ESPName);
      client.subscribe(SubTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      espClient.getLastSSLError(err_buf, sizeof(err_buf));
      Serial.print("SSL error: ");
      Serial.println(err_buf);
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // Initialisation du programme
  Serial.begin(115200);

  //Configuration du certificat
  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  espClient.setTrustAnchors(serverTrustedCA);
  espClient.setInsecure();
  
  // Configuration de la connexion WiFi
  setup_wifi();
  
  // Configuration de l'heure à partir de serveurs NTP
  setClock();

  // Configuration du client MQTT
  client.setServer(mqtt_server, 8883);
  client.setCallback(onMqttMessage);

  // Configuration de la led interne
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Boucle principale du programme
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  client.publish(mqtt_topic, "test"); // Envoyer un message MQTT
  delay(1000);
}
