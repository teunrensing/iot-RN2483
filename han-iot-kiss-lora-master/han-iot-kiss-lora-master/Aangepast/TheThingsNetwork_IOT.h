/**
 * @file TheThingsNetwork_IOT.h
 * @brief This library provides an interface for LoRaWAN communication with an RN2384 using The Things Network.
 * Based on the original code from https://github.com/TheThingsNetwork/arduino-device-lib.git
 * This version is a library with reduced payload.
 * @author Marelle Vink, Teun Rensing, Lucas Nillesen, Raymon Albricht
 * @date 2024-03-10 (10 March)
 * @version 1.0
 */

#ifndef _THETHINGSNETWORK_H_
#define _THETHINGSNETWORK_H_

#include <Arduino.h>
#include <Stream.h>
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAMD)
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif



/**
 * @def TTN_DEFAULT_SF
 * Default spreading factor.
 */
#define TTN_DEFAULT_SF 7

/**
 * @def TTN_DEFAULT_FSB
 * Default frame sub-band.
 */
#define TTN_DEFAULT_FSB 2

/**
 * @def TTN_RETX
 * Default number of retransmissions.
 */
#define TTN_RETX "7"

/**
 * @def TTN_PWRIDX_EU868
 * Default power index for EU868 frequency plan.
 */
#define TTN_PWRIDX_EU868 "1"

/** All defines for regions are removed because we assume that the RN2384 is used in Europe.
// #define TTN_PWRIDX_US915 "5"
// #define TTN_PWRIDX_AU915 "5"
// #define TTN_PWRIDX_AS920_923 "1" // TODO: should be 0, but the current RN2903AS firmware doesn't accept that value (probably still using EU868: 1=14dBm)
// #define TTN_PWRIDX_AS923_925 "1" // TODO: should be 0
// #define TTN_PWRIDX_KR920_923 "1" // TODO: should be 0
// #define TTN_PWRIDX_IN865_867 "1" // TODO: should be 0
*/

/**
 * @def TTN_BUFFER_SIZE
 * Size of the buffer used for communication.
 */
#define TTN_BUFFER_SIZE 300

/**
 * @def TTN_DEFAULT_TIMEOUT
 * Default modem timeout in milliseconds.
 */
#define TTN_DEFAULT_TIMEOUT 10000

/**
 * @typedef port_t
 * Type definition for port number.
 */
typedef uint8_t port_t;

/**
 * @enum ttn_response_t
 * Enumerates possible responses from The Things Network operations.
 */
enum ttn_response_t
{
  TTN_ERROR_SEND_COMMAND_FAILED = (-1),
  TTN_ERROR_UNEXPECTED_RESPONSE = (-10),
  TTN_SUCCESSFUL_TRANSMISSION = 1,
  TTN_SUCCESSFUL_RECEIVE = 2,
  TTN_UNSUCCESSFUL_RECEIVE = 3
};


/**
 * @enum ttn_fp_t
 * Enumerates frequency plans for The Things Network. Deleted because we assume the library is used in Europe.
 */
/*
enum ttn_fp_t
{
  TTN_FP_EU868,
  TTN_FP_US915,
  TTN_FP_AU915,
  TTN_FP_AS920_923,
  TTN_FP_AS923_925,
  TTN_FP_KR920_923,
  TTN_FP_IN865_867
};*/


/**
 * @enum lorawan_class_t
 * Enumerates LoRaWAN device classes.
 * Class A device used -> Class A is a must for LoRaWan and class B and C are optional
 */

enum lorawan_class_t
{
  CLASS_A,
  // CLASS_B,
  // CLASS_C
};


/**
 * @enum ttn_response_code_t
 * Enumerates response codes for The Things Network operations.
 */
enum ttn_response_code_t
{
	TTN_OK,
	TTN_ERROR_BUSY = (-1),
	TTN_ERROR_FRAME_COUNTER_ERROR = (-2),
	TTN_ERROR_INVALID_CLASS = (-3),
	TTN_ERROR_INVALID_LENGTH = (-4),
	TTN_ERROR_INVALID_PARAMETER = (-5),
	TTN_ERROR_NO_KEY_INTITIALIZED = (-6),
	TTN_ERROR_MAC_PAUSE = (-7),
	TTN_ERROR_NO_KEY_MULTICAST = (-8),
	TTN_ERROR_NO_FREE_CHANNEL = (-9),
	TTN_ERROR_NOT_JOINED = (-10),
	TTN_ERROR_SILENT = (-11),
	TTN_ERROR_ERR = (-12),
};

/**
 * @enum ttn_modem_status_t
 * Enumerates modem status codes.
 */
enum ttn_modem_status_t
{
	TTN_MODEM_READ_ERR = -1,
	TTN_MODEM_IDLE = 0,
	TTN_MODEM_TX,
	TTN_MODEM_BEFORE_RX,
	TTN_MODEM_RX1,
	TTN_MODEM_BEFORE_RX2,
	TTN_MODEM_RETX_DELAY,
	TTN_MODEM_APB_DELAY,
	TTN_MODEM_C_RX1,
	TTN_MODEM_C_RX2
};

/**
 * @class TheThingsNetwork
 * @brief Class providing an interface for LoRaWAN communication.
 */
class TheThingsNetwork

