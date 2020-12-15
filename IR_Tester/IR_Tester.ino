/*
 * Project:Ewigkeit
 * CodeName:canon+tester_x2
 * Build:2020/12/14
 * Author:torinosubako
*/
#include <M5StickCPlus.h>
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

const uint16_t kRecvPin = 26; //受信ピン
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
#if DECODE_AC
const uint8_t kTimeout = 50;
#else
const uint8_t kTimeout = 15;
#endif
const uint16_t kMinUnknownSize = 12;
#define LEGACY_TIMING_INFO false

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
#if defined(ESP8266)
  //Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
#else
  //Serial.begin(kBaudRate, SERIAL_8N1);
#endif
  while (!Serial)
    delay(50);
  //Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
#if DECODE_HASH
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif
  irrecv.enableIRIn();
}

void loop() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Waiting..");
  if (irrecv.decode(&results)) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.println("Received!!");
    uint32_t now = millis();
    if (results.overflow)
      Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
    String value = String(resultToHexidecimal(&results));
    Serial.println(resultToSourceCode(&results));
    Serial.println(value);
    if (value == "0x5DF2C18E") {
      M5.Lcd.println("light_ON");
    } else if (value == "0x7057E98C") {
      M5.Lcd.println("light_Ch");
    } else if (value == "0xCA0A68FC") {
      M5.Lcd.println("light_OFF");
    } else if (value == "0x2FD48B7") {
      M5.Lcd.println("TV_OFF");
    } else if (value == "0x41C4F807") {
      M5.Lcd.println("Cooling_Fan\nON/OFF");
    } else {
      M5.Lcd.println(value);
    }
#if LEGACY_TIMING_INFO
    Serial.println(resultToTimingInfo(&results));
    yield();
#endif  // LEGACY_TIMING_INFO
    Serial.println("Fin");
    yield();
  }
  delay(2500);
}
