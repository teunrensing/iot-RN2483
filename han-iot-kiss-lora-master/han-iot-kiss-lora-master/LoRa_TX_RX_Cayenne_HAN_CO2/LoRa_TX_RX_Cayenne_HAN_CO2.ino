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
 * \file LoRa_TX_RX_Cayenne_HAN.ino
 * \brief Sends packets on TTN using the LISSLoRa gadget.
 * Based on the original code from: https://github.com/YourproductSmarter/KISSLoRa-demo
 * \author Remko Welling (remko.welling@han.nl)
 * 
 * # Revision history
 * 
 * Version|Date      |Note
 * --------------------------------------
 * 1.0    |nov 2018  | Initial release
 * 1.1    |21-12-2018| Various additions
 * 1.2    | 5- 1-2019| added sleepmode for AVR and RN module, added documentation,
 * 1.3    |15- 1-2019| Changed addAnalogOutput to send right interval to cayenne,
 * 1.3.1  |27-03-2019| Corrected display of downlink setting of interval in serial communication
 * 4      |27-03-2019| Added functionality to set initial interval using the rotary encoder.
 * 5.1    |5-8-2020  | replaced si7021 sensor for SCD30 to add CO2 sensor
 * 5.2    |9-9-2020  | Added RGB indicator for CO2, intermediate read of rotary encoder to set interval, removed unused code, corrected RGB display timing
 * 5.3    |18-9-2022 | Added links to required libraries.
 * 
 */
#define RELEASE   53
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
7. In the Arduino IDE, choose **Sketch->Upload**.
8. As soon as you see `PORTS {} / {} => {}` printed in the output window turn KISSLoRa on.
9. When the upload process continue past showing `PORTS {} / {} => {}`, you can release the push button.

 */

// Libraries and includes
#include <TheThingsNetwork.h>
#include <CayenneLPP.h>         // include for Cayenne library
#include <Wire.h>
#include "KISSLoRa_sleep.h"     // Include to sleep MCU
#include <SoftPWM.h>            // Include SpoftPWM of Brett Hagman. Click here to get the libary: http://librarymanager/All#SoftPWM
#include <SparkFun_SCD30_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_SCD30

#define USB_CABLE_CONNECTED (USBSTA&(1<<VBUS))

// defines Serial 
#define loraSerial Serial1
#define debugSerial Serial

// LoRaWAN TTN
#define freqPlan TTN_FP_EU868     ///< The KISS device should only be used in Europe

// Set your AppEUI and AppKey
// HAN KISS-xx: devEui is device specific
const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);  // TTN object for LoRaWAN radio
//TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan, 9);  // TTN object for LoRaWAN radio using SF9

// Cayennel LPP
#define APPLICATION_PORT_CAYENNE  99   ///< LoRaWAN port to which CayenneLPP packets shall be sent
#define LPP_PAYLOAD_MAX_SIZE      51   ///< Maximum payload size of a LoRaWAN packet

#define LPP_CH_TEMPERATURE        0    ///< CayenneLPP CHannel for Temperature
#define LPP_CH_HUMIDITY           1    ///< CayenneLPP CHannel for Humidity sensor
#define LPP_CH_LUMINOSITY         2    ///< CayenneLPP CHannel for Luminosity sensor
#define LPP_CH_ROTARYSWITCH       3    ///< CayenneLPP CHannel for Rotary switch
#define LPP_CH_BOARDVCCVOLTAGE    5    ///< CayenneLPP CHannel for Processor voltage
#define LPP_CH_PRESENCE           6    ///< CayenneLPP CHannel for Alarm
#define LPP_CH_SET_INTERVAL       20   ///< CayenneLPP CHannel for setting downlink interval
#define LPP_CH_GAS_CO2            40   ///< CayenneLPP CHannel for CO2 using luminosity in CayenneLPP
#define LPP_CH_SW_RELEASE         90   ///< 

#define ALARM                     0x01 ///< Alarm state
#define SAFE                      0x00 ///< No-alarm state

CayenneLPP lpp(LPP_PAYLOAD_MAX_SIZE);  ///< Cayenne object for composing sensor message

