# PlatformIO — Setup & exemples pour ESP32 (firmware)

## Environnements à créer
- `esp32-reader` : lecteur RFID + communication MQTT/Zigbee
- `esp32-teacher` : boutons / indicateurs locaux

## platformio.ini (exemple minimal)

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
  miguelbalboa/MFRC522
  knolleary/PubSubClient

# Build & upload
# pio run -e esp32dev -t upload
# pio test -e esp32dev

## Exemple minimal (pseudo code)

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>

void setup() {
  // init wifi, mqtt, rfid
}

void loop() {
  if (rfidDetected()) {
    String uid = readRfid();
    publishScan(uid);
  }
}
```

## Tests
- Unit tests with `platformio test` (mocks des périphériques)
- Scripts pour flash + test: `scripts/flash_and_test.sh`

---
*Adapter les libs selon choix PN532 vs RC522 et Zigbee vs MQTT.*
