#include <Arduino.h>
#include <WiFi.h>
#include "logger.h"

// =====================
// CONFIG WIFI
const char* ssid     = "iPhone de Matéo";
const char* password = "StrongP@ssword";

// =====================
// PINS
const int pinJaune = 27;
const int pinNoir  = 13;
const int pinRouge = 12;
const int pinBleu  = 25;
const int pinVert  = 26;

// =====================
// COULEURS & FORMATAGE
#ifndef USE_ANSI
  #if defined(ARDUINO_USE_ANSI)
    #define USE_ANSI ARDUINO_USE_ANSI
  #elif defined(ENV_USE_ANSI)
    #define USE_ANSI ENV_USE_ANSI
  #else
    #define USE_ANSI 1
  #endif
#endif
#include "../include/logger_ansi.h"

// =====================
const char* getWiFiStatusText(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD:       return "PAS DE MODULE";
    case WL_IDLE_STATUS:     return "EN ATTENTE";
    case WL_NO_SSID_AVAIL:   return "SSID INTROUVABLE";
    case WL_SCAN_COMPLETED:  return "SCAN TERMINÉ";
    case WL_CONNECTED:       return "CONNECTÉ";
    case WL_CONNECT_FAILED:  return "ECHEC CONNEXION";
    case WL_CONNECTION_LOST: return "CONNEXION PERDUE";
    case WL_DISCONNECTED:    return "DÉCONNECTÉ";
    default:                 return "INCONNU";
  }
}


// =====================
// SETUP
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();

  pinMode(pinJaune, INPUT_PULLDOWN);
  pinMode(pinNoir,  INPUT_PULLDOWN);
  pinMode(pinRouge, INPUT_PULLDOWN);
  pinMode(pinBleu,  INPUT_PULLDOWN);
  pinMode(pinVert,  INPUT_PULLDOWN);

  logger.separator();
  logger.info("Démarrage du système...");
  logger.info("Tentative de connexion au Wi-Fi '%s'...", ssid);

  WiFi.begin(ssid, password);

  int tentative = 0;
  while (WiFiClass::status() != WL_CONNECTED && tentative < 20) {
    delay(500);
    Serial.print(".");
    tentative++;
  }
  Serial.println();

  if (WiFiClass::status() == WL_CONNECTED) {
    logger.success("Connecté ! IP: %s", WiFi.localIP().toString().c_str());
  } else {
    logger.error("Échec Wi-Fi. Status: %s", getWiFiStatusText(WiFiClass::status()));
  }
  logger.separator();
}

// =====================
// LOOP
void loop() {
  if (digitalRead(pinJaune)) {
    logger.logColor(ANSI_YELLOW, "Bouton détecté : JAUNE");
    delay(250);
  }

  if (digitalRead(pinNoir)) {
    logger.logColor(ANSI_WHITE, "Bouton détecté : NOIR");
    delay(250);
  }

  if (digitalRead(pinRouge)) {
    logger.logColor(ANSI_RED, "Bouton détecté : ROUGE");
    delay(250);
  }

  if (digitalRead(pinBleu)) {
    logger.logColor(ANSI_BLUE, "Bouton détecté : BLEU");
    delay(250);
  }

  if (digitalRead(pinVert)) {
    logger.logColor(ANSI_GREEN, "Bouton détecté : VERT");
    delay(250);
  }

  static unsigned long lastTime = 0;
  if (millis() - lastTime > 10000) {
    lastTime = millis();
    logger.debug("Heartbeat - WiFi Status: %s", getWiFiStatusText(WiFiClass::status()));
  }
}