// Sensors

SCD30 airSensor;                 ///< temperature and humidity sensor

// defines for hardware.
#define LIGHT_SENSOR_PIN  10     ///< Define for Analog input pin
#define LUMINOSITY_HIGH   1000   ///< Highest Lux value for dimmer of RGB LEDs

// defines for LEDs
#define RGBLED_RED        12
#define RGBLED_GREEN      6
#define RGBLED_BLUE       11
#define LED_LORA          8

// defines for rotary encoder
#define ROTARY_PIN_0      5
#define ROTARY_PIN_1      13
#define ROTARY_PIN_2      9
#define ROTARY_PIN_3      0

// defines for pushbutton
#define BUTTON_PIN        7

// Set up application specific
#define REGULAR_INTERVAL  60000   ///< Regular transmission interval in ms

#define INTERVAL_ROTARY_MASK 0x07 ///< Binary mask: 0000 0111
#define INTERVAL_ROTARY_1 1       ///< 1 minute interval
#define INTERVAL_ROTARY_2 2       ///< 5 minutes interval
#define INTERVAL_ROTARY_3 3       ///< 15 minutes interval
#define INTERVAL_ROTARY_4 4       ///< 60 minutes interval

#define INTERVAL_1        60000   ///< 1 minute interval
#define INTERVAL_2        300000  ///< 5 minutes interval
#define INTERVAL_3        900000  ///< 15 minutes interval
#define INTERVAL_4        3600000 ///< 60 minutes interval

uint32_t currentInterval = REGULAR_INTERVAL;
uint32_t nextInterval    = REGULAR_INTERVAL;

/// \brief struct that defines all colors
const uint8_t color[8][3] = {
   {255, 255, 255},  // White base
   {0,   255, 0  },  // green
   {255, 255, 0  },  // yellow
   {0,   255, 128},  // light blue
   {0,   0,   255},  // blue
   {128, 0,   255},  // Purple
   {255, 0,   128},  // bright purple
   {255, 0 ,  0  }   // red
};

float luminosity = {0.0};         ///< Global variable that holds luminosity measured.
const uint16_t   gasRange[] = {0,351,451,701,1001,2501,2503,5001}; ///< CO2 range steps

#define BAR_DISPLAY_DURATION 2000

bool alarm = { false };           ///< Variable to hold alarm state when set in ISR from button.

// \brief setup
void setup(){
  KISSLoRa_sleep_init();
  
  // Initialize LED outputs
  pinMode(RGBLED_RED,   OUTPUT);
  pinMode(RGBLED_GREEN, OUTPUT);
  pinMode(RGBLED_BLUE,  OUTPUT);
  pinMode(LED_LORA,     OUTPUT);

  SoftPWMBegin();

  //init red and green LED's as SoftPWM
  SoftPWMSetPolarity(RGBLED_RED,SOFTPWM_INVERTED);
  SoftPWMSetFadeTime(RGBLED_RED, 0, 1);
  SoftPWMSetPolarity(RGBLED_GREEN,SOFTPWM_INVERTED);
  SoftPWMSetFadeTime(RGBLED_GREEN, 1, 1);
  set_rgb_led(255,255,255);
  
  // Initlialize serial
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // initialize rotary encoder
  //Set pins as inputs
  pinMode(ROTARY_PIN_0, INPUT);
  pinMode(ROTARY_PIN_1, INPUT);
  pinMode(ROTARY_PIN_2, INPUT);
  pinMode(ROTARY_PIN_3, INPUT);

  //Disable pullup resistors
  digitalWrite(ROTARY_PIN_0, 0);
  digitalWrite(ROTARY_PIN_1, 0);
  digitalWrite(ROTARY_PIN_2, 0);
  digitalWrite(ROTARY_PIN_3, 0);

  // configure button
  pinMode(BUTTON_PIN, INPUT);     //Set pin as inputs
  digitalWrite(BUTTON_PIN, 0);    //Disable pullup resistors
  //Attach an interrupt to the button pin - fire when button pressed down.
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressedISR, FALLING);

  //Initialize the SCD30 air sensor
  if (airSensor.begin() == false){
    debugSerial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  // Switch off leds
  set_rgb_led(0,128,0);
  digitalWrite(LED_LORA,     HIGH);  //switch LED_LORA LED off

  Wire.begin();

  // Initialize LoRaWAN radio
  set_rgb_led(255,0,0);    //switch RGBLED_RED LED on
    
  ttn.onMessage(message);           // Set callback for incoming messages
  ttn.reset(true);                  // Reset LoRaWAN mac and enable ADR
  
  debugSerial.println(F("-- STATUS"));
  ttn.showStatus();

  debugSerial.println(F("-- JOIN"));
  ttn.join(appEui, appKey);

  // initilize interval from rotary switch
  nextInterval = getInitialInterval((uint8_t)getRotaryPosition());

  set_rgb_led(0,0,0);   //switch RGBLED_RED LED off when join succeeds
}

