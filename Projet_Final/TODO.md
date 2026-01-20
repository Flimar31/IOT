# TODO — Projet Système de pointage / appel (Complet)

> Checklist centralisée, découpage projet, tâches, dépendances, priorité et critères d'acceptation.

## Vue d'ensemble 🔭
- Objectif : livrer une solution complète (firmware ESP32, gateway Zigbee sur Raspberry Pi, broker MQTT, backend web, base de données, frontend dashboard, CI/CD, docs, tests). 
- Stack recommandée : PlatformIO (firmware), Docker (infra), Node.js/Express (backend) ou Flask, PostgreSQL, Mosquitto (MQTT), zigbee2mqtt, React (frontend).

---

## Priorités & jalons
1. Prototype minimal (MVP, 2–3 semaines)
   - 2 ESP32 fonctionnels (RFID → message MQTT)
   - Raspberry Pi avec Mosquitto + zigbee2mqtt
   - Backend minimal pour recevoir scans et les stocker
   - Frontend minimal listant les présences en temps réel
2. Fonctionnel (3–4 semaines)
   - Sessions cours (start/end), gestion retards, permissions prof/élève
   - UI/UX, exports CSV
   - Tests d'intégration HW ↔ MQTT ↔ API
3. Production & sécurité (2–3 semaines)
   - TLS, authentification, sauvegardes, monitoring, automatisation

---

## Projets & tâches (détaillé, 100%)

> Section refondue : tâches plus fines, critères de validation, tests, logs et opérations pour éviter les oublis par un agent/IA.

### A. Hardware & prototype (ESP32 nodes) ⚙️
- [ ] Choisir modules RFID (RC522 ou PN532) — valider par test de lecture UIDs et distance d'antenne
  - Critères : lecture fiable >95% à 3‑5 cm, documentation des pins
  - Tests : script de scan continu, CSV de 100 scans, taux d'erreur <5%
- [ ] Décider transport de données (Zigbee via XBee / module Zigbee vs MQTT Wi‑Fi)
  - Critères : proof-of-concept pour les deux options (latence, portée, fiabilité)
  - Tests : 50 scans/min pendant 10 min en Wi‑Fi et en Zigbee
- [ ] Achat matériel (voir `SCHEMATICS.md` liste exacte) — lister références, quantités et fournisseurs
- [ ] Câblage & harness : réaliser schéma filaire pour chaque board et version
  - Deliverable : images, diagrammes et checklist de câblage
- [ ] Tests de lecture RFID sur banc (test plan + scripts) — HIL test à automatiser
- [ ] Conception boîtiers (croquis + STL si impression 3D) — intégrer guides d'installation
- [ ] Tests de portée / interférences & power budget
  - Mesure : RSSI/erreurs Zigbee, test avec 5 nodes
- [ ] Feux (traffic light) hardware check
  - [ ] Choix composants (LEDs ou module), drivers (MOSFETs), résistances
  - [ ] Test de courant et de chauffe
  - [ ] Sécurité électrique : fusibles / limites de courant

### B. Firmware ESP32 (PlatformIO) 🔧
Projets PlatformIO : `esp32-reader`, `esp32-teacher`, `esp32-hub-sim` (simulateur pour CI)

Tâches détaillées et checklist :
- [ ] Implémenter drivers RFID (RC522/PN532) avec interface mockable
  - Tests unitaires : mock de bus SPI/I2C
- [ ] Implémenter state machine complète
  - États : IDLE, SESSION_OPEN, LATE_PHASE, SESSION_CLOSED, ERROR
  - Transitions : check conditions, timeouts et commandes prof (badge)
  - Tests unitaires de transitions, coverage >=90%
- [ ] Implémenter gestion feux (LEDs)
  - États mapping : SESSION_OPEN -> vert constant; LATE_PHASE -> orange; SESSION_CLOSED -> rouge 5min -> off
  - Patterns : orange clignote pour signaler cours suivant
  - Tests firmware : logique de timing (simulateur/time mocking)
- [ ] Communication : ajouter 2 modes (Zigbee via Serial API / MQTT via Wi‑Fi)
  - MQTT : TLS, reconnection automatique, exponential backoff
  - Zigbee/XBee : framing, ACK, retries
  - Tests : simulateur `esp32-hub-sim` publie/consume messages en CI
- [ ] Queue d'événements persistante (NVS/flash) et retry policy
  - Durabilité : buffer min 500 événements ou config
  - Tests HIL : couper réseau et vérifier replay
