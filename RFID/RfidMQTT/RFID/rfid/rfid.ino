// Bibliothéques :
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Certificat pour la connexion sécurisée MQTT.
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

// On donne un nom aux PINS.
#define RST_PIN 22
#define SS_PIN  5

// Attribution des PINS à MFRC522.
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Paramètres du réseau WiFi
const char* ssid = "GRP-1-IOM";
const char* password = "Porygon-Z#1";

// Paramètres MQTT
const char* MqttUser = "GRP-1-IOM";
const char* MqttPass = "Porygon-Z#1";
const char* mqtt_server = "192.168.1.50";
const char* mqtt_topic = "/Sensor/rfid/uid/";
const char* mqtt_topic_door = "/Sensor/rfid/doors/";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Initialisation du stockages des UIDS.
#define MAX_UIDS 10
byte authorizedUIDs[MAX_UIDS][4];
byte numStoredUIDs = 0;

byte adminUID[4] = {0xF0, 0x22, 0x80, 0x63};
byte otherUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

// De base, on initialise le mode admin à "false" donc éteint.
bool isAdminMode = false;

void setup_wifi() {
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
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), MqttUser, MqttPass)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    // On est sur SPI pas I2C
    SPI.begin();
    mfrc522.PCD_Init();

    setup_wifi();
    espClient.setCACert(ca_cert);
    client.setServer(mqtt_server, 8883);
    client.setCallback(callback);

    mfrc522.PCD_DumpVersionToSerial();
    Serial.println(F("Placez un PICC pour voir UID, SAK, type et blocs de données..."));

    addUID(adminUID); // Ajoute l'UID administrateur
    addUID(otherUID); // Ajoute un autre UID pré-autorisé (je le laisse pour l'exemple.)
}

void loop() {
    // Vérifie si le client MQTT est connecté, tente une reconnexion si nécessaire.
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Vérifie si un nouveau tag RFID est présent et lit son UID.
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // Lecture et traitement de l'UID du tag RFID.
    readUID(mfrc522.uid.uidByte, mfrc522.uid.size);

    // Vérification si l'UID lu est celui de l'administrateur et bascule du mode administrateur.
    if (checkUID(mfrc522.uid.uidByte, adminUID, 4)) {
        isAdminMode = !isAdminMode;
        Serial.println(isAdminMode ? F("Mode administrateur activé.") : F("Mode administrateur désactivé."));
        mfrc522.PICC_HaltA();
        delay(2000); // Délai pour éviter la lecture multiple de la même carte
        return;
    }

    // En mode administrateur, permet de supprimer ou d'ajouter des UIDs à la liste des autorisés.
    if (isAdminMode) {
        if (delUID(mfrc522.uid.uidByte)) {
            Serial.println(F("Carte supprimée avec succès."));
            delay(5000); // Délai pour permettre le retrait de la carte du lecteur
            isAdminMode = false; // Désactivation du mode administrateur après suppression
        } else {
            Serial.println(F("Enregistrement de la nouvelle carte..."));
            if (addUID(mfrc522.uid.uidByte)) {
                Serial.println(F("Nouvelle carte enregistrée avec succès."));
                isAdminMode = false; // Désactivation du mode administrateur après enregistrement
            } else {
                Serial.println(F("Échec de l'enregistrement de la carte."));
            }
        }
    } else {
        // Hors du mode administrateur, vérification si le tag est autorisé et action en conséquence.
        if (isUIDAuthorized(mfrc522.uid.uidByte)) {
            Serial.println(F("Carte autorisée détectée !"));
            client.publish(mqtt_topic_door, "1");
        } else {
            Serial.println(F("Carte non autorisée."));
            client.publish(mqtt_topic_door, "0");
        }
    }
  
    mfrc522.PICC_HaltA();
    delay(2000); // Délai pour éviter la lecture multiple de la même carte
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

bool delUID(byte *uid) {
    for (byte i = 0; i < numStoredUIDs; i++) {
        if (checkUID(uid, authorizedUIDs[i], 4)) {
            Serial.println(F("Carte trouvée, suppression en cours..."));
            for (byte j = i; j < numStoredUIDs - 1; j++) {
                memcpy(authorizedUIDs[j], authorizedUIDs[j + 1], 4);
            }
            numStoredUIDs--;
            return true;
        }
    }
    Serial.println(F("UID non trouvé."));
    return false;
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
    String message = uidString;
    client.publish(mqtt_topic, message.c_str());
}

bool isUIDAuthorized(byte *uid) {
    for (byte i = 0; i < numStoredUIDs; i++) {
        if (checkUID(uid, authorizedUIDs[i], 4)) {
            return true;
        }
    }
    return false;
}
