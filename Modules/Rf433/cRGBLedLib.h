
#ifndef __C_RGB_LED_LIB__
#define __C_RGB_LED_LIB__

#include <Arduino.h>

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
    static cRGBLedLib *m_instance;
    bool m_is_anode = false;
    int m_PIN_LED_R;
    int m_PIN_LED_G;
    int m_PIN_LED_B;
    
  public:
    static cRGBLedLib *instance() {
      if(cRGBLedLib::m_instance == nullptr){
        cRGBLedLib::m_instance = new cRGBLedLib();
      }
      return cRGBLedLib::m_instance;
    }
    bool init(int PIN_LED_R = 0, int PIN_LED_G = 1, int PIN_LED_B = 2, bool is_anode = false) {

      Serial.println("Init LEDLib");
      m_PIN_LED_R = PIN_LED_R;
      Serial.print("LED Red:");
      Serial.println(PIN_LED_R);
      m_PIN_LED_G = PIN_LED_G;
      Serial.print("LED Green:");
      Serial.println(PIN_LED_G);
      m_PIN_LED_B = PIN_LED_B;
      Serial.print("LED Blue:");
      Serial.println(PIN_LED_B);
      
      pinMode(m_PIN_LED_R, OUTPUT);
      pinMode(m_PIN_LED_G, OUTPUT);
      pinMode(m_PIN_LED_B, OUTPUT);


      return true;
    }
    void displayColor(byte color);
};


void cRGBLedLib::displayColor(byte color) {
  if(m_is_anode) {
  // Version anode commune
    digitalWrite(m_PIN_LED_R, !bitRead(color, 2));
    digitalWrite(m_PIN_LED_G, !bitRead(color, 1));
    digitalWrite(m_PIN_LED_B, !bitRead(color, 0));
  }
  else
  {
  // Version cathode commune
    digitalWrite(m_PIN_LED_R, bitRead(color, 2));
    digitalWrite(m_PIN_LED_G, bitRead(color, 1));
    digitalWrite(m_PIN_LED_B, bitRead(color, 0));
  }

}

 cRGBLedLib * cRGBLedLib::m_instance = nullptr;

#endif