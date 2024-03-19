#include <TheThingsNetwork.h>

#define freqPlan TTN_FP_EU868
#define LORA_SERIAL Serial1
#define DEBUG_SERIAL Serial

// Replace with your actual AppEUI and AppKey
const char* AppEUI = "0000000000000000";
const char* AppKey = "0000000000000000";

char DevEUI[17];

TheThingsNetwork ttn(LORA_SERIAL, DEBUG_SERIAL, freqPlan);

void setup() {
  LORA_SERIAL.begin(57600);
  DEBUG_SERIAL.begin(9600);

  // Wait for the Serial Monitor to open
  while (!DEBUG_SERIAL && millis() < 10000);

  pinMode(LED_BUILTIN, OUTPUT); // Initialize LED output pin

  // Display module status
  DEBUG_SERIAL.println("-- STATUS");
  ttn.showStatus();

  // Extract DevEUI
  if (!extractDevEUI()) {
    DEBUG_SERIAL.println("Failed to extract DevEUI.");
    // Handle error
    while (true);
  }

  DEBUG_SERIAL.print("DevEUI: ");
  DEBUG_SERIAL.println(DevEUI);

  // Join the network
  DEBUG_SERIAL.println("-- JOIN");
  ttn.join(AppEUI, AppKey, DevEUI); // Use the extracted DevEUI
}

void loop() {
  // Example of sending a payload
  byte payload[] = {0x01, 0x02, 0x03, 0x04}; // Example payload
  ttn.sendBytes(payload, sizeof(payload)); // Send the payload
}

bool extractDevEUI() {
  // The showStatus function outputs to debugSerial, wait for it to become available
  delay(1000); // Wait for the status output to complete

  while (DEBUG_SERIAL.available()) {
    String line = DEBUG_SERIAL.readStringUntil('\n');
    if (line.startsWith("DevEUI: ")) {
      // Extract the DevEUI
      String eui = line.substring(8); // Remove "DevEUI: " prefix
      eui.toCharArray(DevEUI, sizeof(DevEUI));
      return true;
    }
  }
  return false; // DevEUI not found or error occurred
}
