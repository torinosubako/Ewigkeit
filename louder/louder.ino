/*
 * Project:Ewigkeit
 * CodeName:Zeit(demonstrator)
 * Build:2020/10/13
 * Author:torinosubako
*/

// これ以上の開発はM5StickCでは不可能(メモリ不足でコンパイルできない)

// 環境に合わせてライブラリーを選択
#include <M5StickCPlus.h>
//#include <M5Stack.h>

// 連携系ライブラリ
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <driver/rtc_io.h>
WiFiClient client;

// 赤外線制御系ライブラリ(canon継承)
#include <IRremoteESP8266.h>
#include <IRsend.h>


// ピン指定情報
// M5StickCのIRは9ピン
// M5Stack-PLUSEMのIRは13ピン
// M5Stack＋IR-UNITのIRは26ピン
// 各GPIOに接続時は所定PINを指定のこと
const uint16_t Ir_Led = 9;
IRsend irsend(Ir_Led);

// Wi-Fi・Firebase設定用データ


const char *ssid = //Your Network SSID//;
const char *password = //Your Network Password//;
const String  host_url = //Your Host Url//;
const String auth_key = //Your Auth Key//;
const String Reset_key = //Your Reset Key//;

//連続起動時リフレッシュ用変数
int reno_cont;
int reno_limit = 20;
int wifi_cont;


void setup() {
  //(canon継承)基幹機能引継部
  Serial.begin(115200);
  irsend.begin();
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("IR Remote Controler");
  M5.Lcd.println("Zeit-Demonstrator:");
  M5.Lcd.println("Fire-Bird");
  //　送信確認用赤色LEDセット(M5StickC/plusのみ)
  pinMode(GPIO_NUM_10, OUTPUT);

  //Wi-Fi初期設定・接続確認部
  //Wi-Fi接続試験
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    wifi_cont ++;
    Serial.println("Connecting to WiFi..");
    Serial.println( wifi_cont);
    if (wifi_cont >= 3){
      esp_restart();
    }
  }
  Serial.println("初期リンクを確立しました");
  Serial.println(WiFi.localIP());
  
  
}

void loop() {
  
  //Wi-Fi接続試験
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    wifi_cont ++;
    Serial.println("Connecting to WiFi..");
    Serial.println( wifi_cont);
    if (wifi_cont >= 3){
      esp_restart();
    }
  }
  Serial.println("リンクを確立しました");
  Serial.println(WiFi.localIP());

  //情報処理関数(あとで関数化予定)
  HTTPClient http;
  http.begin(host_url + auth_key); //URLを指定
  int httpCode = http.GET();  //GETリクエストを送信

  if (httpCode > 0) { //返答がある場合
    String payload = http.getString();  //返答（JSON形式）を取得
    Serial.println(httpCode);
    Serial.println(payload);
    //jsonオブジェクトの作成
    String json = payload;
    DynamicJsonDocument besedata(4096);
    deserializeJson(besedata, json);
    //リソースを解放
    http.end(); 
    //フラグメント抽出・識別
    const int frag = besedata["Fragment"];
    Serial.println("データ受信結果");
    Serial.print(frag);
    if (frag == 0){
      Serial.println("\n前回更新時から変更はありませんでした");
      digitalWrite(GPIO_NUM_10, LOW);
      delay(100);
      digitalWrite(GPIO_NUM_10, HIGH);
    } else {
      Serial.println("前回更新時から変更がありました");
      //詳細識別部
      //開発中
      //フラグメントリセット部(関数化？)
      //開発中
      //fragment_reset();
    }
  } else {
    Serial.println("Error on HTTP request");
    http.end(); //リソースを解放
  }
}
/* 
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

void fragment_reset(){
  DynamicJsonDocument key_zero(5);
  key_zero = "0";
  String json_key;
  serializeJson(key_zero, json_key);
  HTTPClient http;
  http.begin(host_url + Reset_key);
  http.POST(json_key);
  http.end();
  Serial.println("R");
  }
  */
  