- [ ] Gestion des erreurs : watchdog, logs structurés (JSON), codes d'erreur définis
- [ ] OTA (optionnel pour production) : signer images, vérif signature avant apply
  - Tests : déployer image de test et rollback
- [ ] Logging local (stdout + fichier) et niveaux (DEBUG/INFO/WARN/ERROR)
  - Inclure correlation_id : session_id pour chaque event
- [ ] Tests hardware-in-loop (HIL) automatisés
  - Scénarios : démarrage session, 20 élèves présents, fin session, retards
  - Artéfacts : logs attachés, captures série

### C. Raspberry Pi (passerelle Zigbee + infra) 🛠️
- [ ] Choisir dongle Zigbee (CC2531 / CC2652 / ConBee II) et tester compatibilité zigbee2mqtt
- [ ] Installer Docker + docker-compose (documenter setup ARM/ARM64)
- [ ] Déployer et configurer : Mosquitto (TLS), zigbee2mqtt, backend, DB, nginx, grafana(optional)
  - Config : TLS certs, ACL Mosquitto (device tokens), stockage persistant
- [ ] Provisioning script (ansible / shell) : setup initial Pi + users + docker
- [ ] Healthchecks : services exposent /health endpoints (ok json)
- [ ] Logs & centralisation : promtail -> Loki ou Filebeat -> ELK
- [ ] Monitoring : Prometheus exporters + Grafana dashboards (MQTT messages, DB connections, CPU, memory)
- [ ] Backup & restore DB script (automated nightly dumps + retention policy)
- [ ] Mise à jour sécurisée du Pi (package upgrades + reboot policy)

### D. Backend API & DB (server) 🐘
- [ ] Choix stack (Node.js/Express + TypeORM/Postgres) ou Flask/SQLAlchemy — documenter décision
- [ ] Implémenter DB schema détaillé (migrations obligatoires)
  - Tables : users, classes, schedules, sessions, attendances, devices, logs_events
  - Indexes : rfid_uid, session_id, time_stamp
- [ ] Endpoint contracts (OpenAPI) et payload schemas (exemples)
  - `POST /api/session/start` {device_id, teacher_rfid} -> response {session_id}
  - `POST /api/session/:id/scan` {rfid, timestamp, device_id, status}
  - `POST /api/session/:id/end` -> closes session
  - `GET /api/session/:id/report` -> list + summary
  - `POST /api/device/register` -> issue device_token
- [ ] Auth & security
  - Device tokens (MQTT ACL integration), JWT for humans, role-based access
  - Key rotation endpoints + revocation list
- [ ] DB connection management
  - Pooling, retries on transient errors, circuit breaker pattern for DB downtime
  - Health endpoint: DB ok -> 200 else 503
- [ ] Logging & audit
  - All scan events logged to `logs_events` with session_id, rfid, device, outcome, ip, timestamps
  - Audit trail for user actions (start/end session, modifications)
- [ ] Tests
  - Unit tests (jest/pytest), Integration tests (docker-compose with Postgres + Mosquitto) mocked
  - Load tests (k6) to validate concurrency (1000 scans/min goal)
- [ ] Data retention & RGPD
  - Retention policy implementation (cron job purge older than X), export and deletion endpoints
- [ ] Backups & migrations
  - Automated migrations in CI, backup verification script

### E. Frontend (Dashboard) 🖥️
- [ ] Architecture (React + Vite recommended)
- [ ] Real-time data stream via WebSocket or server-sent events (SSE) fed by backend/MQTT bridge
- [ ] Views & features
  - Dashboard Realtime (active session, counts)
  - Session detail (list, present/late/absent, export CSV)
  - Admin (users, classes, schedules)
  - Device view (last_seen, status, logs)
- [ ] Auth (JWT), roles, permissions UI flows
- [ ] Tests: unit, integration, E2E (Cypress) covering main flows
- [ ] Accessibility + i18n (French default)

### F. Docker / Infra / CI/CD 🐳
- [ ] `docker-compose` dev & prod, variables d'env séparées
- [ ] Dockerfiles : multi-stage, small final images, security scanning (Trivy)
- [ ] GitHub Actions (CI)
  - Linting (eslint, flake8), unit-tests, build images, run integration test in job using docker-compose
  - CD : push images to registry, deploy to prod (manual gated for prod)
- [ ] Release process + tags semver, changelog automatisé
- [ ] Secret management : Vault / GitHub secrets, avoid secret in images

