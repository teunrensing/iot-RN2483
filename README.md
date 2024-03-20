# iot-RN2483

This repository contains a custom developed library for the RN2483 module, This library specially aims to address the code size

## Installation
1. Clone this repository to your Arduino libraries directory
2. Include the library in your sketch

```arduino
#include <TheThingsNetwork.h>
```

## Usage
1. Refer to the examples directory for sample sketches demonstrating various functionalities.
2. Initialize the RN2483 module with the desired configuration parameters.
3. Use the provided sketches to interact with the module and send/receive LoRa messages.


```arduino
#include "TheThingsNetwork_IOT.h"

#define LORA_SERIAL Serial1
#define DEBUG_SERIAL Serial

// Replace with your actual AppEUI and AppKey
const char* AppEUI = "0000000000000000";
const char* AppKey = "00000000000000000000000000000000";

char DevEUI[17];

TheThingsNetwork ttn(LORA_SERIAL, DEBUG_SERIAL);

void setup() {
  LORA_SERIAL.begin(57600);
  DEBUG_SERIAL.begin(9600);

  // Wait for the Serial Monitor to open
  while (!DEBUG_SERIAL && millis() < 10000)
    ;

  pinMode(LED_BUILTIN, OUTPUT);  // Initialize LED output pin

  // Display module status
  DEBUG_SERIAL.println("-- STATUS");
  ttn.showStatus();

  // Extract DevEUI
  if (!extractDevEUI(ttn)) {
    DEBUG_SERIAL.println("Failed to extract DevEUI.");
    // Handle error
    while (true)
      ;
  }

  // Join the network
  DEBUG_SERIAL.println("-- JOINING");
  ttn.join(DevEUI, AppEUI, AppKey);  // Use the extracted DevEUI
}

void loop() {

  if (extractDevEUI(ttn)) {
    DEBUG_SERIAL.print("DevEUI: ");
    DEBUG_SERIAL.println(DevEUI);
  }
  delay(10000);
}

bool extractDevEUI(TheThingsNetwork& ttn) {
  // The showStatus function outputs to debugSerial, wait for it to become available
  delay(1000);  // Wait for the status output to complete

  while (DEBUG_SERIAL) {
    size_t size = ttn.getHardwareEui(DevEUI, sizeof(DevEUI));
    if (size > 0) {
      return true;
    }
  }
  return false;  // DevEUI not found or error occurred
}
