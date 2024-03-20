#include "CayenneLPP.hpp"
#include <TheThingsNetwork_IOT.h>
#include "SparkFun_Si7021_Breakout_Library.h" //Include for the temperature and humidity sensor

const char* AppEUI = "0000000000000000";
const char* DevEUI = "0004A30B001F2259"; //DEFINE YOUR DEVEUI
const char* AppKey = "C30F1715DFA6B9F21C2DCB07AC65FF00";

#define loraSerial Serial1
#define debugSerial Serial

// Initialize Cayenne LPP
PAYLOAD_ENCODER::CayenneLPP<51> lpp(50);

Weather sensor; //sensors

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
  ttn.join(AppEUI, AppKey);

  sensor.begin();
}

void loop() {
  // Read sensor data
  float humidity = sensor.getRH();
  debugSerial.print(F("Humidity: "));
  debugSerial.print(humidity);
  debugSerial.println(F(" %RH."));

  // Measure Temperature from the Si7021
  float temperature = sensor.getTemp();
  debugSerial.print(F("Temperature: "));
  debugSerial.print(temperature);
  debugSerial.println(F(" Degrees."));


  // Add sensor data to Cayenne LPP payload
  lpp.reset();
  lpp.addTemperature(1, temperature); // Add temperature data (Channel 1)
  lpp.addHumidity(2, humidity); // Add humidity data (Channel 2)

  // Send Cayenne LPP payload over LoRaWAN
  ttn.sendBytes(lpp.getBuffer(), lpp.getSize());

  delay(60000); // Wait for some time before sending again (60 seconds in this case)
}
