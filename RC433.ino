#include <WiFi.h>
#include <PubSubClient.h>
#include <RCSwitch.h>

const byte COLOR_BLACK = 0b000;
const byte COLOR_RED = 0b100;
const byte COLOR_GREEN = 0b010;
const byte COLOR_BLUE = 0b001;
const byte COLOR_MAGENTA = 0b101;
const byte COLOR_CYAN = 0b011;
const byte COLOR_YELLOW = 0b110;
const byte COLOR_WHITE = 0b111;


class cRGBLedLib {
private:
  cRGBLedLib() {}
  virtual ~cRGBLedLib() {}
  static cRGBLedLib* m_instance;
  bool m_is_anode = false;
  int m_PIN_LED_R;
  int m_PIN_LED_G;
  int m_PIN_LED_B;

public:
  static cRGBLedLib* instance() {
    if (cRGBLedLib::m_instance == nullptr) {
      cRGBLedLib::m_instance = new cRGBLedLib();
    }
    return cRGBLedLib::m_instance;
  }
  bool init(int PIN_LED_R = 0, int PIN_LED_G = 1, int PIN_LED_B = 2, bool is_anode = false) {

    m_PIN_LED_R = PIN_LED_R;
    m_PIN_LED_G = PIN_LED_G;
    m_PIN_LED_B = PIN_LED_B;

    pinMode(m_PIN_LED_R, OUTPUT);
    pinMode(m_PIN_LED_G, OUTPUT);
    pinMode(m_PIN_LED_B, OUTPUT);


    return true;
  }
  void displayColor(byte color);
};


void cRGBLedLib::displayColor(byte color) {
  if (m_is_anode) {
    // Version anode commune
    digitalWrite(m_PIN_LED_R, !bitRead(color, 2));
    digitalWrite(m_PIN_LED_G, !bitRead(color, 1));
    digitalWrite(m_PIN_LED_B, !bitRead(color, 0));
  } else {
    // Version cathode commune
    digitalWrite(m_PIN_LED_R, bitRead(color, 2));
    digitalWrite(m_PIN_LED_G, bitRead(color, 1));
    digitalWrite(m_PIN_LED_B, bitRead(color, 0));
  }
}

cRGBLedLib* cRGBLedLib::m_instance = nullptr;


RCSwitch mySwitchT = RCSwitch();

#define RXD2 13  // Transmitter

//**** WIFI ****/
const char* ssid = "SFR_B148";
const char* password = "8jjt66btzf3ibw23hsbm";

/*const char* ssid = "GSR_Wifi";
const char* password = "P4ssw0rd_w!f!_Str0ng";*/

//**** MQTT ****/
const char* mqtt_server = "192.168.1.56";

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
const long WAITING_DELAY = 10000;  //D: DELAY BETWEEN TWO EXECUTIONS

void setup() {
  Serial.begin(115200);
  delay(1000);

  connect_to_wifi();

  if (cRGBLedLib::instance()->init(34, 35, 33, false)) {
    cRGBLedLib::instance()->displayColor(COLOR_GREEN);
  }

  mySwitchT.enableTransmit(RXD2);
  mySwitchT.setProtocol(2);
  mySwitchT.setPulseLength(320);
  mySwitchT.setRepeatTransmit(15);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if (WiFi.status() == WL_CONNECTED) {
    //- ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ SERVICE CODE

    //- ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ END SERVICE CODE
  } else  //D: NOT CONNECTED
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
    Serial.print("Sending: ");
    Serial.println(atoi(messageTemp.c_str()));
    mySwitchT.send(atoi(messageTemp.c_str()), 24);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe=
      client.subscribe("rc/salon/transmit");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void connect_to_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Attente de connexion");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    cRGBLedLib::instance()->displayColor(COLOR_CYAN);
    delay(250);
    Serial.print(".");
    cRGBLedLib::instance()->displayColor(COLOR_BLUE);
    delay(250);
    Serial.print(".");
    cRGBLedLib::instance()->displayColor(COLOR_CYAN);
    delay(250);
    Serial.println("\n");
    cRGBLedLib::instance()->displayColor(COLOR_BLUE);
    delay(250);
  }
  cRGBLedLib::instance()->displayColor(COLOR_GREEN);

  Serial.println("Connexion etablie !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  delay(500);
  cRGBLedLib::instance()->displayColor(COLOR_BLACK);
}
