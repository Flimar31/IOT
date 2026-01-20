# SCHEMATICS — Matériel & Schémas (Espaces & Infra)

## Liste matériel recommandée 🧾
- 2+ ESP32 DevKit (esp32dev)
- Modules RFID : RC522 (SPI) ou PN532 (I2C/SPI)
- Module Zigbee USB dongle (coordinator) : CC2652R / ConBee II
- Modules XBee Series 2 (optionnel si Zigbee via XBee)
- LCD 16x2 I2C
- Buzzer (active)
- Traffic light module (3 LEDs) ou 3 LEDs individuelles + résistances + MOSFETs
- Boutons poussoirs
- Breadboard, câbles Dupont, alimentation 5V/3A
- Raspberry Pi 3/4/Zero 2 W (coordo & services)

---

## Schéma câblage : ESP32 + RC522 (SPI)
Pins ESP32 (exemples) -> RC522
- 3V3 -> VCC
- GND -> GND
- MOSI (GPIO23) -> MOSI
- MISO (GPIO19) -> MISO
- SCK  (GPIO18) -> SCK
- SDA/SS (GPIO5) -> NSS
- RST  (GPIO22) -> RST

Note : ajuster GPIO selon board. Utiliser niveaux 3.3V (RC522 ok).

## Schéma câblage : ESP32 + PN532 (I2C)
- 3V3 -> VCC
- GND -> GND
- SDA -> GPIO21 (SDA)
- SCL -> GPIO22 (SCL)
- RSTO -> GPIO15 (optionnel)

## Schéma : ESP32 + XBee (UART)
- ESP32 TX -> XBee DIN (via line-level if necessary)
- ESP32 RX -> XBee DOUT
- GND -> GND
- Ensure voltage compatibility (XBee usually 3.3V)

## Feux (Traffic light) & Buzzer
- Feux RGB: utiliser 3 GPIO via MOSFET/NPN to sink current; prévoir résistances et transistor drivers
- Buzzer: GPIO -> transistor -> buzzer (alim 5V possible)

## Raspberry Pi (passerelle)
- USB dongle Zigbee sur port USB (ConBee/CC2652)
- Installer Docker, exposer Mosquitto (1883/8883) et zigbee2mqtt
- Pi connecté au réseau local (Ethernet recommandé)

---

## Topologie réseau (ASCII)

Coordinator (Raspberry Pi)
  +-- Mosquitto (broker) <--> Backend API (subcribe) <--> DB (Postgres)
  +-- zigbee2mqtt (coordinator) <--> Zigbee nodes (ESP32+XBee)

ESP32 Nodes
  - RFID reader -> ESP32 -> XBee/Zigbee or MQTT -> Mosquitto -> Backend

Flow example : [RFID scan] -> [ESP32] -> [MQTT publish school/room1/scan] -> [Backend subscriber] -> [DB]

---

## Emplacements & boîtiers
- Installer lecteur à l'entrée des salles
- Raspberry Pi centralise par bâtiment (1 pi par bâtiment) ou par étage selon besoin

---

## Notes importantes
- Vérifier alimentation (LEDs + buzzer consommation)
- Tests d'interférence pour Zigbee; éviter colocations d'antennes proches
- Prévoir câble USB rallonge pour dongle Zigbee si boîtier Pi fermé

---
*Schémas pour prototype; pour production, produire des schémas PCB ou plans de boîtier.*