// \brief mainloop
void loop(){
  debugSerial.println(F("-- LOOP"));
  
  nextInterval = getInitialInterval((uint8_t)getRotaryPosition());
  if(currentInterval != nextInterval){
    debugSerial.print("Next interval set to: " + String(nextInterval/1000));
    debugSerial.println(F(" Seconds"));    
  }
 
  airSensor.setMeasurementInterval(nextInterval/10000);    // Update SCD30 to new continous measurement interval 
  currentInterval = nextInterval;
  
  set_rgb_led(0,0,0);  //switch RGBLED LED off

  float humidity { 50 };
  float temperature { 20 };
  float gas { 0.0 }; ///< Variable to hold CO2 value
  getSensorValues(temperature, humidity, gas, currentInterval);
  
  debugSerial.print(F("Temperature: "));
  debugSerial.print(temperature);
  debugSerial.println(F(" Degrees."));

  debugSerial.print(F("Humidity: "));
  debugSerial.print(humidity);
  debugSerial.println(F(" %RH."));

  // Measure luminosity
  luminosity = get_lux_value();
  Serial.print(F("Ambient light: "));
  Serial.print(luminosity);
  Serial.println(F(" lux"));
  
  debugSerial.print(F("CO2: "));
  debugSerial.print(gas);
  debugSerial.println(F(" ppm."));

  // get rotary encode position
  uint8_t rotaryPosition = (uint8_t)getRotaryPosition();
  Serial.print(F("Rotary encoder position: "));
  Serial.println(rotaryPosition);

//  /// get VDD from RN module
//  float vdd = (float)ttn.getVDD()/1000;
//  Serial.print(F("RN2483 voltage: "));
//  Serial.print(vdd);
//  Serial.println(F(" Volt"));
  
  // Wake RN2483 
  ttn.wake();
  
  // Compose Cayenne message
  lpp.reset();    // reset cayenne object

  // add sensor values to cayenne data package
  lpp.addTemperature(LPP_CH_TEMPERATURE, temperature);
  lpp.addRelativeHumidity(LPP_CH_HUMIDITY, humidity);
  lpp.addLuminosity(LPP_CH_LUMINOSITY, luminosity);
  lpp.addDigitalInput(LPP_CH_ROTARYSWITCH, rotaryPosition);
  lpp.addLuminosity(LPP_CH_GAS_CO2, gas);
  lpp.addPresence(LPP_CH_PRESENCE, SAFE);
  lpp.addAnalogOutput(LPP_CH_SET_INTERVAL, (float)currentInterval/1000);
  
  digitalWrite(LED_LORA, LOW);  //switch LED_LORA LED on

  // send cayenne message on port 99
  ttn.sendBytes(lpp.getBuffer(), lpp.getSize(), APPLICATION_PORT_CAYENNE);

  // Set RN2483 to sleep mode
  ttn.sleep(currentInterval - 100);
  // This delay is not optional, try to remove it
  // and say bye bye to your RN2483 sleep mode
  delay(50);

  digitalWrite(LED_LORA, HIGH);  //switch LED_LORA LED off
  
  // Set KISSLoRa to idle.
  drawRGBMeter(currentInterval, gas, gasRange);
}

