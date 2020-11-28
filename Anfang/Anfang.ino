/*
 * Project:Ewigkeit
 * CodeName:Anfang(Full_Version)
 * Build:2020/11/24
 * Author:torinosubako
*/
// 連携系ライブラリ(Zeit継承)
#include <M5Stack.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <driver/rtc_io.h>
WiFiClient client;

// 赤外線制御系ライブラリ(canon継承)
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Toshiba.h>

//IRピン指定情報
//M5Stack-PLUSEMのIRは13ピン,M5Stack＋IR-UNITのIRは26ピン
//各GPIOに接続時は所定PINを指定のこと
const uint16_t Ir_Led = 26;
IRsend irsend(Ir_Led);
IRToshibaAC ac(Ir_Led);

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


//基幹機能セットアップ
void setup() {
  Serial.begin(115200);
  ac.begin();
  irsend.begin();
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("IR Remote Controler");
  M5.Lcd.println("Anfang-Demonstrator:");
  M5.Lcd.println("Ringing Bloom");

  //送信確認用LEDセット(必要なら)
  //pinMode(GPIO_NUM_10, OUTPUT);
  
  //Wi-Fi初期設定・接続確認部
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

//作動部
void loop() {
  
  //Wi-Fi接続
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

  //情報処理関数
  HTTPClient http;
  http.begin(host_url + auth_key); //URLを指定
  int httpCode = http.GET();  //GETリクエストを送信

  if (httpCode > 0) { //返答がある場合
    String payload = http.getString();  //返答（JSON形式）を取得
    Serial.println(httpCode);
    Serial.println(payload);
    //jsonオブジェクトの作成
    String json = payload;
    DynamicJsonDocument besedata(768);
    deserializeJson(besedata, json);
    //リソースを解放
    http.end(); 
    //フラグメント抽出・識別
    const int frag = besedata["Fragment"]["key"];
    Serial.println("データ受信結果");
    Serial.print(frag);
    if (frag == 0){
      Serial.println("\n前回更新時から変更はありませんでした");
      digitalWrite(GPIO_NUM_10, LOW);
      delay(100);
      digitalWrite(GPIO_NUM_10, HIGH);
    } else {
      Serial.println("\n前回更新時から変更がありました");
      //詳細識別部
      ir_post(json);
      //フラグメントリセット部
      fragment_reset(Reset_key);
    }
  } else {
    Serial.println("Error on HTTP request");
  }
  delay(600000);
}

//リモコン関数(PI:Andromeda)
void ir_post(String json){
  DynamicJsonDocument besedata(768);
  deserializeJson(besedata, json);
  //デバイスフラグメントキー取得
  const int AC_flag = besedata["Air_Con"]["Fragment"]["key"];
  const int CF_flag = besedata["Cooling_Fan"]["Fragment"]["key"];
  const int li_flag = besedata["Light"]["Fragment"]["key"];
  
  //エアコン制御
  const int AC_Mode = besedata["Air_Con"]["Mode"]["key"];
  const int AC_Temp = besedata["Air_Con"]["Temp"]["key"];
  const int AC_Fan = besedata["Air_Con"]["Fan"]["key"];
  //扇風機制御
  const int CF_Mode = besedata["Cooling_Fan"]["Mode"]["key"];
  //テレビ制御 
  const int TV_Off = besedata["TV"]["Off"]["key"];
  //照明制御
  const int li_Mode = besedata["Light"]["Mode"]["key"];
  
  //エアコン
  if (AC_flag == 1){
    //赤外線デバイス初期化
    ac.begin(); 
    if (AC_Mode == 0) {
      Serial.println("エアコン制御:Auto_Mode");
      ac.on();
      ac.setMode(kToshibaAcAuto);
    } else if (AC_Mode == 1) {
      Serial.println("エアコン制御:Cooling");
      ac.on();
      ac.setMode(kToshibaAcCool);
    } else if (AC_Mode == 2) {
      Serial.println("エアコン制御:Drying");
      ac.on();
      ac.setMode(kToshibaAcDry);
    } else if (AC_Mode == 3) {
      Serial.println("エアコン制御:Heating");
      ac.on();
      ac.setMode(kToshibaAcHeat);
    } else if (AC_Mode == 4) {
      Serial.println("エアコン制御:Blower");
      ac.on();
      ac.setMode(kToshibaAcFan);
    } else if (AC_Mode == 7) {
      Serial.println("エアコン制御:OFF");
      ac.off();
    } else{
      Serial.println("なにもしない");
    }
    ac.setFan(AC_Fan);
    ac.setTemp(AC_Temp);
    ac.send();
    delay(1250);
    ac.send();
    delay(1250);
    fragment_reset("/Air_Con/Fragment");
  }
  
  //扇風機
  if (CF_flag == 1){
    uint64_t fan_key;
    //int fan_bit;
    if (CF_Mode == 0) {
      Serial.println("扇風機制御:OFF");
      fan_key = 0x41C4F807;
    } else if (CF_Mode == 1) {
      Serial.println("扇風機制御:ON");
      fan_key = 0x41C4F807;
    } else {
      Serial.println("扇風機制御:現状維持");
    }
    irsend.sendNEC(fan_key, 32);
    delay(1250);
    irsend.sendNEC(fan_key, 32);
    fragment_reset("/Cooling_Fan/Fragment");
  }
  //TV制御
  if (TV_Off == 1){
    Serial.println("テレビ制御:OFF");
    irsend.sendNEC(0x2FD48B7, 32);
    delay(1250);
    irsend.sendNEC(0x2FD48B7, 32);
    delay(1250);
    fragment_reset("/TV/Off");
  }
  
  //照明制御
  if (li_flag == 1){
    uint64_t li_key;
    if (li_Mode == 0) {
      Serial.println("照明制御:OFF");
      li_key = 0xCA0A68FC;
    } else if (li_Mode == 1) {
      Serial.println("照明制御:ON");
      li_key = 0x5DF2C18E;
    } else {
      Serial.println("照明制御:現状維持");
    }
    irsend.sendData(li_key, 42);
    delay(1250);
    irsend.sendData(li_key, 42);
    fragment_reset("/Light/Fragment");
  }

//フラグメントリセット関数
void fragment_reset(String Reset_flag){
  DynamicJsonDocument doc(64);
  doc["key"] = 0;
  String json;
  serializeJson(doc, json);
  HTTPClient http;
  http.begin(host_url + Reset_flag + auth_key);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.PUT(json);
  Serial.printf("status_code=%d\r\n", status_code);
  http.end();
}
