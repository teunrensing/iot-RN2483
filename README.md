# iot-RN2483

This repository contains documentation and a custom developed library for the RN2483 module. This library specially aims to address the code size. 
The library, named 'TheThingsNetwork_IOT', provides an interface for LoRaWAN communication with an RN2384 using The Things Network.
This adjusted library is based on the original code from https://github.com/TheThingsNetwork/arduino-device-lib.git

## Installation
1. Clone this repository to your Arduino libraries directory for full documentation, ór
2. For just using the library, copy the TheThingsNetwork_IOT library in your /Arduino/libraries folder.
3. Include the library in your sketch as showed below.

```arduino
#include <TheThingsNetwork_IOT.h>
```

## Usage
1. Make sure your device with LoRaWan compatibility is registered at the things network (https://www.thethingsindustries.com/docs/devices/adding-devices/).
2. Refer to the examples directory for sample sketches demonstrating various functionalities.
3. Initialize the RN2483 module with the desired configuration parameters described at the top of the example sketches.
4. Now compile and upload the code to the RN2483 to interact with the module and send/receive LoRa messages.


```arduino
#include <TheThingsNetwork_IOT.h>

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
