/*--------------------------------------------------------------------
  This code is free software:
  you can redistribute it and/or modify it under the terms of a Creative
  Commons Attribution-NonCommercial 4.0 International License
  (http://creativecommons.org/licenses/by-nc/4.0/) by
  Remko Welling (https://ese.han.nl/~rwelling/) E-mail: remko.welling@han.nl

  The program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  --------------------------------------------------------------------*/

/*!
 * \file LoRa_TX_RX_Cayenne_HAN_Personalisation_001.ino
 * \brief Sends packets on TTN using the LISSLoRa gadget.
 * Based on the original code from: https://github.com/YourproductSmarter/KISSLoRa-demo
 * Thi sversion is using ABP pesonalisation
 * \author Remko Welling (remko.welling@han.nl)
 * \date 15-1-2019
 * \version 2
 * 
 * # Revision history
 * 
 * Version|Date      |Note
 * --------------------------------------
 * 1      |nov 2018  | Initial release
 * 2      |11-2-2019 | Update to the level of LoRa_TX_RX_Cayenne_HAN_xxx
 * 
 */
#define RELEASE   2
/*!

## To use this board:
Install the USB drivers for the SparkFun boards, following the steps [for Windows](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-windows) or [for Linux and Mac](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-mac--linux).
In the Arduino IDE go to **File->Preferences->Additional Boards Manager URLs** and add: `https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json`
Go to **Tools->Board->Boards Manager**, search for **SparkFun AVR Boards** and click on **install**.
Go to **Tools->Board** and select **SparkFun Pro Micro**.
Go to **Tools->Processor** and select **ATmega32U4 (3.3V, 8MHz)**.

## Uploading Firmware
> The serial monitor must be closed before uploading code to the device.

1. Switch the KISS LoRa device off.
2. Plug it into your computer using a micro USB cable.
3. In the Arduino IDE make sure the correct **Tools->Board** (SparkFun Pro Micro) and **Tools->Processor** (ATmega 32U4, 8MHz) are selected.
4. Switch the device on and make sure the correct **Tools->Port** is selected.
5. Switch the device off again.
6. Press and hold the **Push Button** on the kiss device.
6. In the Arduino IDE, choose **Sketch->Upload**.
7. As soon as you see `PORTS {} / {} => {}` printed in the output window turn KISSLoRa on.
8. When the upload process continue past showing `PORTS {} / {} => {}`, you can release the push button.


 */

// Libraries
#include <TheThingsNetwork.h>
#include <CayenneLPP.h>         // include for Cayenne library
#include "SparkFun_Si7021_Breakout_Library.h" // include for temperature and humidity sensor
#include <Wire.h>
#include "KISSLoRa_sleep.h"     // Include to sleep MCU

// defines Serial 
#define loraSerial  Serial1
#define debugSerial Serial

// LoRaWAN TTN

// The KISS device should only be used in Europe
#define freqPlan TTN_FP_EU868

// Select personalisation type, uncomment selected option
//#define OTAA
#define ABP

#if defined(OTAA)
  // Set your AppEUI and AppKey
  // HAN KISS-xx: devEui is device specific
  const char *appEui = "0000000000000000";
  const char *appKey = "00000000000000000000000000000000";
#elif defined(ABP)
  const char *devAddr = "00000000";
  const char *nwkSKey = "0000000000000000";
  const char *appSKey = "00000000000000000000000000000000";
#else
  #error "No device configured."
#endif

// Enable single channel operation, uncomment define to enable
//#define SINGLE_CHANNEL

// Define SF to override default (0)
//#define SF 0  ///< default SF (7)
#define SF 9

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);  // TTN object for LoRaWAN radio

// Cayennel LPP
#define APPLICATION_PORT_CAYENNE  99  ///< LoRaWAN port to which CayenneLPP packets shall be sent
#define LPP_PAYLOAD_MAX_SIZE      51  ///< Maximum payload size of a LoRaWAN packet

