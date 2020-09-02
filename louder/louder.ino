/*
 * Project:Ewigkeit
 * CodeName:canon
 * Build:2020/09/02
 * Author:torinosubako
*/

// 環境に合わせてライブラリーを選択
//#include <M5StickCPlus.h>
#include <M5Stack.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>


// ピン指定情報
// M5StickCのIRは9ピン
// M5Stack-PLUSEMのIRは13ピン
// M5Stack＋IR-UNITのIRは26ピン
const uint16_t Ir_Led = 13;
IRsend irsend(Ir_Led);



void setup() {
  irsend.begin();
  M5.begin();
  
  //　送信確認用赤色LEDセット(M5StickC/plusのみ)
  //pinMode(GPIO_NUM_10, OUTPUT);
  
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("IR Remote Control");
  M5.Lcd.println("Push the button");
}

void loop() {
  M5.update();
  M5.Lcd.setCursor(0, 0, 1);
  // ボタンA(BtnA)が押されたとき
  if (M5.BtnA.wasPressed()) {
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("IR Remote Control");
    M5.Lcd.println("Pressed Button-A");
    M5.Lcd.println("Send infrared signal");
    
    // 山善・扇風機・電源
    irsend.sendNEC(0x41C4F807, 32);
    delay(1000);
    
    //東芝・液晶テレビ・電源
    //irsend.sendNEC(0x2FD48B7, 32);
    //delay(1000);
    
    //　送信確認用赤色LED(M5StickC/plusのみ)
    // digitalWrite(GPIO_NUM_10, LOW);
    // delay(100);
    // digitalWrite(GPIO_NUM_10, HIGH);
    
    //　操作待ち画面へ遷移
    M5.Lcd.setCursor(0, 0, 1);
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("IR Remote Control");
    M5.Lcd.println("Push the button");
    }
    
  // ボタンB(BtnB)が押されたとき(M5Stack用)
  if (M5.BtnB.wasPressed()) {
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("IR Remote Control");
    M5.Lcd.println("Pressed Button-B");
    M5.Lcd.println("Send infrared signal");
    
    // 山善・扇風機・風量
    irsend.sendNEC(0x41C4D827, 32);
    delay(1000);
    
    //　操作待ち画面へ遷移
    M5.Lcd.setCursor(0, 0, 1);
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("IR Remote Control");
    M5.Lcd.println("Push the button");
    }

    // ボタンC(BtnC)が押されたとき(M5Stack用)
  if (M5.BtnC.wasPressed()) {
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("IR Remote Control");
    M5.Lcd.println("Pressed Bbutton-C");
    M5.Lcd.println("Send infrared signal");
    
    // 山善・扇風機・リズム
    irsend.sendNEC(0x41C4E817, 32);
    delay(1000);
    
    //　操作待ち画面へ遷移
    M5.Lcd.setCursor(0, 0, 1);
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("IR Remote Control");
    M5.Lcd.println("Push the button");
    }
}
