#include <ir_Toshiba.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Toshiba.h>

const uint16_t kIrLed = 9;
IRToshibaAC ac(kIrLed);

void printState() {
  // Display the settings.
  Serial.println("Toshiba A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  // Display the encoded IR sequence.
  unsigned char* ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < ac.getStateLength(); i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}

void setup() {
  ac.begin();
  Serial.begin(115200);
  delay(200);

  Serial.println("Default state of the remote.");
  printState();
  Serial.println("Setting desired state for A/C.");
  ac.off();
  //ac.setFan(1);
  //ac.setMode(kToshibaAcCool);
  //ac.setTemp(26);
}

void loop() {
  // Now send the IR signal.
#if SEND_TOSHIBA_AC
  Serial.println("Sending IR command to A/C ...");
  ac.send();
#endif
  printState();
  delay(5000);
}
