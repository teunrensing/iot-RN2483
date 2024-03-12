#include <TheThingsNetwork.h>

const char* AppEUI = "0000000000000000";
const char* DevEUI = "0000000000000000"; //DEFINE YOUR DEVEUI
const char* AppKey = "0000000000000000";


#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

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
  ttn.join(appEui, appKey);
}
