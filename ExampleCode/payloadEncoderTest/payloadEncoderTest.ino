/**
   @file main.cpp
   @brief Example code for sending sensor data using The Things Network and Cayenne LPP payload format.
*/

#include "CayenneLPP.hpp"
#include <TheThingsNetwork_IOT.h>
#include "SparkFun_Si7021_Breakout_Library.h" //Include for the temperature and humidity sensor

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

/** @brief Initialize Cayenne LPP payload encoder. */
PAYLOAD_ENCODER::CayenneLPP<51> lpp(50);

/** @brief Temperature and humidity sensor object. */
Weather sensor;

/** @brief TheThingsNetwork object for LoRa communication. */
TheThingsNetwork ttn(loraSerial, debugSerial);

/**
   @brief Arduino setup function.
          Initializes serial ports, LED pin, sensor, and joins The Things Network.
*/
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
  ttn.join(AppEUI, AppKey);

  sensor.begin(); // Initialize sensor
}

/**
   @brief Arduino main loop function.
          Reads sensor data, adds it to the Cayenne LPP payload, and sends it over LoRaWAN.
*/
void loop() {
  /** @brief  // Read sensor data*/
  float humidity = sensor.getRH();
  debugSerial.print(F("Humidity: "));
  debugSerial.print(humidity);
  debugSerial.println(F(" %RH."));

  /** @brief  // Measure Temperature from the Si7021 */
  float temperature = sensor.getTemp();
  debugSerial.print(F("Temperature: "));
  debugSerial.print(temperature);
  debugSerial.println(F(" Degrees."));

  /** @brief  // Add sensor data to Cayenne LPP payload*/
  lpp.reset();
  lpp.addTemperature(1, temperature); // Add temperature data (Channel 1)
  lpp.addHumidity(2, humidity); // Add humidity data (Channel 2)

  /** @brief  // Send Cayenne LPP payload over LoRaWAN*/
  ttn.sendBytes(lpp.getBuffer(), lpp.getSize());

  delay(60000); // Wait for some time before sending again (60 seconds in this case)
}
