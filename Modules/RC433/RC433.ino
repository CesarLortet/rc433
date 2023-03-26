#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <RCSwitch.h>

#include "cRGBLedLib.h"
  
RCSwitch mySwitchT = RCSwitch();

#define RXD2 13 // Transmitter

//**** WIFI ****/
const char* ssid = "SFR_B148";
const char* password = "8jjt66btzf3ibw23hsbm";

//**** MQTT ****/
const char* mqtt_server = "192.168.1.20";

//**** VARS ****/
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
long lastPing = 0;

char msg[50];

int value = 0; 
int buttonState = 0;

//**** PROTOTYPES *********/
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void connect_to_wifi();
void displayColor(byte color);

unsigned long last_time = 0;
const long WAITING_DELAY = 10000; //D: DELAY BETWEEN TWO EXECUTIONS

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  connect_to_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  cRGBLedLib::instance()->init(35, 32, 33, false);

  mySwitchT.enableTransmit(RXD2);
  mySwitchT.setProtocol(2);
  mySwitchT.setPulseLength(320);
  mySwitchT.setRepeatTransmit(15);
}

void loop() {
   /* if (!client.connected()) {
      reconnect();
    }
    
    client.loop();*/

    if(WiFi.status() == WL_CONNECTED)
    {
      //- ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ SERVICE CODE
        Serial.println("OK");
        cRGBLedLib::instance()->displayColor(COLOR_WHITE);
        delay(5000);
        cRGBLedLib::instance()->displayColor(COLOR_RED);
        delay(5000);

      
      //- ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ END SERVICE CODE
    }
    else //D: NOT CONNECTED
    {
      connect_to_wifi();
    }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "rc/salon/transmit") {
      cRGBLedLib::instance()->displayColor(COLOR_CYAN);
      mySwitchT.send(atoi(messageTemp.c_str()), 24);
      delay(500);
      cRGBLedLib::instance()->displayColor(COLOR_BLACK);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    cRGBLedLib::instance()->displayColor(COLOR_MAGENTA);
    Serial.print("Attempting MQTT connection...");
    cRGBLedLib::instance()->displayColor(COLOR_YELLOW);
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      cRGBLedLib::instance()->displayColor(COLOR_BLACK);
      Serial.println("connected");
      // Subscribe
      client.subscribe("rc/salon/transmit");
    } else {
      cRGBLedLib::instance()->displayColor(COLOR_RED);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void connect_to_wifi()
{
  WiFi.persistent(false);
  WiFi.begin(ssid, password);
  Serial.print("Attente de connexion");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    cRGBLedLib::instance()->displayColor(COLOR_WHITE);
    delay(250);
    Serial.print(".");
    cRGBLedLib::instance()->displayColor(COLOR_CYAN);
    delay(250);
    Serial.print(".");
    cRGBLedLib::instance()->displayColor(COLOR_WHITE);
    delay(250);
    Serial.println("\n");
    cRGBLedLib::instance()->displayColor(COLOR_CYAN);
    delay(250);
  }

  Serial.println("Connexion etablie !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  cRGBLedLib::instance()->displayColor(COLOR_BLACK);
}
