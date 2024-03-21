/**
   @file main.cpp
   @brief Example code for connecting to The Things Network.
*/

#include <TheThingsNetwork_IOT.h>

/** @brief The AppEUI for connecting to The Things Network. */
const char* AppEUI = "0000000000000000";

/** @brief The DevEUI for connecting to The Things Network.
    @note Replace "0004A30B001F2259" with your actual DevEUI.
*/
const char* DevEUI = "0004A30B001F2259";

/** @brief The AppKey for connecting to The Things Network. */
const char* AppKey = "C30F1715DFA6B9F21C2DCB07AC65FF00";

/** @brief Serial port for LoRa communication. */
#define loraSerial Serial1

/** @brief Serial port for debugging. */
#define debugSerial Serial

/** @brief TheThingsNetwork object for LoRa communication. */
TheThingsNetwork ttn(loraSerial, debugSerial);

/**
   @brief Arduino setup function.
          Initializes serial ports and joins The Things Network.
*/
void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);
  /**
     @brief Arduino setup function.
            Initialize LED output pin
  */
  pinMode(LED_BUILTIN, OUTPUT);
  /**
     @brief Arduino setup function.
          Wait a maximum of 10s for Serial Monitor
  */

  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(DevEUI, AppEUI, AppKey);
}

/**
   @brief Arduino main loop function.
*/
void loop() {}