/// \brief function called at RX message
/// \param payload pointer to received payload
/// \param size payload size
/// \param port Application port
void message(const uint8_t *payload, size_t size, port_t port){
  debugSerial.println(F("-- MESSAGE"));
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ": ");

  switch(port){
    case 99:
      if(payload[0] == 0x14){
        uint32_t tempValue = 0;
        tempValue |= payload[1] << 8;
        tempValue |= payload[2];
        nextInterval = tempValue * 10;
        debugSerial.print("New interval: " + String(nextInterval/1000));
        debugSerial.println(F(" Seconds"));
        set_rgb_led(0,0,128);
      }else{
        debugSerial.println(F("Wrong downlink message."));
      }
      break;
    default:
      {
        for (int i = 0; i < size; i++){
          debugSerial.print(" " + String(payload[i]));
        }
        debugSerial.println();
      }
      //Toggle green LED when a message is received
      set_rgb_led(0,128,0);
      break;
  }
}

/// \brief read luminosty value from sensor
///  Get the lux value from the APDS-9007 Ambient Light Photo Sensor
/// \return luminosity in Lux.
float get_lux_value(void){
  int digital_value = analogRead(LIGHT_SENSOR_PIN);
  double vlux = digital_value * (2.56/1023.0); //lux value in volts
  double ilux = (vlux / 56) * 1000;            //lux value in micro amperes
  double lux = pow(10, (ilux / 10));           //Convert ilux to Lux value
  return (float)lux;                        //Return Lux value as value without decimal
}

/// \brief read rotary switch value
///  Poll the rotary switch
/// \retval binary representation of rotarty switch position ( 0 to 9)
int8_t getRotaryPosition(){
  int8_t value = 0;
  if (digitalRead(ROTARY_PIN_0)) {value |= 1 << 0;}
  if (digitalRead(ROTARY_PIN_1)) {value |= 1 << 1;}
  if (digitalRead(ROTARY_PIN_2)) {value |= 1 << 2;}
  if (digitalRead(ROTARY_PIN_3)) {value |= 1 << 3;}
  return value;
}

/// \brief function called at interrupt generated by pushbutton
void buttonPressedISR()
{
  alarm = true;
}

/// \brief Determine interval in ms using rotary value
/// \pre This function is only using bits 4, 2, and 1 while ignoring bit 8.
/// Set this using define INTERVAL_ROTARY_MASK
/// \param rotaryValue actual value from rotary encoder.
/// \return interval in ms
uint32_t getInitialInterval(uint8_t rotaryValue){
  uint32_t intervalMs = REGULAR_INTERVAL;
  
  rotaryValue &= INTERVAL_ROTARY_MASK;
  switch(rotaryValue){
    case INTERVAL_ROTARY_1:
      intervalMs = INTERVAL_1;
      break;

    case INTERVAL_ROTARY_2:
      intervalMs = INTERVAL_2;
      break;

    case INTERVAL_ROTARY_3:
      intervalMs = INTERVAL_3;
      break;

    case INTERVAL_ROTARY_4:
      intervalMs = INTERVAL_4;
      break;

    default:
      intervalMs = REGULAR_INTERVAL;
      break;
  }
  return intervalMs;
}

/// \brief Read all sensors from SCD30
/// Based on the interval set the number of attempts to read the sensor is calculated.
/// \param temp Address of variabele where temperature measurement result shall be stored.
/// \param hum Address of variabele where humidity measurement result shall be stored.
/// \param temp Address of variabele where co2 measurement result shall be stored.
/// \param interval Address of variabele where interval time can be retrieved.
/// \return true: successfull acquisition of sensor values, false: failed to acquire sensor values from SCD30
bool getSensorValues(float &temp,float &hum, float &co2, uint32_t &interval){
  uint32_t counter = interval/10000*2;
  bool returnValue { false };
  
  while(counter > 0){
    debugSerial.print(F("Access SCD30, attempts left: "));
    debugSerial.println(counter);
    
    if (airSensor.dataAvailable()){
      debugSerial.println(F("Reading SCD30... "));
      co2 = (float)airSensor.getCO2();
      temp = airSensor.getTemperature();
      hum = (float)airSensor.getHumidity();
      counter = 0;
      returnValue = true;
    }else{
      delay(1000);
      counter--;
    }
  }
  return returnValue;
}

