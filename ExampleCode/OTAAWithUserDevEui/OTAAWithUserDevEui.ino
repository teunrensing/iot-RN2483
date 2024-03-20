#include "TheThingsNetwork_IOT.h"

const char* AppEUI = "0000000000000000";
const char* DevEUI = "0004A30B001F2259"; //DEFINE YOUR DEVEUI
const char* AppKey = "C30F1715DFA6B9F21C2DCB07AC65FF00";


#define loraSerial Serial1
#define debugSerial Serial

TheThingsNetwork ttn(loraSerial, debugSerial);

void setup() {
   loraSerial.begin(57600);
 debugSerial.begin(9600);

  // Initialize LED output pin
  pinMode(LED_BUILTIN, OUTPUT);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(DevEUI, AppEUI, AppKey);
}

void loop(){}
