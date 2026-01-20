Projet de sécurité batiment scolaire avec pointage/appel et gestion des retards

Matériel initial :
2 ESP32
1 Module RFID
Cartes RFID
Badge RFID
1 Afficheur LCD 16x2
1 Buzzer
Traffic light module
Boutons poussoirs
Câbles de connexion
Breadboard
Raspberry Pi

Il doit y'avoir minimum 2 ESP32 dans le projet, une communication entre les ESP32 par le protocole Zigbee avoir un rasberry pi pour la des passerelle zigbee et un serveur web pour la gestion des données.

Fonctionnalités principales :
Badge pour prof et cartes pour élèves

Pointage/Appel des élèves via RFID
Le prof scanne son badge pour démarrer l'appel le feu passe au vert une fois l'appel démarré les élèves scannent leur carte RFID pour pointer leur présence une fois terminé le prof re-scanne son badge pour terminer l'appel et déclencher l'envoi des données au serveur web via le raspberry pi le feu passe au orange et la phase de gestion des retards commence les élèves retardataires peuvent scanner leur carte RFID pour signaler leur retard une fois le cours fini le prof re-scanne ça carte pour cloturer le cours le feu passe au rouge pendant 5 minutes puis s'étteint si y'a un autre cours le feu orange clignote pour dire au prof qu'il peut scanner ça carte

il y'a donc une gestion des cartes, des permissions prof / elèves et des nom, des horaires et des emploits du temps, des classes