/// \brief Turn RGB led on with a given state colour of turn it off
/// Inspired on "KISSLoRa-demo_app.ino - demo application for the KISS LoRa gadget" code
/// source: https://github.com/YourproductSmarter/KISSLoRa-demo/blob/master/KISSLoRa-demo_app/KISSLoRa-demo_app.ino
static void set_rgb_led(uint8_t R, uint8_t G, uint8_t B){
  luminosity = get_lux_value();
  if(luminosity < LUMINOSITY_HIGH){
    R *= luminosity/LUMINOSITY_HIGH;
    G *= luminosity/LUMINOSITY_HIGH;
    B *= luminosity/LUMINOSITY_HIGH;
  }
  
  if(R == 0){  
    SoftPWMEnd(RGBLED_RED);
    pinMode(RGBLED_RED, OUTPUT);
    digitalWrite(RGBLED_RED, HIGH);
  }else{
    SoftPWMSet(RGBLED_RED,255-R);
  }
  
  if(G == 0){  
    SoftPWMEnd(RGBLED_GREEN);
    pinMode(RGBLED_GREEN, OUTPUT);
    digitalWrite(RGBLED_GREEN, HIGH);
  }else{
    SoftPWMSet(RGBLED_GREEN,255-G);
  }
  
  analogWrite(RGBLED_BLUE, 255-B);
}

/// \brief set RGB led color and adjust using actual luminosity
/// \param delay_time_ms wait time between transmit intervals
/// \param value to be displayed in 6 steps
/// \param range array with ranges for RGB display.
static void drawRGBMeter(uint32_t delay_time_ms, const float value, const uint16_t * range){
  uint8_t bars = 0;
  // Scale value in to ranges
  if((value >= range[0]) && (value < range[1])){
    bars = 2;
  }else if((value >= range[1]) && (value < range[2])){
    bars = 3;
  }else if((value >= range[2]) && (value < range[3])){
    bars = 4;
  }else if((value >= range[3]) && (value < range[4])){
    bars = 5;
  }else if((value >= range[4]) && (value < range[5])){
    bars = 6;
  }else if((value >= range[5]) && (value < range[6])){
    bars = 7;
  }else if(value >= range[6]){
    bars = 8;
  }
  // Display color bars
  while (delay_time_ms){
    int displayDelay{0};
    set_rgb_led(0,0,0);
    delay(BAR_DISPLAY_DURATION);
    displayDelay += BAR_DISPLAY_DURATION;
    for( int i = 0; i < bars; i++){
      set_rgb_led(color[i][0],color[i][1],color[i][2]);
      delay(BAR_DISPLAY_DURATION);
      displayDelay += BAR_DISPLAY_DURATION;
    }
      
    // Event: Send acknowledged message at alarm.
    if(alarm){
      debugSerial.println(F("-- ALARM!"));
      set_rgb_led(255,0,0);  //switch RGBLED_RED LED on

      // Wake RN2483 
      ttn.wake();
        
      lpp.reset();
      lpp.addPresence(LPP_CH_PRESENCE, ALARM);
      lpp.addDigitalInput(LPP_CH_SW_RELEASE, RELEASE);
  
      // Send it off
      ttn.sendBytes(lpp.getBuffer(), lpp.getSize(), APPLICATION_PORT_CAYENNE, true);

      // Set RN2483 to sleep mode
      ttn.sleep(delay_time_ms - 2200);
      // This delay is not optional, try to remove it
      // and say bye bye to your RN2483 sleep mode
      delay(50);
      
      alarm = false;
      
      set_rgb_led(0,0,0);  //switch RGBLED_RED LED off
    }
    
    if(delay_time_ms > displayDelay){
      delay_time_ms -= displayDelay;
    }else{
      delay_time_ms = 0;
    }
  }
}
