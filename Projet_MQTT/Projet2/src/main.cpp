#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "logger.h"

// =====================
// CONFIG WIFI
const char* ssid     = "iPhone de Matéo";
const char* password = "StrongP@ssword";

// =====================
// CONFIG MQTT
const char* mqtt_server = "172.20.10.3";  // Adresse IP du PC fournie automatiquement
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/button";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

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
// CALLBACK MQTT - Appelée quand un message est reçu
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  logger.separator();
  logger.success("Message MQTT reçu sur topic '%s'", topic);
  
  if (message == "JAUNE") {
    logger.logColor(ANSI_YELLOW, "Bouton reçu : JAUNE");
  } else if (message == "NOIR") {
    logger.logColor(ANSI_WHITE, "Bouton reçu : NOIR");
  } else if (message == "ROUGE") {
    logger.logColor(ANSI_RED, "Bouton reçu : ROUGE");
  } else if (message == "BLEU") {
    logger.logColor(ANSI_BLUE, "Bouton reçu : BLEU");
  } else if (message == "VERT") {
    logger.logColor(ANSI_GREEN, "Bouton reçu : VERT");
  } else {
    logger.info("Message : %s", message.c_str());
  }
  logger.separator();
}

// =====================
// FONCTION DE RECONNEXION MQTT
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    logger.info("Tentative de connexion au broker MQTT...");
    String clientId = "ESP32_Projet2_";
    clientId += String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
      logger.success("Connecté au broker MQTT !");
      mqttClient.subscribe(mqtt_topic);
      logger.info("Abonné au topic : %s", mqtt_topic);
    } else {
      logger.error("Échec MQTT, rc=%d. Nouvelle tentative dans 5s...", mqttClient.state());
      delay(5000);
    }
  }
}

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
    
    // Configuration et connexion MQTT
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqttCallback);
    reconnectMQTT();
  } else {
    logger.error("Échec Wi-Fi. Status: %s", getWiFiStatusText(WiFiClass::status()));
  }
  logger.separator();
}

// =====================
// LOOP
void loop() {
  // Maintenir la connexion MQTT et traiter les messages
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Les boutons locaux peuvent toujours être utilisés
  if (digitalRead(pinJaune)) {
    logger.logColor(ANSI_YELLOW, "Bouton LOCAL détecté : JAUNE");
    delay(250);
  }

  if (digitalRead(pinNoir)) {
    logger.logColor(ANSI_WHITE, "Bouton LOCAL détecté : NOIR");
    delay(250);
  }

  if (digitalRead(pinRouge)) {
    logger.logColor(ANSI_RED, "Bouton LOCAL détecté : ROUGE");
    delay(250);
  }

  if (digitalRead(pinBleu)) {
    logger.logColor(ANSI_BLUE, "Bouton LOCAL détecté : BLEU");
    delay(250);
  }

  if (digitalRead(pinVert)) {
    logger.logColor(ANSI_GREEN, "Bouton LOCAL détecté : VERT");
    delay(250);
  }

  static unsigned long lastTime = 0;
  if (millis() - lastTime > 10000) {
    lastTime = millis();
    logger.debug("Heartbeat - WiFi: %s | MQTT: %s", 
                 getWiFiStatusText(WiFiClass::status()),
                 mqttClient.connected() ? "Connecté" : "Déconnecté");
  }
}