#define LPP_CH_TEMPERATURE        0    ///< CayenneLPP CHannel for Temperature
#define LPP_CH_HUMIDITY           1    ///< CayenneLPP CHannel for Humidity sensor
#define LPP_CH_LUMINOSITY         2    ///< CayenneLPP CHannel for Luminosity sensor
#define LPP_CH_ROTARYSWITCH       3    ///< CayenneLPP CHannel for Rotary switch
#define LPP_CH_ACCELEROMETER      4    ///< CayenneLPP CHannel for Accelerometer
#define LPP_CH_BOARDVCCVOLTAGE    5    ///< CayenneLPP CHannel for Processor voltage
#define LPP_CH_PRESENCE           6    ///< CayenneLPP CHannel for Alarm
#define LPP_CH_SET_INTERVAL       20   ///< CayenneLPP CHannel for setting downlink interval
#define LPP_CH_SW_RELEASE         90   ///< 

#define ALARM             0x01  ///< Alarm state
#define SAFE              0x00  ///< No-alarm state

CayenneLPP lpp(LPP_PAYLOAD_MAX_SIZE);  ///< Cayenne object for composing sensor message
uint8_t lppChannel { 0 } ;             ///< channel iterator

// Sensors
Weather sensor;     // temperature and humidity sensor

#define LIGHT_SENSOR_PIN 10 // Define for Analog inpu pin

// defines for LEDs
#define RGBLED_RED    12
#define RGBLED_GREEN  6
#define RGBLED_BLUE   11
#define LED_LORA      8

// defines for rotary encoder
#define ROTARY_PIN_0  5
#define ROTARY_PIN_1  13
#define ROTARY_PIN_2  9
#define ROTARY_PIN_3  30

// defines for pushbutton
#define BUTTON_PIN    7

// defines for accelerometer
// Set up to read the accelerometer values in range -2g to +2g - valid ranges: ±2G,±4G or ±8G
#define ACC_RANGE     2

float x,y,z;                      ///< Variables to hold acellerometer axis values.

void setup()
{
  // Initlialize serial
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Initialize LED outputs
  pinMode(RGBLED_RED,   OUTPUT);
  pinMode(RGBLED_GREEN, OUTPUT);
  pinMode(RGBLED_BLUE,  OUTPUT);
  pinMode(LED_LORA,     OUTPUT);

  // initialize rotary encoder, Set pins as inputs
  pinMode(ROTARY_PIN_0, INPUT);
  pinMode(ROTARY_PIN_1, INPUT);
  pinMode(ROTARY_PIN_2, INPUT);
  pinMode(ROTARY_PIN_3, INPUT);
  
  //Disable pullup resistors
  digitalWrite(ROTARY_PIN_0, 0);
  digitalWrite(ROTARY_PIN_1, 0);
  digitalWrite(ROTARY_PIN_2, 0);
  digitalWrite(ROTARY_PIN_3, 0);

//  // configure button
//  pinMode(BUTTON_PIN, INPUT);     //Set pin as inputs
//  digitalWrite(BUTTON_PIN, 0);    //Disable pullup resistors
//  //Attach an interrupt to the button pin - fire when button pressed down.
//  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressedISR, FALLING);

  //Initialize the I2C Si7021 sensor
  sensor.begin();

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  // Switch off leds
  digitalWrite(RGBLED_RED,    HIGH);  //switch RGBLED_RED LED off
  digitalWrite(RGBLED_GREEN,  HIGH);  //switch RGBLED_GREEN LED off
  digitalWrite(RGBLED_BLUE,   HIGH);  //switch RGBLED_BLUE LED off
  digitalWrite(LED_LORA,      HIGH);  //switch LED_LORA LED off

  Wire.begin();
  initAccelerometer();
  setAccelerometerRange(ACC_RANGE); 

  // Initialize LoRaWAN radio
  ttn.onMessage(message); // Set callback for incoming messages
  ttn.reset(true);  // Reset LoRaWAN mac and enable ADR
  
  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");

  #if defined(OTAA)
    ttn.join(appEui, appKey);
  #elif defined(ABP)
    ttn.personalize(devAddr, nwkSKey, appSKey);
    #if defined(SINGLE_CHANNEL)
      setSingleChannel();
    #endif
  #endif
}

