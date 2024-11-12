#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

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

#define RST_PIN 22  // Définissez le numéro de broche pour RST.
#define SS_PIN  5   // Définissez le numéro de broche pour SS.

MFRC522 mfrc522(SS_PIN, RST_PIN); // Créez une instance de MFRC522.

// Paramètres du réseau WiFi
const char* ssid = "GRP-1-IOM";
const char* password = "Porygon-Z#1";

// Paramètres MQTT
const char* MqttUser = "GRP-1-IOM";
const char* MqttPass = "Porygon-Z#1";
const char* mqtt_server = "192.168.1.50";
const char* mqtt_topic = "/Sensor/rfid/";

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define MAX_UIDS 10  // Capacité maximale de UIDs autorisés.
byte authorizedUIDs[MAX_UIDS][4]; // Tableau pour stocker les UIDs autorisés.
byte numStoredUIDs = 0;  // Nombre actuel de UIDs stockés.

byte adminUID[4] = {0x30, 0x84, 0x67, 0x7A}; // UID de la carte administrateur
byte otherUID[4] = {0xDE, 0xAD, 0xBE, 0xEF}; // Autre UID

bool isAdminMode = false;  // Variable pour suivre l'état du mode administrateur

// Déclaration des fonctions
bool checkUID(byte *cardUID, byte *authorizedUID, byte uidLength);
void readUID(byte *uid, byte uidLength);
bool addUID(const byte *newUID);

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

void callback(char* topic, byte* payload, unsigned int length) {
  // Callback pour traiter les messages MQTT reçus
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Reconnexion au broker MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MqttUser, MqttPass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();

    setup_wifi();
    espClient.setCACert(ca_cert);
    client.setServer(mqtt_server, 8883);
    client.setCallback(callback);

    delay(4);
    mfrc522.PCD_DumpVersionToSerial();
    Serial.println(F("Placez un PICC pour voir UID, SAK, type et blocs de données..."));

    addUID(adminUID); // Ajoutez l'UID administrateur
    addUID(otherUID); // Ajoutez un autre UID pré-autorisé
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    readUID(mfrc522.uid.uidByte, mfrc522.uid.size);

    if (checkUID(mfrc522.uid.uidByte, adminUID, 4)) {
        if (isAdminMode) {
            Serial.println(F("Mode administrateur désactivé."));
            isAdminMode = false;
        } else {
            Serial.println(F("Mode administrateur activé. Passez une carte pour l'enregistrer."));
            isAdminMode = true;
            mfrc522.PICC_HaltA();
            delay(2000);
            return;
        }
    }

    if (isAdminMode) {
        // Vérifier si la carte est déjà enregistrée
        for (byte i = 0; i < numStoredUIDs; i++) {
            if (checkUID(mfrc522.uid.uidByte, authorizedUIDs[i], 4)) {
                // Supprimer la carte si elle est déjà enregistrée
                Serial.println(F("Carte déjà enregistrée. Suppression..."));
                for (byte j = i; j < numStoredUIDs - 1; j++) {
                    for (byte k = 0; k < 4; k++) {
                        authorizedUIDs[j][k] = authorizedUIDs[j + 1][k];
                    }
                }
                numStoredUIDs--;
                return;
            }
        }

        Serial.println(F("Enregistrement de la nouvelle carte..."));
        if (addUID(mfrc522.uid.uidByte)) {
            Serial.println(F("Nouvelle carte enregistrée avec succès."));
        } else {
            Serial.println(F("Échec de l'enregistrement de la carte."));
        }
        isAdminMode = false;
    } else {
        if (checkUID(mfrc522.uid.uidByte, authorizedUIDs[numStoredUIDs - 1], 4)) {
            Serial.println(F("Carte autorisée détectée !"));
            client.publish(mqtt_topic, "Carte autorisée détectée !");
        } else {
            Serial.println(F("Carte non autorisée."));
        }
    }

    mfrc522.PICC_HaltA();
    delay(2000);
}

bool addUID(const byte *newUID) {
    if (numStoredUIDs >= MAX_UIDS) {
        Serial.println(F("Espace de stockage insuffisant pour un nouveau UID."));
        return false;
    }

    for (byte i = 0; i < 4; i++) {
        authorizedUIDs[numStoredUIDs][i] = newUID[i];
    }

    numStoredUIDs++;
    return true;
}

bool checkUID(byte *cardUID, byte *authorizedUID, byte uidLength) {
    for (byte i = 0; i < uidLength; i++) {
        if (cardUID[i] != authorizedUID[i]) {
            return false;
        }
    }
    return true;
}

void readUID(byte *uid, byte uidLength) {
    Serial.print(F("UID de la carte :"));
    String uidString = "";
    for (byte i = 0; i < uidLength; i++) {
        Serial.print(uid[i] < 0x10 ? " 0" : " ");
        Serial.print(uid[i], HEX);
        uidString += String(uid[i], HEX);
    }
    Serial.println();
    // Message MQTT lorsque le badge est scanné
    String message = "Badge scanné: " + uidString;
    client.publish(mqtt_topic, message.c_str());
}
