/*
 * Project:Ewigkeit
 * CodeName:canon
 * Build:2020/08/26
 * Author:torinosubako
*/

#include <M5StickCPlus.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// M5StickCのIRは9ピン
const uint16_t Ir_Led = 9;
IRsend irsend(Ir_Led);

void setup() {
  irsend.begin();

  M5.begin();
  pinMode(GPIO_NUM_10, OUTPUT);
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("IR Remote Control");
  M5.Lcd.println("Press M5 button");
}

void loop() {
  M5.update();

  // M5ボタン(BtnA)が押されたとき
  if (M5.BtnA.wasPressed()) {
    // ここで赤外線信号を送信する
    // 山善・扇風機・電源
    //irsend.sendNEC(0x41C4F807, 32);
    //delay(100);
    //東芝・液晶テレビ・電源
    digitalWrite(GPIO_NUM_10, LOW);
    irsend.sendNEC(0x2FD48B7, 32);
    delay(100);
    digitalWrite(GPIO_NUM_10, HIGH);
  }
}