void loop()
{
  debugSerial.println("-- LOOP");

  // Measure Relative Humidity from the Si7021
  float humidity = sensor.getRH();
  debugSerial.print("Humidity: ");
  debugSerial.print(humidity);
  debugSerial.println(" %RH.");

  // Measure Temperature from the Si7021
  float temperature = sensor.getTemp();
  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement with getTemp() instead with readTemp()
  debugSerial.print("Temperature: ");
  debugSerial.print(temperature);
  debugSerial.println(" Degrees.");

  // Measure luminosity
  float luminosity = get_lux_value();
  Serial.print("Ambient light: ");
  Serial.print(luminosity);
  Serial.println(" lux");

  // get rotary encode position
  uint8_t rotaryPosition = (uint8_t)getRotaryPosition();
  Serial.print("Rotary encoder position: ");
  Serial.println(rotaryPosition);

  /// get accelerometer
  getAcceleration(&x, &y, &z);
  Serial.print("Acceleration:\tx=");
  Serial.print(x);
  Serial.print("g\n\t\ty=");
  Serial.print(y);
  Serial.print("g\n\t\tz=");
  Serial.print(z);
  Serial.println("g");
  Serial.println("---");

  float vdd = (float)ttn.getVDD()/1000;
  Serial.print("RN2483 voltage: ");
  Serial.print(vdd);
  Serial.println(" Volt");
  
  // Compose Cayenne message
  lpp.reset();    // reset cayenne object
  lppChannel = 0; // reset channel counter

  // add sensor values to cayenne data package
  lpp.addTemperature(LPP_CH_TEMPERATURE, temperature);
  lpp.addRelativeHumidity(LPP_CH_HUMIDITY, humidity);
  lpp.addLuminosity(LPP_CH_LUMINOSITY, luminosity);
  lpp.addDigitalInput(LPP_CH_ROTARYSWITCH, rotaryPosition);
  lpp.addAccelerometer(LPP_CH_ACCELEROMETER, x, y, z);
  lpp.addAnalogInput(LPP_CH_BOARDVCCVOLTAGE, vdd);
//  lpp.addPresence(LPP_CH_PRESENCE, SAFE);
//  lpp.addAnalogOutput(LPP_CH_SET_INTERVAL, (float)currentInterval/1000);
  
  digitalWrite(LED_LORA, LOW);  //switch LED_LORA LED on

  // send cayenne message on port 99
  ttn.sendBytes(lpp.getBuffer(), lpp.getSize(), APPLICATION_PORT_CAYENNE, false, SF);

//  // Send single byte to poll for incoming messages, using port 1
//  ttn.poll(1, false);

  digitalWrite(LED_LORA, HIGH);  //switch LED_LORA LED off

  delay(60000);
}

void message(const uint8_t *payload, size_t size, port_t port)
{
  debugSerial.println("-- MESSAGE");
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ":");

  for (int i = 0; i < size; i++)
  {
    debugSerial.print(" " + String(payload[i]));
  }

  debugSerial.println();

  //Toggle red LED when a message is received
  pinMode(RGBLED_RED, OUTPUT);
  digitalWrite(RGBLED_RED, !digitalRead(RGBLED_RED));
}

//Get the lux value from the APDS-9007 Ambient Light Photo Sensor
float get_lux_value()
{
  int digital_value = analogRead(LIGHT_SENSOR_PIN);
  double vlux = digital_value * (2.56/1023.0); //lux value in volts
  double ilux = (vlux / 56) * 1000;            //lux value in micro amperes
  double lux = pow(10, (ilux / 10));           //Convert ilux to Lux value
  return (float)lux;                        //Return Lux value as value without decimal
}

//Poll the rotary switch
int8_t getRotaryPosition()
{
  int8_t value = 0;
  if (digitalRead(ROTARY_PIN_0)) {value |= 1 << 0;}
  if (digitalRead(ROTARY_PIN_1)) {value |= 1 << 1;}
  if (digitalRead(ROTARY_PIN_2)) {value |= 1 << 2;}
  if (digitalRead(ROTARY_PIN_3)) {value |= 1 << 3;}
  
  //Set value to -1 if it's higher than 9 - this should never happen but just in case
  if (value > 9)
  {
    value = -1;
  }

  return value;
}

