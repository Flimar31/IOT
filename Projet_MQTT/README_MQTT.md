# Système MQTT - Communication ESP32

## 🎯 Description

Ce projet permet la communication entre deux ESP32 via MQTT :
- **Projet1** : Publisher - Envoie les pressions de boutons via MQTT
- **Projet2** : Subscriber - Reçoit les messages MQTT et les affiche avec le logger

## 📋 Prérequis

1. **Docker Desktop** installé et lancé
2. **PlatformIO** configuré dans VS Code
3. Deux ESP32 connectés
4. Connexion WiFi "iPhone de Matéo" disponible

## 🚀 Instructions de démarrage

### 1. Lancer le broker MQTT

Double-cliquez sur le fichier `start_mqtt_broker.bat`

Le script va :
- Vérifier que Docker est lancé
- Télécharger l'image Mosquitto si nécessaire
- Lancer le broker sur le port 1883

### 2. Configurer l'adresse IP du broker

**IMPORTANT** : Avant de téléverser le code sur les ESP32, modifiez la variable `mqtt_server` dans les deux projets :

Dans [Projet1/src/main.cpp](Projet1/src/main.cpp) et [Projet2/src/main.cpp](Projet2/src/main.cpp), remplacez :

```cpp
const char* mqtt_server = "localhost";
```

Par l'adresse IP de votre PC (celui qui exécute Docker). Pour la trouver :

**Windows** :
```bash
ipconfig
```
Cherchez l'adresse IPv4 de votre connexion WiFi (ex: 192.168.1.100)

### 3. Téléverser le code

**Projet1 (Publisher)** :
```bash
cd Projet1
pio run --target upload
pio device monitor
```

**Projet2 (Subscriber)** :
```bash
cd Projet2
pio run --target upload
pio device monitor
```

## 📡 Fonctionnement

### Projet1 - Publisher
- Quand un bouton est pressé, il envoie un message MQTT avec la couleur
- Topic utilisé : `esp32/button`
- Messages : "JAUNE", "NOIR", "ROUGE", "BLEU", "VERT"

### Projet2 - Subscriber
- S'abonne au topic `esp32/button`
- Reçoit les messages et les affiche avec les couleurs appropriées
- Utilise le logger pour formater les messages

## 🎨 Boutons et couleurs

- **Pin 27** → JAUNE
- **Pin 13** → NOIR
- **Pin 12** → ROUGE
- **Pin 25** → BLEU
- **Pin 26** → VERT

## 🔧 Commandes Docker utiles

**Voir les logs du broker** :
```bash
docker logs -f mqtt-broker
```

**Arrêter le broker** :
```bash
docker stop mqtt-broker
```

**Redémarrer le broker** :
```bash
docker restart mqtt-broker
```

**Supprimer le conteneur** :
```bash
docker stop mqtt-broker
docker rm mqtt-broker
```

## 🐛 Dépannage

### L'ESP32 ne se connecte pas au broker

1. Vérifiez que le broker Docker est lancé
2. Vérifiez l'adresse IP dans `mqtt_server`
3. Assurez-vous que l'ESP32 et le PC sont sur le même réseau WiFi
4. Vérifiez que le port 1883 n'est pas bloqué par le pare-feu

### Messages d'erreur "rc=-2"

Cela signifie que l'ESP32 ne peut pas atteindre le broker :
- Vérifiez l'IP du serveur MQTT
- Ping l'adresse depuis votre PC pour vérifier la connectivité

### Le Projet2 ne reçoit rien

1. Vérifiez que le Projet2 affiche "Abonné au topic : esp32/button"
2. Vérifiez que le Projet1 affiche "Message MQTT envoyé"
3. Consultez les logs Docker pour voir si les messages arrivent au broker

## 📝 Architecture

```
┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│   Projet1    │         │    Docker    │         │   Projet2    │
│   (ESP32)    │──MQTT──▶│   Mosquitto  │──MQTT──▶│   (ESP32)    │
│  Publisher   │         │    Broker    │         │  Subscriber  │
└──────────────┘         └──────────────┘         └──────────────┘
     Boutons                Port 1883               Logger coloré
```

## ✅ Vérification

Pour tester que tout fonctionne :

1. Lancez `start_mqtt_broker.bat`
2. Téléversez et monitorez le Projet2
3. Téléversez et monitorez le Projet1
4. Appuyez sur un bouton sur le Projet1
5. Vous devriez voir le message apparaître sur le Projet2 avec la couleur appropriée !

---

**Note** : Les deux ESP32 peuvent également utiliser leurs boutons locaux. Le Projet1 publiera toujours sur MQTT, tandis que le Projet2 marquera les boutons locaux avec "LOCAL" dans les logs.
