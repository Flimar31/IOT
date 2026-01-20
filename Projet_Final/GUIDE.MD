# Projet : Système de sécurité pour bâtiment scolaire — pointage / appel / gestion des retards

## Résumé

Ce document décrit un projet complet pour un système de pointage/appel et gestion des retards dans un bâtiment scolaire. Il repose sur au moins **2 ESP32**, une **passerelle Zigbee** sur **Raspberry Pi**, et un **serveur web** pour la gestion et la consultation des données. Le flux principal permet à un professeur de démarrer/terminer un appel via badge RFID, aux élèves de pointer leur présence, puis de signaler des retards.

---

## Objectifs

* Automatiser l'appel (présence) en classe.
* Enregistrer les présences et retards dans une base de données centralisée.
* Fournir une interface web pour voir les listes, retards et rapports.
* Assurer sécurité, permissions (professeurs / élèves) et respect de la vie privée (RGPD).

---

## Matériel proposé (détaillé)

* **2× ESP32** (minimum) — nœuds enseignants / nœuds élèves. Recommandé : Devkit ESP32.
* **Modules radio Zigbee** (pour chaque ESP32) *ou* modules XBee (Series 2) connectés en UART aux ESP32 — **raison** : l'ESP32 ne fournit pas nativement Zigbee grand public ; il faut un module radio compatible Zigbee ou une alternative (voir section architecture).
* **Dongle Zigbee USB (coordinator)** pour Raspberry Pi (ex : CC2531/ConBee/CC2652) connecté au Pi pour faire la passerelle.
* **Raspberry Pi** (3/4/Zero 2 W) — héberge le broker MQTT, zigbee2mqtt/deCONZ, et le serveur web / API.
* **Module RFID** (ex : RC522 ou PN532) + *cartes/badges RFID* pour profs et élèves.
* **Afficheur LCD 16×2** (I2C recommandé) pour retour local.
* **Buzzer** pour feedback sonore.
* **Module 'traffic light' (feux)** — 3 LEDs (vert/orange/rouge) ou module 3 LEDs avec driver.
* **Boutons poussoirs** (sauvegarde manuelle des états).
* **Breadboard, câbles Dupont, résistances, transistors MOSFET/driver** (si besoin pour feux)
* **Alimentation 5V/3A** pour le système central et alimentations séparées pour modules exigeant de l’intensité.

---

## Architecture fonctionnelle

### Rôles des composants

* **ESP32 (Node)** : lecture RFID, interface locale (LCD, buzzer, feux), envoi d'événements vers le réseau Zigbee (via module radio) ou publication MQTT si il est connecté en Wi‑Fi.
* **Modules Zigbee / XBee** : assurent la communication réseau maillée / point-à-point entre ESP32 et le coordonnateur.
* **Raspberry Pi (passerelle)** : coordonne Zigbee, fait la translation vers MQTT, héberge un broker MQTT (Mosquitto), zigbee2mqtt (ou deCONZ) pour traduire frames Zigbee → MQTT, et un backend (API + base de données).
* **Serveur web / API** : stockage des sessions d'appel, gestion des utilisateurs, authentification, affichage des rapports.

### Communication entre nœuds

Option A — **Zigbee (recommandé si vous voulez maillage et fiabilité)**

* Chaque ESP32 est connecté à un module radio Zigbee/XBee en UART.
* Les modules communiquent entre eux et envoient des événements au coordinator (Raspberry Pi + dongle Zigbee).
* Le Pi avec zigbee2mqtt publie les événements sur MQTT (`school/room1/attendance` etc.).

Option B — **MQTT direct via Wi‑Fi (alternative simple)**

* Les ESP32 se connectent au Wi‑Fi et publient directement sur le broker MQTT du Raspberry Pi.
* Plus simple à implémenter — mais pas Zigbee.

> Choix : gardez Zigbee si vous voulez robustesse sans dépendre du Wi‑Fi (maillage, longue portée). Sinon MQTT direct est plus simple pour prototypage.

---

## Flux principal (séquence d'utilisation)