/** @note !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* All functions in this class are documented in de .cpp file. 
* Also the commented out functions are documented, and why they are deleted.
* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

{
private:
  Stream *modemStream; ///< Pointer to the modem stream.
  Stream *debugStream = NULL; ///< Pointer to the debug stream.
  //ttn_fp_t fp; // See line 45. Enum could be deleted
  uint8_t sf; ///< Spreading factor.
  uint8_t fsb; ///< Frame sub-band. 
  bool adr; ///< Adaptive data rate setting.
  char buffer[512]; ///< Communication buffer.
  bool baudDetermined = false; ///< Flag indicating whether baud rate is determined.
  void (*messageCallback)(const uint8_t *payload, size_t size, port_t port); ///< Callback function for message reception.
  lorawan_class_t lw_class = CLASS_A; ///< LoRaWAN device class.

  void clearReadBuffer();
  size_t readLine(char *buffer, size_t size, uint8_t attempts = 3);
  size_t readResponse(uint8_t prefixTable, uint8_t indexTable, uint8_t index, char *buffer, size_t size);
  size_t readResponse(uint8_t table, uint8_t index, char *buffer, size_t size);

  void debugPrintIndex(uint8_t index, const char *value = NULL);
  void debugPrintMessage(uint8_t type, uint8_t index, const char *value = NULL);

  void autoBaud();
  void configureEU868();
  /* Next all removed because EU868 is used.
  void configureUS915(uint8_t fsb);
  void configureAU915(uint8_t fsb);
  void configureAS920_923();
  void configureAS923_925();
  void configureKR920_923();
  void configureIN865_867();
  */
  //void configureChannels(uint8_t fsb); // We could delete this function, because one channel is used
  bool setSF(uint8_t sf);// Can be changed further or can be deleted
  bool waitForOk();

  ttn_response_t parseBytes();
  void sendCommand(uint8_t table, uint8_t index, bool appendSpace, bool print = true);
  bool sendMacSet(uint8_t index, uint8_t value1, unsigned long value2);
  bool sendMacSet(uint8_t index, const char *value);
  bool sendChSet(uint8_t index, uint8_t channel, unsigned long value);
  bool sendChSet(uint8_t index, uint8_t channel, const char *value);
  bool sendJoinSet(uint8_t type);
  bool sendPayload(uint8_t mode, uint8_t port, uint8_t *payload, size_t len);
  void sendGetValue(uint8_t table, uint8_t prefix, uint8_t index);

public:
/* Commented functions are not neccessary public functions for compatibility wiht the TTN-library. If functions are not commented
  out but also not needed, a explanation is given where the function is used.
  */

/**
 * @brief Flag indicating whether a hard reset is needed.
 */
  bool needsHardReset = false;

  //TheThingsNetwork(Stream &modemStream, Stream &debugStream, ttn_fp_t fp, uint8_t sf = TTN_DEFAULT_SF, uint8_t fsb = TTN_DEFAULT_FSB);
    /**
     * @brief Constructor.
     * @param modemStream The stream for communication with the LoRaWAN modem.
     * @param debugStream The stream for debugging output.
     * @param sf The default spreading factor.
     * @param fsb The default frequency sub-band.
     * @param fp This parameter is deleted, because one region is used.
     */
  TheThingsNetwork(Stream &modemStream, Stream &debugStream, uint8_t sf = TTN_DEFAULT_SF, uint8_t fsb = TTN_DEFAULT_FSB); //removed ttn_fp_t fp

  void reset(bool adr = true); 
  void resetHard(uint8_t resetPin); 
  void showStatus(); 
  size_t getHardwareEui(char *buffer, size_t size); 
  size_t getAppEui(char *buffer, size_t size);
  size_t getVersion(char *buffer, size_t size);
  enum ttn_modem_status_t getStatus();
  uint16_t getVDD(); 

  // int16_t getRSSI();
  // uint32_t getFrequency();
  // uint32_t getWatchDogTimer();
  // uint32_t getFCU();
  // uint32_t getFCD();
  // uint8_t getBW();
  // uint8_t getCR();
  // int8_t getPower();
  // int8_t getSNR();
  // int8_t getDR();
  // int8_t getPowerIndex();
  // bool getChannelStatus (uint8_t channel);
  // ttn_response_code_t getLastError();
  void onMessage(void (*cb)(const uint8_t *payload, size_t size, port_t port)); 
  bool provision(const char *appEui, const char *appKey, bool resetFirst = true);
  bool provision(const char *devEui, const char *appEui, const char *appKey);
  bool join(const char *appEui, const char *appKey, int8_t retries = -1, uint32_t retryDelay = 10000); 
  bool join(const char *devEui, const char *appEui, const char *appKey, int8_t retries = -1, uint32_t retryDelay = 10000);
  bool join(int8_t retries = -1, uint32_t retryDelay = 10000); 
  bool personalize(const char *devAddr, const char *nwkSKey, const char *appSKey, bool resetFirst = true);
  bool personalize(); 
  //bool setClass(lorawan_class_t p_lw_class); // Used in join function
  ttn_response_t sendBytes(const uint8_t *payload, size_t length, port_t port = 1, bool confirm = false, uint8_t sf = 0); 
  ttn_response_t poll(port_t port = 1, bool confirm = false, bool modem_only = false);
  void sleep(uint32_t mseconds);
  void wake(); 
  void saveState(); 
  void linkCheck(uint16_t seconds);
  uint8_t getLinkCheckGateways(); 
  uint8_t getLinkCheckMargin();
  //bool setChannel(uint8_t channel, uint32_t frequency = 0l, uint8_t dr_min = 255, uint8_t dr_max = 255);
  // bool setRx2Channel(uint32_t frequency, uint8_t dr);
  // bool setChannelDCycle (uint8_t channel, float duty_cycle);
   bool setChannelStatus (uint8_t channel, bool status); // Used in configureEU868() function
  // bool setPowerIndex(uint8_t index);
  // bool setDR(uint8_t dr);
   bool setADR(bool adr);
  // bool setRX1Delay(uint16_t delay);
  // bool setFCU(uint32_t fcu);
  // bool setFCD(uint32_t fcd);
 // bool checkValidModuleConnected(bool autoBaudFirst = false);
};

#endif
