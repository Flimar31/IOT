# AI_INSTRUCTIONS — Règles & Guideline pour AIs travaillant sur le projet 🤖📋

> Objectif : normaliser le travail des IA/agents pour garantir qualité, tests, traçabilité et conformité aux demandes utilisateurs.

## Règles impératives (must follow)
1. Lire et comprendre l'issue / ticket / requête avant tout travail.
2. Créer une branche dédiée (`feat/<short-desc>`), faire des commits atomiques avec message clair (convention : `type(scope): short message`).
3. **Tests obligatoires** : écrire tests unitaires et d'intégration couvrant les nouvelles features. Aucune PR n'est validée sans tests automatiques passés.
4. Lint & format : faire passer linters (ESLint, clang-format, black, etc.) avant PR.
5. CI : chaque PR déclenche pipelines (lint → unit tests → build → integration). Les tests doivent être fiables (mock hardware si nécessaire).
6. Documentation : tout changement doit inclure mise à jour README / changelog / API docs.
7. Vérifier et documenter les effets secondaires (migration DB, breaking changes).
8. Avant de marquer tâche comme DONE : exécuter la check-list d'acceptation (voir ci-dessous) et joindre logs/tests au PR.

## Checklist d'acceptation (à joindre au PR)
- [ ] Code commenté & lisible
- [ ] Tests unitaires ajoutés et passés
- [ ] Tests d'intégration passés (simuler MQTT / DB)
- [ ] Build Docker local fonctionne (`docker-compose up --build`)
- [ ] Documentation mise à jour
- [ ] Sécurité : aucune clé en clair

## Bonnes pratiques pour productivité
- Utiliser TDD pour features critiques (firmware, auth).
- Fournir snapshots et fixtures (scans simulés) pour tests reproductibles.
- Écrire scripts d'automatisation : `scripts/run_tests.sh`, `scripts/replay_scan.js`.
- Préparer environnements de dev locaux via `docker-compose.override.yml`.

## Tests hardware & CI
- Simuler HW dans CI via mocks ou Stub MQTT publishers
- Pour tests réels (hardware-in-loop), automatiser flash + run via PlatformIO CLI et capturer logs
  - Commande exemple : `pio run -e esp32dev -t upload && platformio test -e esp32dev`

## Communication & reporting
- Chaque PR doit inclure : résumé, étapes de test, artefacts (logs, screenshots), nom de l'issue associée.
- Si une IA ne peut exécuter un test matériel, marquer l'étape comme `hw-needed` et documenter exact steps manuels.

## Pour forcer complétude des demandes utilisateur
- Toujours referencer la demande utilisateur originale dans le ticket et vérifier que chaque point est traité.
- Si quelque chose est hors scope ou nécessitant validation, ouvrir une note d'arbitrage et demander confirmation explicite.


---
*Ces règles sont à appliquer strictement par tous les agents/IA collaborant au projet.*