1. Le professeur scanne son **badge prof** au lecteur RFID de son ESP32 local.
2. ESP32 vérifie la permission et envoie `START_SESSION` (topic ou frame Zigbee) au coordinator.
3. Le **feu** passe au **vert** et l'écran affiche "Appel démarré".
4. Les élèves scannent leur **carte élève** sur un lecteur (un lecteur central ou plusieurs lecteurs en salle). Chaque scan envoie un événement `PRESENT` avec l'ID du badge.
5. Les présences sont affichées en temps réel sur l'écran et remontées au serveur via MQTT/API.
6. Quand l'appel est terminé, le prof re-scanne son badge → `END_SESSION` : l'ESP32 déclenche la phase retards (feu orange), envoie le résumé au serveur. Les élèves retardataires peuvent encore scanner pour marquer un `LATE` pendant la phase définie.
7. À la fin du cours, le prof scanne pour **clôturer** le cours → feu rouge pendant 5 minutes (période bloquante configurable) puis extinction. Si un autre cours démarre, le feu orange peut clignoter pour signaler au professeur qu'il peut démarrer l'appel.

---

## Cas d'usage et permissions

* **Professeurs** : peuvent démarrer/terminer sessions, voir la liste complète, modifier statuts.
* **Élèves** : ne peuvent que pointer leur présence / retards, consulter éventuellement leur historique via l'interface personnelle.
* **Administrateurs** : gestion des comptes, classes, emplois du temps, rapports et export CSV.

---

## Base de données — Schéma proposé (SQL)

Tables principales :

* `users` (id, rfid_uid, nom, prenom, role {student, teacher, admin}, classe_id, email)
* `classes` (id, nom)
* `schedules` (id, classe_id, jour, heure_debut, heure_fin, matiere)
* `sessions` (id, classe_id, teacher_id, start_time, end_time, status)
* `attendances` (id, session_id, user_id, present BOOLEAN, time_stamp, status {present, late, absent})
* `devices` (id, name, esp_id, location, last_seen)

Exemples d'index : index sur `rfid_uid`, `session_id`, `classe_id`, `time_stamp`.

---

## API REST — Endpoints essentiels (exemples)

* `POST /api/session/start` {device_id, teacher_rfid} → crée session et retourne session_id
* `POST /api/session/:id/scan` {rfid, timestamp, device_id} → enregistre présence (present/late selon état)
* `POST /api/session/:id/end` → clôture session
* `GET /api/session/:id/report` → retourne le rapport (liste, retards)
* `GET /api/classes/:id/schedule`
* `POST /api/users` (création comptes, import CSV)

Auth : JWT + TLS (HTTPS). Les devices peuvent utiliser un token device pour s'authentifier au broker.

---

## Messages & Topics MQTT (exemples)

* `school/<site>/<room>/control` — messages de contrôle (START_SESSION, END_SESSION)
* `school/<site>/<room>/scan` — payload: `{ "rfid":"04AABBCC", "device":"esp32-xx", "time":"2026-01-20T09:00:00" }`
* `school/<site>/<room>/state` — pour retourner l'état local (feu, session en cours)

Format JSON court et structuré. Le backend souscrit à ces topics et stocke les événements.

---

## Firmware ESP32 — architecture logicielle

Composants logiciels :

* **RFID driver** (RC522 ou PN532) : lecture UID.
* **Comm module** : XBee API via Serial (si Zigbee) ou client MQTT (si Wi‑Fi/MQTT direct).
* **UI local** : gestion écran LCD, LED/feu, buzzer, boutons.
* **State machine** : états `IDLE` → `SESSION_OPEN` → `LATE_PHASE` → `SESSION_CLOSED`.

### Exigences du code

* Reconnect automatique du réseau (Zigbee / Wi‑Fi).
* Gestion des files d'événements si le coordo est indisponible (queue en mémoire, retry).
* Timeouts configurables (durée appel, durée période retard).

---

## Sécurité & confidentialité (RGPD)

* Stocker seulement les données nécessaires (ID, nom, classe, date de présence).
* Chiffrement en transit : **TLS** pour l'API web, et usage d'un broker MQTT configuré en TLS si possible.
* Authentification forte pour l'interface d'administration (2FA recommandé).
* Durée de conservation configurable (ex : 1 an) et procédures d'export/suppression.
* Journalisation des accès pour audits.

