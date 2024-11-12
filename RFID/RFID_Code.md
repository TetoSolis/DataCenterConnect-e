***
# Codes

## Codes de base
Code de base permettant la lecture de la carte.

```cpp
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22 // Configurable, voir la disposition typique des pins ci-dessus
#define SS_PIN  5  // Configurable, voir la disposition typique des pins ci-dessus

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crée une instance MFRC522

void setup() {
    Serial.begin(115200); // Initialise la communication série avec le PC

    while (!Serial); // Ne rien faire si aucun port série n'est ouvert (ajouté pour les Arduinos basés sur ATMEGA32U4)

    SPI.begin(); // Initialise le bus SPI
    mfrc522.PCD_Init(); // Initialise MFRC522

    delay(4); // Délai optionnel. Certaines cartes nécessitent plus de temps après l'initialisation pour être prêtes, voir le fichier Readme
    mfrc522.PCD_DumpVersionToSerial(); // Affiche les détails du PCD - Détails du lecteur de cartes MFRC522
    Serial.println(F("Scannez un PICC pour voir l'UID, le SAK, le type et les blocs de données..."));
}

void loop() {
    // Vérifie si une nouvelle carte RFID est présente près du lecteur
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return; // Si aucune nouvelle carte n'est détectée, sort de la fonction loop()
    }

    // Tente de lire l'UID de la carte détectée
    if (!mfrc522.PICC_ReadCardSerial()) {
        return; // Si la lecture de l'UID échoue, sort de la fonction loop()
    }

    // Vérifie si l'UID de la carte lue correspond à l'UID autorisé
    if (checkUID(mfrc522.uid.uidByte, authorizedUID, 4)) {
        Serial.println(F("Carte autorisée détectée !")); // Affiche un message si les UIDs correspondent
        // Ici, vous pouvez ajouter du code pour effectuer des actions spécifiques lorsque la carte autorisée est détectée
    } else {
        Serial.println(F("Carte non autorisée.")); // Affiche un message si l'UID de la carte ne correspond pas à l'UID autorisé
    }

    // Met la carte RFID actuellement lue en état d'arrêt pour éviter de la relire immédiatement
    mfrc522.PICC_HaltA();
}
```

## Code avec conditions
Code avec condition pour ne laisser passer qu'une carte.

```cpp
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22 // Configurable, voir la disposition typique des pins ci-dessus
#define SS_PIN  5  // Configurable, voir la disposition typique des pins ci-dessus

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crée une instance MFRC522

const byte authorizedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF}; // Exemple d'UID

// Déclaration de la fonction checkUID
bool checkUID(byte *cardUID, const byte *authorizedUID, byte uidLength);

void setup() {
    Serial.begin(115200); // Initialise la communication série avec le PC

    while (!Serial); // Ne rien faire si aucun port série n'est ouvert

    SPI.begin(); // Initialise le bus SPI
    mfrc522.PCD_Init(); // Initialise MFRC522

    delay(4); // Délai optionnel. Certaines cartes nécessitent plus de temps après l'initialisation pour être prêtes
    mfrc522.PCD_DumpVersionToSerial(); // Affiche les détails du PCD - Détails du lecteur de cartes MFRC522
    Serial.println(F("Scannez un PICC pour voir l'UID, le SAK, le type et les blocs de données..."));
}

void loop() {
    // Réinitialise la boucle si aucune nouvelle carte n'est présente sur le capteur/lecteur
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Sélectionne l'une des cartes
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // Vérifie si l'UID de la carte correspond à l'UID autorisé
    if (checkUID(mfrc522.uid.uidByte, authorizedUID, 4)) {
        Serial.println(F("Carte autorisée détectée !"));
    } else {
        Serial.println(F("Carte non autorisée."));
    }

    // Arrête la lecture de la carte
    mfrc522.PICC_HaltA();
}

bool checkUID(byte *cardUID, const byte *authorizedUID, byte uidLength) {
    for (byte i = 0; i < uidLength; i++) {
        if (cardUID[i] != authorizedUID[i]) {
            return false;
        }
    }
    return true;
}
```

# Pins

| ESP32          | RFID Reader |
|----------------|-------------|
| 3.3V           | 3.3V        |
| 22             | RST         |
| GND            | GND         |
| Not required   | IRQ         |
| 19             | MISO        |
| 23             | MOSI        |
| 18             | SCK         |
| 5              | SDA         |
