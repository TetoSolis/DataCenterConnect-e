# RFID Explication
***
## RFID Tag Passif

- **RFID Tag Passif**: Fonctionne sans batterie, alimenté par un champ magnétique créé par le lecteur.
- **EEPROM**: Mémoire non volatile, conservant les données sans alimentation.
- **Fréquences et Évolution**:
    - Initialement utilisé à 125-134 kHz, robuste mais sans sécurité anti-clonage.
    - Évolution vers 13,56 MHz (HF) avec ajout de crypto-processeurs et mémoire anti-copie.
    - UHF (850-960 MHz) pour applications industrielles, avec étiquettes papier et portée jusqu'à 10 mètres.
- **Cryptographie**: Utilisation de RSA (asymétrique), SHA (hachage), DES/3DES/AES (symétriques).

## Protocoles et Sécurité

- **MIFARE Classic**: 1K ou 4K de données, cryptage CRYPTO1 (compromis en 2008).
- **MIFARE DESfire**: Meilleure sécurité avec DES, 2K3DES, 3K3DES, et AES.
- **Passeport Biométrique**: Utilise AES-128, clé dérivée de la clé optique.

## Technologies sans Fil pour IoT

- **NFC (Near Field Communication)**: Communication sécurisée à courte distance (jusqu'à 20 cm), principalement pour les paiements sans contact.
- **BAN (Body Area Network)**: Technologie dédiée à l'e-santé, nécessitant une radio ultra-faible puissance, un protocole MAC simple, une sécurité élevée et une interopérabilité.