---

## Déploiement & tests

### Environnement de dev

* Raspberry Pi avec Debian/Raspbian
* Mosquitto (MQTT), zigbee2mqtt ou deCONZ selon le dongle
* Backend local (Node.js/Express + PostgreSQL) ou Flask + PostgreSQL
* Frontend : React ou Vue (tableau de bord, liste, recherche)

### Tests à effectuer

* Tests unitaires pour l'API et la DB.
* Tests d'intégration : lecture RFID → publication MQTT → enregistrement DB.
* Tests de charge : montée en charge si plusieurs salles simultanées.
* Scénarios de pannes : perte réseau, perte du coordo, lecteurs déconnectés.

---

## Plan d'implémentation (phases)

1. **Prototype (2–3 semaines)** : 2 ESP32, 1 lecteur RFID, Pi + MQTT, backend minimal (enregistrement scans). UI minimal.
2. **Fonctionnel (3–4 semaines)** : Ajout gestion sessions, permissions, feux, buzzer, gestion retards.
3. **Sécurisation & production (2–3 semaines)** : TLS, authentification, tests, containerisation (Docker), backup DB.
4. **Améliorations** : Dashboard avancé, export CSV, notifications (email / SMS) pour absences répétées.

---

## Estimation budgétaire (approximative)

* ESP32 : 2 × 8–15 €
* Module RFID RC522 : ~3–8 €
* Badges RFID (lot) : 0,5–2 € / unité
* Dongle Zigbee USB : 20–40 €
* Modules XBee (si choisis) : 15–35 € / module
* Raspberry Pi : 35–60 €
* LCD 16×2 I2C : 3–8 €
* Feu / module traffic : 5–20 €
* Câblage, breadboard, boîtier : 10–40 €

---

## Annexes : Exemples de snippets

### Extrait pseudo-code ESP32 (Arduino style – MQTT direct) :

```cpp
// Pseudo: lecteur RFID -> publish MQTT
#include <WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>

void loop() {
  if (rfidDetected()) {
    String uid = readRfid();
    // selon état local
    pub.publish("school/room1/scan", payload(uid));
  }
}
```

### Exemple JSON payload d'un scan

```json
{
  "rfid": "04AABBCC",
  "device": "esp32-room1-reader1",
  "time": "2026-01-20T09:00:00Z",
  "session_id": "sess-20260120-09-01"
}
```

---

## Améliorations et extensions possibles

* Lecture NFC via smartphone (option pour élèves sans badge).
* Intégration SSO (LDAP / Azure AD) pour gestion des professeurs.
* Notifications automatiques (email / SMS) pour absences répétées.
* Version mobile / PWA du dashboard.
* Analytics : taux de présence par classe, par enseignant, par période.

---

## Points d'attention / risques

* **Compatibilité Zigbee** : choisissez des modules et un dongle compatibles (zigbee2mqtt a une liste de matériel supporté).
* **Données personnelles** : clarifier conservation et accès (accord parentale si mineurs).
* **Interférences RF** : positionnement des lecteurs et des antennes.
* **Défauts de reconnaissance RFID** : prévoir options de saisie manuelle.

---

## Conclusion

Ce document fournit un cadre complet pour concevoir et implémenter votre système de pointage/appel. Il couvre du matériel au logiciel, schéma DB, endpoints, mesures de sécurité, et plan d'implémentation. Les décisions clés à trancher pour lancer le prototype sont : **Zigbee vs MQTT Wi‑Fi**, choix du module RFID, et le backend (stack préférée : Node.js + PostgreSQL + React).

---

Si vous souhaitez, je peux fournir dans la suite :

* Un diagramme d'architecture (SVG/PNG).
* Un schéma de câblage détaillé pour un ESP32 + RC522 + XBee.
* Un plan de tests avec scripts Jenkins / GitHub Actions.
* Le code complet (firmware ESP32) prêt à flasher et les endpoints serveur minimal (Node.js/Express).

Fin du document.