### G. Tests / Qualité 🧪
- [ ] Unit tests (coverage >=80% min, 90% for core logic)
- [ ] Integration tests (MQTT → API → DB) via `docker-compose.ci.yml`
- [ ] E2E tests (simulate ESP32 via script publishing MQTT messages)
- [ ] HIL tests (real HW) with automation scripts and CI gating if possible
- [ ] Load testing (k6) and define SLAs (latency <200ms for scan processing)
- [ ] Security scanning (Snyk/Trivy) in CI

### H. Sécurité & opérations 🔐
- [ ] TLS everywhere (API, Broker) + Let's Encrypt automation
- [ ] RBAC + 2FA for admin UI
- [ ] Device onboarding : provisioning token creation, embedded certs or tokens
- [ ] Secret rotation process and emergency key revocation
- [ ] Incident response plan (post-mortem template)

### I. Documentation & Guides 📚
- [ ] README général et per-module README (firmware, backend, frontend, infra)
- [ ] Guides de montage : step-by-step wiring + photos + STL boîtiers
- [ ] API docs (OpenAPI + Swagger UI) + exemples curl
- [ ] PlatformIO : flash instructions, config example for Wi‑Fi/MQTT/Zigbee
- [ ] SOPs : backup restore, device onboarding, emergency rollback

### J. Automatisation & scripts ⚙️
- [ ] Scripts de provisioning Pi (idempotent) + doc
- [ ] DB init / seed / migrate scripts (dev/prod) + test migration rollback
- [ ] Device onboarding CLI : `tools/register_device --name ...` → returns token
- [ ] Replay tools & fixtures : `tools/replay_scans.js` pour tests E2E
- [ ] Monitoring alerts scripts (setup grafana alerts, notify channel)

### K. Cross-cutting / Logs / Monitoring / SLA 📈
- [ ] Logs structurés (JSON) sur tous les composants (firmware: compact JSON; backend: structured logger)
  - Parties : timestamp, level, module, session_id, device_id, trace_id
- [ ] Central log pipeline : promtail -> Loki (Grafana) ou Filebeat -> ELK
- [ ] Metrics : define metrics (scans/sec, failed_scans, sessions_open, latency_ms) + Prometheus instrumentation
- [ ] SLOs & Alerts : define thresholds (error rate >1% triggers alert, queue backlog >1000 events)
- [ ] Observability playbook : how to debug (common queries, dashboards)

### L. Agent / IA checklist (pour ne rien oublier) ✅
- [ ] Lire la demande utilisateur & le `GUIDE.md` → confirmer scope
- [ ] Créer ticket / branch pour la tâche
- [ ] Implémenter code + tests (unit & integration)
- [ ] Mettre à jour docs + exemples
- [ ] Lancer CI (green) + ajouter artefacts (logs/tests)
- [ ] Préparer test manuel si HW nécessaire (étapes claires) et marquer `hw-needed` si pas possible
- [ ] Ouvrir PR avec checklist remplie (tests passés, build ok, docs update)

---
*Cette refonte apporte granularité, critères de validation et points transverses (logs, sécurité, monitoring). Elle intègre les recommandations du `GUIDE.md` (topics MQTT, schéma DB, RGPD, zigbee vs wifi).*
---

## Dépendances & ordre conseillé
1. Pi + Mosquitto + DB (infra minimale)
2. Backend + MQTT listener
3. ESP32 firmware minimal (publish scan) -> tester sur dev broker
4. Frontend + real-time feed
5. Zigbee path and multi-node

---

## Template d'acceptance criteria ✅
- Fonctionalité documentée dans README
- Tests unitaires couvrent logique critique
- Tests d'intégration automatisés passent
- Déploiable via docker-compose
- Instructions de flash claires

---

## Estimations (ordre de grandeur)
- Prototype : 2–3 semaines (1 dev + 1 hw)
- Fonctionnel : 3–4 semaines (2 devs + 1 hw) 
- Production : 1–2 semaines (devops + tests)

---

## Notes pratiques / Tips 💡
- Favoriser MQTT direct (Wi‑Fi) pour prototypage rapide ; garder Zigbee pour version maillée.
- Toujours fournir fixtures et scripts pour rejouer scans en CI.

---

## Fichiers à créer ensuite
- `docker-compose.yml` (template) — voir `docker-compose.template.yml`
- `platformio/` project scaffolds — voir `platformio/README.md`
- `SCHEMATICS.md` (schémas matériels)
- `AI_INSTRUCTIONS.md` (règles pour AIs)


---
*Fichier généré automatiquement — adaptez priorités selon contraintes réelles.*
