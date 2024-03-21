/**
 * @file main.cpp
 * @brief Example code for using The Things Network library.
 */

#include <TheThingsNetwork_IOT.h>

#define LORA_SERIAL Serial1
#define DEBUG_SERIAL Serial
/**
 * @brief  Replace with your actual AppEUI and AppKey.
 */
const char* AppEUI = "0000000000000000";
const char* AppKey = "C30F1715DFA6B9F21C2DCB07AC65FF00";

char DevEUI[17]; /**< Buffer to store the device EUI. */

TheThingsNetwork ttn(LORA_SERIAL, DEBUG_SERIAL); /**< The Things Network instance. */

/**
 * @brief Setup function.
 */
void setup() {
  LORA_SERIAL.begin(57600);
  DEBUG_SERIAL.begin(9600);

  // Wait for the Serial Monitor to open
  while (!DEBUG_SERIAL && millis() < 10000)
    ;

  pinMode(LED_BUILTIN, OUTPUT);  /**< Initialize LED output pin*/

  // Display module status
  DEBUG_SERIAL.println("-- STATUS");
  ttn.showStatus();

  if (!extractDevEUI(ttn)) { /**< Extract DevEUI */
    DEBUG_SERIAL.println("Failed to extract DevEUI.");
    // Handle error
    while (true)
      ;
  }

  /**< Join the network */
  DEBUG_SERIAL.println("-- JOINING");
  ttn.join(DevEUI, AppEUI, AppKey);  /**< Use the extracted DevEUI*/
}

/**
 * @brief Loop function.
 */
void loop() {

  if (extractDevEUI(ttn)) {
    DEBUG_SERIAL.print("DevEUI: ");
    DEBUG_SERIAL.println(DevEUI);
  }
  delay(10000);
}

/**
 * @brief Extracts the DevEUI from the hardware.
 * 
 * @param ttn The Things Network instance.
 * @return true if the DevEUI is extracted successfully, false otherwise.
 */
bool extractDevEUI(TheThingsNetwork& ttn) {
  /**< The showStatus function outputs to debugSerial, wait for it to become available*/
  delay(1000);  /**< Wait for the status output to complete */

  while (DEBUG_SERIAL) {
    size_t size = ttn.getHardwareEui(DevEUI, sizeof(DevEUI));
    if (size > 0) {
      return true;
    }
  }
  return false;  /**< DevEUI not found or error occurred*/
}
