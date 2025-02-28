# RC433 avec ESP32

## Aperçu
Ce projet utilise un ESP32 pour se connecter à un réseau Wi-Fi, communiquer via MQTT, et transmettre des signaux RF 433 MHz afin d'émuler une télécommande connectée. Une LED RGB est utilisée pour fournir un retour visuel sur l'état de la connexion Wi-Fi, de MQTT et des transmissions RF.

### Objectifs
- Usurper un signal RF provenant d'une télécommande via des messages MQTT.
- Afficher des états visuels via une LED RGB (connexion Wi-Fi, MQTT, transmission).
- Assurer une connexion stable au réseau Wi-Fi et au broker MQTT.

### Matériel requis
- **ESP32** : Microcontrôleur avec Wi-Fi intégré.
- **Émetteur RF 433 MHz** : Connecté à la broche `RXD2` (GPIO13).
- **LED RGB** : Connectée aux broches GPIO35, GPIO32, GPIO33.
- Réseau Wi-Fi et broker MQTT (ex. Mosquitto).

---

## Montage

@TODO

## Structure du code

### Librairies utilisées
- **`WiFi.h`** : Gestion de la connexion Wi-Fi.
- **`PubSubClient.h`** : Communication MQTT.
- **`Arduino.h`** : Fonctions de base Arduino.
- **`RCSwitch.h`** : Gestion de l'émetteur RF 433 MHz.
- **`cRGBLedLib.h`** : Contrôle de la LED RGB (bibliothèque personnalisée).

### Constantes et variables globales
- **Wi-Fi** :
  - `ssid` : Nom du réseau Wi-Fi (à personnaliser).
  - `password` : Mot de passe du réseau (à personnaliser).
- **MQTT** :
  - `mqtt_server` : Adresse IP du broker MQTT (par défaut "127.0.0.1").
- **RF** :
  - `RXD2` (GPIO13) : Broche de l'émetteur RF.
- **Délais** :
  - `WAITING_DELAY` : Délai de 10 secondes entre deux exécutions (non utilisé dans le code actuel).
- **LED RGB** : Gestion via `cRGBLedLib`.

### Fonctions principales
1. **`setup()`** :
   - Initialise la communication série (115200 bauds).
   - Établit la connexion Wi-Fi.
   - Configure le serveur MQTT (port 1883) et le callback MQTT.
   - Initialise la LED RGB (broches 35, 32, 33).
   - Configure l'émetteur RF avec protocole 2, longueur d'impulsion 320 µs, 15 répétitions.

2. **`loop()`** :
   - Vérifie la connexion Wi-Fi :
     - Si connecté : Affiche "OK", allume la LED en blanc (5s), puis rouge (5s).
     - Si déconnecté : Tente de se reconnecter au Wi-Fi.
   - **Note** : Les sections MQTT (`client.loop()`, `reconnect()`) sont commentées mais peuvent être activées.

3. **`callback(char* topic, byte* message, unsigned int length)`** :
   - Fonction appelée lorsqu'un message MQTT est reçu.
   - Si le topic est `"rc/salon/transmit"` :
     - Convertit le message en entier (code RF).
     - Allume la LED en cyan, envoie le code RF via `mySwitchT.send()`, puis éteint la LED.

4. **`reconnect()`** :
   - Tente de se reconnecter au broker MQTT si la connexion est perdue.
   - Affiche des couleurs sur la LED pour indiquer l'état :
     - Magenta : Tentative de connexion.
     - Jaune : En cours.
     - Noir : Connecté.
     - Rouge : Échec (nouvelle tentative après 5s).
   - S'abonne au topic `"rc/salon/transmit"`.

5. **`connect_to_wifi()`** :
   - Initialise la connexion Wi-Fi avec `ssid` et `password`.
   - Affiche des points dans la console et alterne les couleurs blanc/cyan sur la LED pendant la tentative.
   - Une fois connecté, affiche l'adresse IP et éteint la LED.

---

## Fonctionnement
1. **Démarrage** :
   - L'ESP32 tente de se connecter au Wi-Fi.
   - Si MQTT est activé (code décommenté), il se connecte au broker.

2. **Mode connecté** :
   - Si Wi-Fi est actif, la LED affiche blanc puis rouge en boucle (test visuel).
   - Si un message MQTT arrive sur `"rc/salon/transmit"` (ex. "123456"), ce code est transmis via RF.

3. **Mode déconnecté** :
   - Si Wi-Fi est perdu, l'ESP32 tente de se reconnecter avec un feedback visuel.

### Exemple d'utilisation MQTT
- Commande : `mosquitto_pub -h 127.0.0.1 -t "rc/salon/transmit" -m "123456"`
- Résultat : L'ESP32 envoie le code RF `123456` (24 bits) et la LED passe au cyan pendant 0,5s.

---

## Configuration
1. **Wi-Fi** :
   - Remplacez `your_ssid` et `your_password` par vos identifiants.
2. **MQTT** :
   - Mettez à jour `mqtt_server` avec l'adresse de votre broker (ex. "192.168.1.100").
3. **RF** :
   - Le protocole (2), la longueur d'impulsion (320 µs) et les répétitions (15) sont configurés pour une télécommande spécifique. Ajustez selon votre matériel.

---

## Installation
1. **Dépendances** :
   - Installez les librairies Arduino : `WiFi`, `PubSubClient`, `RCSwitch`.
   - Ajoutez `cRGBLedLib.h` (non standard, à fournir ou adapter).
2. **Téléversement** :
   - Utilisez la commande fournie pour flasher le firmware :
     ```
     python3 /home/user/snap/arduino/85/.arduino15/packages/esp32/tools/esptool_py/4.6/esptool.py -p /dev/ttyUSB0 -b 115200 --before default_reset --after hard_reset --chip esp32 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x10000 /home/user/Downloads/esp32-test-firmware.factory.bin
     ```
   - Remplacez `/dev/ttyUSB0` par votre port série et le chemin du fichier `.bin` par le vôtre.

---

## Débogage
- **Console série** : Ouvrez le moniteur série à 115200 bauds pour voir les messages ("OK", IP, erreurs MQTT).
- **LED RGB** :
  - Blanc/Cyan clignotant : Connexion Wi-Fi en cours.
  - Blanc puis Rouge : Wi-Fi connecté.
  - Cyan : Transmission RF.
  - Magenta/Jaune/Rouge : Problèmes MQTT.