void buttonPressedISR()
{
  //Toggle LED when button is pressed
  //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

//Write one register to the acceleromter
void writeAccelerometer(unsigned char REG_ADDRESS, unsigned char DATA)
{
  Wire.beginTransmission(0x1D);
  Wire.write(REG_ADDRESS);
  Wire.write(DATA);
  Wire.endTransmission();
}

//Read one register from the accelerometer
uint8_t readAccelerometer(unsigned char REG_ADDRESS)
{
  uint8_t resp;
  Wire.beginTransmission(0x1D);
  Wire.write(REG_ADDRESS);
  Wire.endTransmission(false);
  Wire.requestFrom(0x1D, 1);
  resp = Wire.read();
  return resp;
}

//Configure and activate the FXLS8471Q Accelerometer 
static void initAccelerometer(void)
{
  //Check if the chip responds to the who-am-i command, should return 0x6A (106)
  if (readAccelerometer(0x0D) == 106)
  {
    //Configure FXLS8471Q CTRL_REG1 register
    //Set f_read bit to activate fast read mode
    //Set active bit to put accelerometer in active mode
    writeAccelerometer(0x2A, 0x03);  
  }
  else
  {
    Serial.println(F("--- I2C Accelerometer not initialized"));
  } 
}

//Set the range of the FXLS8471Q Accelerometer, valid ranges: 2G,4G or 8G
static void setAccelerometerRange(uint8_t range_g)
{
  uint8_t range_b;
  switch(range_g)
  {
    case 2:
      range_b = 0;
    break;
    case 4:
      range_b = 1;
    break;
    case 8:
      range_b = 2;
    break;
    default:
      range_b = 0;
    break;
  }
  writeAccelerometer(0x0E, range_b);
}

//Read the acceleration from the accelerometer
void getAcceleration(float *x, float *y, float *z)
{
  // Resource: https://github.com/sparkfun/MMA8452_Accelerometer/blob/master/Libraries/Arduino/src/SparkFun_MMA8452Q.cpp
  // Read the acceleration from registers 1 through 6 of the MMA8452 accelerometer.
  // 2 registers per axis, 12 bits per axis.
  // Bit-shifting right does sign extension to preserve negative numbers.
  *x = ((short)(readAccelerometer(1)<<8 | readAccelerometer(2))) >> 4;
  *y = ((short)(readAccelerometer(3)<<8 | readAccelerometer(4))) >> 4;
  *z = ((short)(readAccelerometer(5)<<8 | readAccelerometer(6))) >> 4;

  // Scale 12 bit signed values to units of g. The default measurement range is ±2g.
  // That is 11 bits for positive values and 11 bits for negative values.
  // value = (value / (2^11)) * 2
  *x = (float)*x / (float)(1<<11) * (float)(ACC_RANGE);
  *y = (float)*y / (float)(1<<11) * (float)(ACC_RANGE);
  *z = (float)*z / (float)(1<<11) * (float)(ACC_RANGE);
}

//  Disable 7 of the 8 default channels. This basically disables the
//  mandatory LoRaWAN channel hopping, so FOR TESTING ONLY!!
//  Use: sendChSet(MAC_CHANNEL_STATUS, ch, "on");
//  like: exec("mac set ch status 0 off");
//  This is a bruteforce override that shall be optimized.
void setSingleChannel(void)
{
  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 1 off");
  String ret = loraSerial.readStringUntil('\n');

  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 2 off");
  ret = loraSerial.readStringUntil('\n');

  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 3 off");
  ret = loraSerial.readStringUntil('\n');

  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 4 off");
  ret = loraSerial.readStringUntil('\n');

  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 5 off");
  ret = loraSerial.readStringUntil('\n');

  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 6 off");
  ret = loraSerial.readStringUntil('\n');

  delay(100);
  while(loraSerial.available())
  {
    loraSerial.read();
  }
  loraSerial.println("mac set ch status 7 off");
  ret = loraSerial.readStringUntil('\n');

//  ret.trim();
}
