
/*
 * Project:Ewigkeit
 * CodeName:Zeit
 * Build:2020/09/16
 * Author:torinosubako
*/

// 環境に合わせてライブラリーを選択
//#include <M5StickCPlus.h>
#include <M5Stack.h>

// 連携系ライブラリ
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <driver/rtc_io.h>

// 赤外線制御系ライブラリ(canon継承)
#include <IRremoteESP8266.h>
#include <IRsend.h>


// ピン指定情報
// M5StickCのIRは9ピン
// M5Stack-PLUSEMのIRは13ピン
// M5Stack＋IR-UNITのIRは26ピン
// 各GPIOに接続時は所定PINを指定のこと
const uint16_t Ir_Led = 13;
IRsend irsend(Ir_Led);

// Wi-Fi・Firebase設定用データ
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

//連続起動時リフレッシュ用変数
int reno_cont;
int reno_limit = 20;


void setup() {
  //(canon継承)基幹機能引継部
  Serial.begin(115200);
  irsend.begin();
  M5.begin();
  M5.Lcd.clear();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("IR Remote Control");
  M5.Lcd.println("Push the button");
  //　送信確認用赤色LEDセット(M5StickC/plusのみ)
  //pinMode(GPIO_NUM_10, OUTPUT);

  //Wi-Fi初期設定・接続確認部
  //Wi-Fi接続試験(2.5sec)
  WiFi.begin(ssid, password);
  delay(2500);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2500);
    wifi_cont ++;
    Serial.println("Connecting to WiFi..");
    Serial.println( wifi_cont);
    if (wifi_cont >= 3){
      M5.Power.reset();
    }
  }
  Serial.println("初期リンクを確立しました");
  Serial.println(WiFi.localIP());
  
  
}

void loop() {
  
  //Wi-Fi接続試験(2.5sec)
  WiFi.begin(ssid, password);
  delay(2500);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2500);
    wifi_cont ++;
    Serial.println("Connecting to WiFi..");
    Serial.println( wifi_cont);
    if (wifi_cont >= 3){
      M5.Power.reset();
    }
  }
  Serial.println("リンクを確立しました");
  Serial.println(WiFi.localIP());

  //
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
  }
    
  //　操作待ち画面へ遷移
  M5.Lcd.setCursor(0, 0, 1);
  M5.Lcd.clear(BLACK);
  M5.Lcd.println("IR Remote Control");
  M5.Lcd.println("Push the button");
}
