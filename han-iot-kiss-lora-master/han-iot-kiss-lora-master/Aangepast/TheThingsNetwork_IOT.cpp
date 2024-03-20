// Copyright © 2017 The Things Network
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#include "TheThingsNetwork_IOT.h"

#define DEBUG_SESSION 0 // Set to 1 to use debug functions

#if DEBUG_SESSION == 1
#define YES_DEBUG
#else
#define NO_DEBUG
#endif

/**
   @brief Macro for printing debug messages with a newline.

   This macro prints debug messages to the debug stream (if available) followed by a newline character.

   @param[in] ... Variable arguments list for the message to be printed.
*/
#define debugPrintLn(...)                \
  {                                      \
    if (debugStream)                     \
      debugStream->println(__VA_ARGS__); \
  }
/**
  @brief Macro for printing debug messages without a newline.

  This macro prints debug messages to the debug stream (if available) without appending a newline character.

  @param[in] ... Variable arguments list for the message to be printed.
*/
#define debugPrint(...)                \
  {                                    \
    if (debugStream)                   \
      debugStream->print(__VA_ARGS__); \
  }
/**
   @brief Macro to convert a hexadecimal character to its corresponding nibble value.

   This macro converts a hexadecimal character to its corresponding nibble value.

   @param[in] c The hexadecimal character.
   @return The nibble value.
*/
#define TTN_HEX_CHAR_TO_NIBBLE(c) ((c >= 'A') ? (c - 'A' + 0x0A) : (c - '0'))
/**
   @brief Macro to convert a pair of hexadecimal characters to a byte.

   This macro converts a pair of hexadecimal characters (representing a byte) to a byte value.

   @param[in] h The high nibble character.
   @param[in] l The low nibble character.
   @return The byte value.
*/
#define TTN_HEX_PAIR_TO_BYTE(h, l) ((TTN_HEX_CHAR_TO_NIBBLE(h) << 4) + TTN_HEX_CHAR_TO_NIBBLE(l))



const char ok[] PROGMEM = "ok";             /**< String constant "ok" stored in PROGMEM. */
const char on[] PROGMEM = "on";             /**< String constant "on" stored in PROGMEM. */
const char off[] PROGMEM = "off";           /**< String constant "off" stored in PROGMEM. */
const char accepted[] PROGMEM = "accepted"; /**< String constant "accepted" stored in PROGMEM. */
const char mac_tx_ok[] PROGMEM = "mac_tx_ok"; /**< String constant "mac_tx_ok" stored in PROGMEM. */
const char mac_rx[] PROGMEM = "mac_rx";     /**< String constant "mac_rx" stored in PROGMEM. */
const char mac_err[] PROGMEM = "mac_err";   /**< String constant "mac_err" stored in PROGMEM. */
const char rn2483[] PROGMEM = "RN2483";     /**< String constant "RN2483" stored in PROGMEM. */

/** @brief Here are constants that are not used because we don't use those microchips.
  const char rn2483a[] PROGMEM = "RN2483A";//Not in use
  const char rn2903[] PROGMEM = "RN2903";//Not in use
  const char rn2903as[] PROGMEM = "RN2903AS";//Not in use
*/

/** @brief Here is the edited compare table without the constants that are not used.
  original: const char *const compare_table[] PROGMEM = {ok, on, off, accepted, mac_tx_ok, mac_rx, mac_err, rn2483, rn2483a, rn2903, rn2903as};
  edited: const char *const compare_table[] PROGMEM = {ok, on, off, accepted, mac_tx_ok, mac_rx, mac_err, rn2483};
*/
const char *const compare_table[] PROGMEM = {ok, on, off, accepted, mac_tx_ok, mac_rx, mac_err, rn2483};


#define CMP_OK 0           /**< Constant representing "ok". */
#define CMP_ON 1           /**< Constant representing "on". */
#define CMP_OFF 2          /**< Constant representing "off". */
#define CMP_ACCEPTED 3     /**< Constant representing "accepted". */
#define CMP_MAC_TX_OK 4    /**< Constant representing "mac_tx_ok". */
#define CMP_MAC_RX 5       /**< Constant representing "mac_rx". */
#define CMP_MAC_ERR 6      /**< Constant representing "mac_err". */
#define CMP_RN2483 7       /**< Constant representing "RN2483". */
/** @brief Here are defines that are not used because we don't use those microchips.
  #define CMP_RN2483A 8
  #define CMP_RN2903 9
  #define CMP_RN2903AS 10
*/

// CMP OK
const char busy[] PROGMEM = "busy"; /**< @brief String constant stored in PROGMEM representing "busy". */
const char fram_counter_err_rejoin_needed[] PROGMEM = "fram_counter_err_rejoin_needed"; /**< @brief String constant stored in PROGMEM representing "fram_counter_err_rejoin_needed". */
const char invalid_class[] PROGMEM = "invalid_class"; /**< @brief String constant stored in PROGMEM representing "invalid_class". */
const char invalid_data_len[] PROGMEM = "invalid_data_len"; /**< @brief String constant stored in PROGMEM representing "invalid_data_len". */
const char invalid_param[] PROGMEM = "invalid_param"; /**< @brief String constant stored in PROGMEM representing "invalid_param". */
const char keys_not_init[] PROGMEM = "keys_not_init"; /**< @brief String constant stored in PROGMEM representing "keys_not_init". */
const char mac_paused[] PROGMEM = "mac_paused"; /**< @brief String constant stored in PROGMEM representing "mac_paused". */
const char multicast_keys_not_set[] PROGMEM = "multicast_keys_not_set"; /**< @brief String constant stored in PROGMEM representing "multicast_keys_not_set". */
const char no_free_ch[] PROGMEM = "no_free_ch"; /**< @brief String constant stored in PROGMEM representing "no_free_ch". */
const char not_joined[] PROGMEM = "not_joined"; /**< @brief String constant stored in PROGMEM representing "not_joined". */
const char silent[] PROGMEM = "silent"; /**< @brief String constant stored in PROGMEM representing "silent". */
const char err[] PROGMEM = "err"; /**< @brief String constant stored in PROGMEM representing "err". */

/**
 * @brief Array of PROGMEM strings used for comparison.
 * 
 * This array contains PROGMEM strings that are used for comparison purposes.
 * It includes strings like "busy", "invalid_param", "not_joined", etc.
 * These strings are stored in program memory to save RAM space.
 */
const char *const compareerr_table[] PROGMEM = {ok, busy, fram_counter_err_rejoin_needed, invalid_class, invalid_data_len, invalid_param, keys_not_init, mac_paused, multicast_keys_not_set, no_free_ch, not_joined, silent, err};

#if defined(YES_DEBUG)
#define CMP_ERR_OK 0 /**< @brief Error code representing OK. */
#define CMP_ERR_BUSY 1 /**< @brief Error code representing busy. */
#define CMP_ERR_FRMCNT 2 /**< @brief Error code representing frame counter error, rejoin needed. */
#define CMP_ERR_INVCLS 3 /**< @brief Error code representing invalid class. */
#define CMP_ERR_INVDLEN 4 /**< @brief Error code representing invalid data length. */
#define CMP_ERR_INVPAR 5 /**< @brief Error code representing invalid parameter. */
#define CMP_ERR_NKEYINT 6 /**< @brief Error code representing keys not initialized. */
#define CMP_ERR_MACPAUSE 7 /**< @brief Error code representing MAC paused. */
#define CMP_ERR_NKYMLTCST 8 /**< @brief Error code representing multicast keys not set. */
#define CMP_ERR_NFRCHN 9 /**< @brief Error code representing no free channel. */
#define CMP_ERR_NJOIN 10 /**< @brief Error code representing not joined. */
#define CMP_ERR_SILENT 11 /**< @brief Error code representing silent. */
#define CMP_ERR_ERR 12 /**< Error code representing error. */

#define CMP_ERR_LAST CMP_ERR_ERR /**< @brief Represents the last error code. */
#endif

#define SENDING "Sending: " /**< @brief Message prefix for sending. */
#define SEND_MSG "\r\n"    /**< @brief Message suffix for sending. */

#if defined(YES_DEBUG)
const char eui[] PROGMEM = "EUI: "; /**< @brief EUI string constant. */
const char battery[] PROGMEM = "Battery: "; /**< @brief Battery string constant. */
const char appEui[] PROGMEM = "AppEUI: "; /**< @brief AppEUI string constant. */
const char devEui[] PROGMEM = "DevEUI: "; /**< @brief DevEUI string constant. */
const char band[] PROGMEM = "Band: "; /**< @brief Band string constant. */
const char data_rate[] PROGMEM = "Data Rate: "; /**< @brief Data Rate string constant. */
const char rx_delay_1[] PROGMEM = "RX Delay 1: "; /**< @brief RX Delay 1 string constant. */
const char rx_delay_2[] PROGMEM = "RX Delay 2: "; /**< @brief RX Delay 2 string constant. */
const char version[] PROGMEM = "Version: "; /**< @brief Version string constant. */
const char model[] PROGMEM = "Model: "; /**< @brief Model string constant. */
const char devaddr[] PROGMEM = "DevAddr: "; /**< @brief DevAddr string constant. */

const char *const show_table[] PROGMEM = {eui, battery, appEui, devEui, band, data_rate, rx_delay_1, rx_delay_2, version, model, devaddr};/**< @brief Array of string constants for displaying information. */
#endif

#define SHOW_EUI 0 /**< @brief Index for EUI display. */
#define SHOW_BATTERY 1 /**< @brief Index for battery display. */
#define SHOW_APPEUI 2 /**< @brief Index for AppEUI display. */
#define SHOW_DEVEUI 3 /**< @brief Index for DevEUI display. */
#define SHOW_BAND 4 /**< @brief Index for band display. */
#define SHOW_DATA_RATE 5 /**< @brief Index for data rate display. */
#define SHOW_RX_DELAY_1 6 /**< @brief Index for RX Delay 1 display. */
#define SHOW_RX_DELAY_2 7 /**< @brief Index for RX Delay 2 display. */
#define SHOW_VERSION 8 /**< @brief Index for version display. */
#define SHOW_MODEL 9 /**< @brief Index for model display. */
#define SHOW_DEVADDR 10 /**< @brief Index for DevAddr display. */

#if defined(YES_DEBUG)
/** @brief Error message for invalid spreading factor (SF). */
const char PROGMEM invalid_sf[] = "Invalid SF";
/** @brief Error message for invalid frequency plan. */
const char PROGMEM invalid_fp[] = "Invalid frequency plan";
/** @brief Error message for unexpected response. */
const char PROGMEM unexpected_response[] = "Unexpected response: ";
/** @brief Error message for failed command sending. */
const char PROGMEM send_command_failed[] = "Send command failed";
/** @brief Error message for failed join command. */
const char PROGMEM join_failed[] = "Send join command failed";
/** @brief Error message for join not accepted. */
const char PROGMEM join_not_accepted[] = "Join not accepted: ";
/** @brief Error message for personalize not accepted. */
const char PROGMEM personalize_not_accepted[] = "Personalize not accepted";
/** @brief Error message for response not being OK. */
const char PROGMEM response_is_not_ok[] = "Response is not OK: ";
/** @brief Error message for invalid key length. */
const char PROGMEM error_key_length[] = "One or more keys are of invalid length.";
/** @brief Check configuration message. */
const char PROGMEM check_configuration[] = "Check your coverage, keys and backend status.";
/** @brief No response message. */
const char PROGMEM no_response[] = "No response from RN module.";
/** @brief Error message for invalid module. */
const char PROGMEM invalid_module[] = "Invalid module (must be RN2xx3[xx]).";

/** @brief Array of error messages stored in PROGMEM. */
const char *const PROGMEM error_msg[] = {invalid_sf, invalid_fp, unexpected_response, send_command_failed, join_failed, join_not_accepted, personalize_not_accepted, response_is_not_ok, error_key_length, check_configuration, no_response, invalid_module};
#endif

/** @brief Error code for "Invalid SF". */
#define ERR_INVALID_SF 0
/** @brief Error code for "Invalid frequency plan". */
#define ERR_INVALID_FP 1
/** @brief Error code for "Unexpected response". */
#define ERR_UNEXPECTED_RESPONSE 2
/** @brief Error code for "Send command failed". */
#define ERR_SEND_COMMAND_FAILED 3
/** @brief Error code for "Send join command failed". */
#define ERR_JOIN_FAILED 4
/** @brief Error code for "Join not accepted". */
#define ERR_JOIN_NOT_ACCEPTED 5
/** @brief Error code for "Personalize not accepted". */
#define ERR_PERSONALIZE_NOT_ACCEPTED 6
/** @brief Error code for "Response is not OK". */
#define ERR_RESPONSE_IS_NOT_OK 7
/** @brief Error code for "One or more keys are of invalid length". */
#define ERR_KEY_LENGTH 8
/** @brief Error code for "Check your coverage, keys and backend status". */
#define ERR_CHECK_CONFIGURATION 9
/** @brief Error code for "No response from RN module". */
#define ERR_NO_RESPONSE 10
/** @brief Error code for "Invalid module (must be RN2xx3[xx])". */
#define ERR_INVALID_MODULE 11

#if defined(YES_DEBUG)
/** @brief String indicating personalize accepted along with status. */
const char personalize_accepted[] PROGMEM = "Personalize accepted. Status: ";

/** @brief String indicating join accepted along with status. */
const char join_accepted[] PROGMEM = "Join accepted. Status: ";

/** @brief String indicating successful transmission. */
const char successful_transmission[] PROGMEM = "Successful transmission";

/** @brief String indicating successful transmission along with received data. */
const char successful_transmission_received[] PROGMEM = "Successful transmission. Received ";

/** @brief String indicating a valid module connection. */
const char valid_module[] PROGMEM = "Valid module connected.";

/**
 * @brief Array of success messages.
 * 
 * Contains messages indicating various success states.
 */
const char *const success_msg[] PROGMEM = {personalize_accepted, join_accepted, successful_transmission, successful_transmission_received, valid_module};
#endif

#define SCS_PERSONALIZE_ACCEPTED 0 /**< @brief Success message index for "Personalize accepted". */
#define SCS_JOIN_ACCEPTED 1 /**< @brief Success message index for "Join accepted". */
#define SCS_SUCCESSFUL_TRANSMISSION 2 /**< @brief Success message index for "Successful transmission". */
#define SCS_SUCCESSFUL_TRANSMISSION_RECEIVED 3 /**< @brief Success message index for "Successful transmission received". */
#define SCS_VALID_MODULE 4 /**< @brief Success message index for "Valid module connected". */

#if defined(YES_DEBUG)
const char radio_prefix[] PROGMEM = "radio"; /**< @brief Prefix for radio commands. */
const char radio_set[] PROGMEM = "set"; /**< @brief Command for setting radio parameters. */
const char radio_get[] PROGMEM = "get"; /**< @brief Command for getting radio parameters. */
const char radio_get_bw[] PROGMEM = "bw"; /**< @brief Command for getting bandwidth. */
const char radio_get_prlen[] PROGMEM = "prlen"; /**< @brief Command for getting preamble length. */
const char radio_get_crc[] PROGMEM = "crc"; /**< @brief Command for getting CRC status. */
const char radio_get_cr[] PROGMEM = "cr"; /**< @brief Command for getting coding rate. */
const char radio_get_sf[] PROGMEM = "sf"; /**< @brief Command for getting spreading factor. */
const char radio_get_freq[] PROGMEM = "freq"; /**< @brief Command for getting frequency. */
const char radio_get_rxbw[] PROGMEM = "rxbw"; /**< @brief Command for getting receive bandwidth. */
const char radio_get_wdt[] PROGMEM = "wdt"; /**< @brief Command for getting watchdog timeout. */
const char radio_get_pwr[] PROGMEM = "pwr"; /**< @brief Command for getting power level. */
const char radio_get_rssi[] PROGMEM = "rssi"; /**< @brief Command for getting RSSI. */
const char radio_get_snr[] PROGMEM = "snr"; /**< @brief Command for getting SNR. */

const char *const radio_table[] PROGMEM = {radio_prefix, radio_set, radio_get, radio_get_bw, radio_get_prlen, radio_get_crc, radio_get_cr, radio_get_sf, radio_get_freq, radio_get_rxbw, radio_get_wdt, radio_get_pwr, radio_get_rssi, radio_get_snr};/**< @brief Table of radio commands. */

#define RADIO_PREFIX 0 /**< @brief Radio prefix command index. */
#define RADIO_SET 1 /**< @brief Radio set command index. */
#define RADIO_GET 2 /**< @brief Radio get command index. */
#define RADIO_GET_BW 3 /**< @brief Radio get bandwidth command index. */
#define RADIO_GET_PRLEN 4 /**< @brief Radio get preamble length command index. */
#define RADIO_GET_CRC 5 /**< @brief Radio get CRC command index. */
#define RADIO_GET_CR 6 /**< @brief Radio get coding rate command index. */
#define RADIO_GET_SF 7 /**< @brief Radio get spreading factor command index. */
#define RADIO_GET_FREQ 8 /**< @brief Radio get frequency command index. */
#define RADIO_GET_RXBW 9 /**< @brief Radio get receive bandwidth command index. */
#define RADIO_GET_WDT 10 /**< @brief Radio get watch dog timer command index. */
#define RADIO_GET_PWR 11 /**< @brief Radio get power command index. */
#define RADIO_GET_RSSI 12 /**< @brief Radio get RSSI command index. */
#define RADIO_GET_SNR /**< @brief Radio get SNR command index. */
#endif

const char sys_prefix[] PROGMEM = "sys"; /**< @brief System prefix string. */
const char sys_sleep[] PROGMEM = "sleep"; /**< @brief System sleep command string. */
const char sys_reset[] PROGMEM = "reset"; /**< @brief System reset command string. */
const char sys_erase_fw[] PROGMEM = "eraseFW"; /**< @brief System erase firmware command string. */
const char sys_factory_rst[] PROGMEM = "factoryRESET"; /**< System factory reset command string. */
const char sys_set[] PROGMEM = "set"; /**< @brief System set command string. */
const char sys_get[] PROGMEM = "get"; /**< @brief System get command string. */
const char sys_get_ver[] PROGMEM = "ver"; /**< @brief System get version command string. */
const char sys_get_vdd[] PROGMEM = "vdd"; /**< @brief System get VDD command string. */
const char sys_get_hweui[] PROGMEM = "hweui"; /**< @brief System get HWEUI command string. */
const char sys_set_get_nvm[] PROGMEM = "nvm"; /**< @brief System set/get NVM command string. */
const char sys_set_pindig[] PROGMEM = "pindig"; /**< @brief System set pin digital command string. */

const char *const sys_table[] PROGMEM = {sys_prefix, sys_sleep, sys_reset, sys_erase_fw, sys_factory_rst, sys_set, sys_get, sys_get_ver, sys_get_vdd, sys_get_hweui, sys_set_get_nvm, sys_set_pindig};/**< @brief Table of system commands. */

#define SYS_PREFIX 0 /**< @brief System prefix command index. */
#define SYS_SLEEP 1 /**< @brief System sleep command index. */
#define SYS_RESET 2 /**< @brief System reset command index. */
#define SYS_ERASE_FW 3 /**< @brief System erase firmware command index. */
#define SYS_FACTORY_RST 4 /**< @brief System factory reset command index. */
#define SYS_SET 5 /**< @brief System set command index. */
#define SYS_GET 6 /**< @brief System get command index. */
#define SYS_GET_VER 7 /**< @brief System get version command index. */
#define SYS_GET_VDD 8 /**< @brief System get VDD command index. */
#define SYS_GET_HWEUI 9 /**< @brief System get HWEUI command index. */
#define SYS_SET_GET_NVM 10 /**< @brief System set/get NVM command index. */
#define SYS_SET_PINDIG 11 /**< @brief System set pin digital command index. */

const char mac_prefix[] PROGMEM = "mac"; /**< @brief MAC prefix string. */
const char mac_reset[] PROGMEM = "reset"; /**< @brief MAC reset command string. */
const char mac_tx[] PROGMEM = "tx"; /**< @brief MAC transmit command string. */
const char mac_join[] PROGMEM = "join"; /**< @brief MAC join command string. */
const char mac_save[] PROGMEM = "save"; /**< @brief MAC save command string. */
const char mac_force_enable[] PROGMEM = "forceENABLE"; /**< @brief MAC force enable command string. */
const char mac_pause[] PROGMEM = "pause"; /**< @brief MAC pause command string. */
const char mac_resume[] PROGMEM = "resume"; /**< @brief MAC resume command string. */
const char mac_set[] PROGMEM = "set"; /**< @brief MAC set command string. */
const char mac_get[] PROGMEM = "get"; /**< @brief MAC get command string. */

const char *const mac_table[] PROGMEM = {mac_prefix, mac_reset, mac_tx, mac_join, mac_save, mac_force_enable, mac_pause, mac_resume, mac_set, mac_get};/**< @brief Table of mac commands. */

#define MAC_PREFIX 0 /**< @brief MAC prefix command index. */
#define MAC_RESET 1 /**< @brief MAC reset command index. */
#define MAC_TX 2 /**< @brief MAC transmit command index. */
#define MAC_JOIN 3 /**< @brief MAC join command index. */
#define MAC_SAVE 4 /**< @brief MAC save command index. */
#define MAC_FORCE_ENABLE 5 /**< @brief MAC force enable command index. */
#define MAC_PAUSE 6 /**< @brief MAC pause command index. */
#define MAC_RESUME 7 /**< @brief MAC resume command index. */
#define MAC_SET 8 /**< @brief MAC set command index. */
#define MAC_GET 9 /**< @brief MAC get command index. */

const char mac_devaddr[] PROGMEM = "devaddr"; /**< MAC device address key. */
const char mac_deveui[] PROGMEM = "deveui"; /**< MAC device EUI key. */
const char mac_appeui[] PROGMEM = "appeui"; /**< MAC application EUI key. */
const char mac_nwkskey[] PROGMEM = "nwkskey"; /**< MAC network session key key. */
const char mac_appskey[] PROGMEM = "appskey"; /**< MAC application session key key. */
const char mac_appkey[] PROGMEM = "appkey"; /**< MAC application key key. */
const char mac_pwridx[] PROGMEM = "pwridx"; /**< MAC power index key. */
const char mac_dr[] PROGMEM = "dr"; /**< MAC data rate key. */
const char mac_adr[] PROGMEM = "adr"; /**< MAC adaptive data rate key. */
const char mac_bat[] PROGMEM = "bat"; /**< MAC battery key. */
const char mac_retx[] PROGMEM = "retx"; /**< MAC retransmissions key. */
const char mac_linkchk[] PROGMEM = "linkchk"; /**< MAC link check key. */
const char mac_rxdelay1[] PROGMEM = "rxdelay1"; /**< MAC receive delay 1 key. */
const char mac_rxdelay2[] PROGMEM = "rxdelay2"; /**< MAC receive delay 2 key. */
const char mac_band[] PROGMEM = "band"; /**< MAC band key. */
const char mac_ar[] PROGMEM = "ar"; /**< MAC acknowledgment request key. */
const char mac_rx2[] PROGMEM = "rx2"; /**< MAC receive window 2 key. */
const char mac_ch[] PROGMEM = "ch"; /**< MAC channel key. */
const char mac_gwnb[] PROGMEM = "gwnb"; /**< MAC gateway count key. */
const char mac_mrgn[] PROGMEM = "mrgn"; /**< MAC margin key. */
const char mac_class[] PROGMEM = "class"; /**< MAC class key. */
const char mac_status[] PROGMEM = "status"; /**< MAC status key. */
const char mac_upctr[] PROGMEM = "upctr"; /**< MAC uplink counter key. */
const char mac_dnctr[] PROGMEM = "dnctr"; /**< MAC downlink counter key. */

const char *const mac_options[] PROGMEM = {mac_devaddr, mac_deveui, mac_appeui, mac_nwkskey, mac_appskey, mac_appkey, mac_pwridx, mac_dr, mac_adr, mac_bat, mac_retx, mac_linkchk, mac_rxdelay1, mac_rxdelay2, mac_band,
                                           mac_ar, mac_rx2, mac_ch, mac_gwnb, mac_mrgn, mac_class, mac_status, mac_upctr, mac_dnctr
                                          };/**< MAC options list*/


#define MAC_DEVADDR 0 /**< MAC device address index. */
#define MAC_DEVEUI 1 /**< MAC device EUI index. */
#define MAC_APPEUI 2 /**< MAC application EUI index. */
#define MAC_NWKSKEY 3 /**< MAC network session key index. */
#define MAC_APPSKEY 4 /**< MAC application session key index. */
#define MAC_APPKEY 5 /**< MAC application key index. */
#define MAC_PWRIDX 6 /**< MAC power index index. */
#define MAC_DR 7 /**< MAC data rate index. */
#define MAC_ADR 8 /**< MAC adaptive data rate index. */
#define MAC_BAT 9 /**< MAC battery index. */
#define MAC_RETX 10 /**< MAC retransmissions index. */
#define MAC_LINKCHK 11 /**< MAC link check index. */
#define MAC_RXDELAY1 12 /**< MAC receive delay 1 index. */
#define MAC_RXDELAY2 13 /**< MAC receive delay 2 index. */
#define MAC_BAND 14 /**< MAC band index. */
#define MAC_AR 15 /**< MAC acknowledgment request index. */
#define MAC_RX2 16 /**< MAC receive window 2 index. */
#define MAC_CH 17 /**< MAC channel index. */
#define MAC_GWNB 18 /**< MAC gateway count index. */
#define MAC_MRGN 19 /**< MAC margin index. */
#define MAC_CLASS 20 /**< MAC class index. */
#define MAC_STATUS 21 /**< MAC status index. */
#define MAC_UPCTR 22 /**< MAC uplink counter index. */
#define MAC_DNCTR 23 /**< MAC downlink counter index. */

const char mac_join_mode_otaa[] PROGMEM = "otaa"; /**< MAC join mode OTAA. */
const char mac_join_mode_abp[] PROGMEM = "abp"; /**< MAC join mode ABP. */

const char *const mac_join_mode[] PROGMEM = {mac_join_mode_otaa, mac_join_mode_abp}; /**< Array of MAC join modes. */

#define MAC_JOIN_MODE_OTAA 0 /**< MAC join mode OTAA index. */
#define MAC_JOIN_MODE_ABP 1 /**< MAC join mode ABP index. */

const char channel_dcycle[] PROGMEM = "dcycle"; /**< Channel duty cycle option. */
const char channel_drrange[] PROGMEM = "drrange"; /**< Channel data rate range option. */
const char channel_freq[] PROGMEM = "freq"; /**< Channel frequency option. */
const char channel_status[] PROGMEM = "status"; /**< Channel status option. */

const char *const mac_ch_options[] PROGMEM = {channel_dcycle, channel_drrange, channel_freq, channel_status}; /**< Array of MAC channel options. */

#define MAC_CHANNEL_DCYCLE 0 /**< MAC channel duty cycle index. */
#define MAC_CHANNEL_DRRANGE 1 /**< MAC channel data rate range index. */
#define MAC_CHANNEL_FREQ 2 /**< MAC channel frequency index. */
#define MAC_CHANNEL_STATUS 3 /**< MAC channel status index. */

const char mac_tx_type_cnf[] PROGMEM = "cnf"; /**< MAC transmission type confirmation. */
const char mac_tx_type_ucnf[] PROGMEM = "uncnf"; /**< MAC transmission type unconfirmed. */

const char *const mac_tx_table[] PROGMEM = {mac_tx_type_cnf, mac_tx_type_ucnf}; /**< Array of MAC transmission types. */

#define MAC_TX_TYPE_CNF 0 /**< MAC transmission type confirmation index. */
#define MAC_TX_TYPE_UCNF 1 /**< MAC transmission type unconfirmed index. */

#define MAC_TABLE 0 /**< MAC table index. */
#define MAC_GET_SET_TABLE 1 /**< MAC get/set table index. */
#define MAC_JOIN_TABLE 2 /**< MAC join table index. */
#define MAC_CH_TABLE 3 /**< MAC channel table index. */
#define MAC_TX_TABLE 4 /**< MAC transmission table index. */
#define SYS_TABLE 5 /**< System table index. */
#define RADIO_TABLE 6 /**< Radio table index. */
#define ERR_MESSAGE 7 /**< Error message index. */
#define SUCCESS_MESSAGE 8 /**< Success message index. */
#define CMP_TABLE 9 /**< Comparison table index. */
#define CMP_ERR_TABLE 10 /**< Comparison error table index. */

/**
 * @brief Compares a string with a string stored in PROGMEM.
 * 
 * This function compares a null-terminated string with a string stored in PROGMEM
 * and returns the result of the comparison.
 * 
 * @param str1 The null-terminated string to compare.
 * @param str2Index The index of the string stored in PROGMEM to compare with.
 * @param table The table index specifying the PROGMEM table to search from.
 *              Default is CMP_TABLE.
 * @return An integer less than, equal to, or greater than zero if str1 is found,
 *         respectively, to be less than, to match, or be greater than str2.
 */
int pgmstrcmp(const char *str1, uint8_t str2Index, uint8_t table = CMP_TABLE)
{
  if (0 == strlen(str1))
    return -1;

  char str2[128];

  switch (table) {
    case CMP_ERR_TABLE:
      strcpy_P(str2, (char *)pgm_read_word(&(compareerr_table[str2Index])));
      break;

    default:
    case CMP_TABLE:
      strcpy_P(str2, (char *)pgm_read_word(&(compare_table[str2Index])));
  }

  return memcmp(str1, str2, min(strlen(str1), strlen(str2)));
}

/**
 * @brief Calculates the number of digits in a number.
 * 
 * This function calculates the number of digits in a given number.
 * 
 * @param port The number to calculate the digits for.
 * @return The number of digits in the given number.
 */
uint8_t digits(uint8_t port)
{
  if (port >= 100)
  {
    return 3;
  }
  else if (port >= 10)
  {
    return 2;
  }
  return 1;
}

/**
 * @brief Extracts the port number from a string.
 * 
 * This function extracts the port number from a given string.
 * 
 * @param s The string containing the port number.
 * @return The port number extracted from the string.
 */
uint8_t receivedPort(const char *s)
{
  uint8_t port = 0;
  uint8_t i = 0;
  while (s[i] != ' ' && s[i] != '\0')
  {
    port *= 10;
    port += s[i] - 48;
    i++;
  }
  return port;
}

/**
 * @brief Constructor for TheThingsNetwork class.
 * 
 * Initializes an instance of TheThingsNetwork class with the provided modem stream,
 * debug stream, spreading factor (SF), and frequency sub-band (FSB).
 * 
 * The original function is TheThingsNetwork::TheThingsNetwork(Stream &modemStream, Stream &debugStream, ttn_fp_t fp, uint8_t sf, uint8_t fsb).
 * In the edited function sf is nolonger a parm for the function because we use constant value.
 * 
 * @param modemStream The stream for communicating with the modem.
 * @param debugStream The stream for debug output.
 * @param sf The spreading factor to be used.
 * @param fsb The frequency sub-band to be used.
 */
TheThingsNetwork::TheThingsNetwork(Stream &modemStream, Stream &debugStream, uint8_t sf, uint8_t fsb)
{
  this->debugStream = &debugStream;
  this->modemStream = &modemStream;
  this->modemStream->setTimeout(TTN_DEFAULT_TIMEOUT);
  // this->fp = fp;
  this->sf = sf;
  this->fsb = fsb;
  this->adr = false;
  this->messageCallback = NULL;
}

/**
 * @brief Retrieves the Application EUI (AppEUI) from the device.
 * 
 * This function reads the AppEUI from the device and stores it in the provided buffer.
 * 
 * @param buffer A pointer to the buffer where the AppEUI will be stored.
 * @param size The size of the buffer.
 * @return The number of characters read and stored in the buffer.
 */
size_t TheThingsNetwork::getAppEui(char *buffer, size_t size)
{
  return readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_APPEUI, buffer, size);
}

/**
 * @brief Retrieves the Hardware EUI (HWEUI) from the device.
 * 
 * This function reads the HWEUI from the device and stores it in the provided buffer.
 * 
 * @param buffer A pointer to the buffer where the HWEUI will be stored.
 * @param size The size of the buffer.
 * @return The number of characters read and stored in the buffer.
 */
size_t TheThingsNetwork::getHardwareEui(char *buffer, size_t size)
{
  return readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_HWEUI, buffer, size);
}

/**
 * @brief Retrieves the firmware version from the device.
 * 
 * This function reads the firmware version from the device and stores it in the provided buffer.
 * 
 * @param buffer A pointer to the buffer where the firmware version will be stored.
 * @param size The size of the buffer.
 * @return The number of characters read and stored in the buffer.
 */
size_t TheThingsNetwork::getVersion(char *buffer, size_t size)
{
  return readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_VER, buffer, size);
}

uint16_t TheThingsNetwork::getVDD()
{
  if (readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_VDD, buffer, sizeof(buffer)) > 0) {
    return atoi(buffer);
  }
  return 0;
}
/** @brief These functions are not used and not needed by default.
 uint8_t TheThingsNetwork::getBW()
 {
   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_BW, buffer, sizeof(buffer)) > 0) {
     return atoi(buffer);
   }
   return 0;
 }

 uint8_t TheThingsNetwork::getCR()
 {
   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_CR, buffer, sizeof(buffer)) > 2) {
     return atoi(buffer+2); // skip the first two chars "4/"
   }
   return 0;
 }

 uint32_t TheThingsNetwork::getFrequency()
 {
   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_FREQ, buffer, sizeof(buffer)) > 0) {
     return atol(buffer);
   }
   return 0;
 }

 uint32_t TheThingsNetwork::getFCU()
 {
   if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_UPCTR, buffer, sizeof(buffer)) > 0) {
     return strtoul(buffer, NULL, 10);
   }
   return 0;
 }

 uint32_t TheThingsNetwork::getFCD()
 {
   if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_DNCTR, buffer, sizeof(buffer)) > 0) {
     return strtoul(buffer, NULL, 10);
   }
   return 0;
 }

 uint32_t TheThingsNetwork::getWatchDogTimer()
 {
   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_WDT, buffer, sizeof(buffer)) > 0) {
     return atol(buffer);
   }
   return 0;
 }
*/
enum ttn_modem_status_t TheThingsNetwork::getStatus()
{
  if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_STATUS, buffer, sizeof(buffer)) > 0) {
    char **endptr = NULL;
    int status = (strtol(buffer, endptr, 16) & 0x0F); // Mask out only active status

    if (endptr == NULL)
      return (enum ttn_modem_status_t)status;
  }
  return TTN_MODEM_READ_ERR; // unable to read status
}

// int8_t TheThingsNetwork::getPower()
// {
//   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_PWR, buffer, sizeof(buffer)) > 0) {
//     return atoi(buffer);
//   }
//   return -128;
// }

// int16_t TheThingsNetwork::getRSSI()
// {
//   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_RSSI, buffer, sizeof(buffer)) > 0) {
//     return atoi(buffer);
//   }
//   return -255;
// }

// int8_t TheThingsNetwork::getSNR()
// {
//   if (readResponse(RADIO_TABLE, RADIO_TABLE, RADIO_GET_SNR, buffer, sizeof(buffer)) > 0) {
//     return atoi(buffer);
//   }
//   return -128;
// }

// int8_t TheThingsNetwork::getDR()
// {
//   if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_DR, buffer, sizeof(buffer))){
//     return atoi(buffer);
//   }
//   return -1;
// }

// int8_t TheThingsNetwork::getPowerIndex()
// {
//   if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_PWRIDX, buffer, sizeof(buffer)) > 0) {
//     return atoi(buffer);
//   }
//   return -1;
// }

// bool TheThingsNetwork::getChannelStatus (uint8_t channel)
// {
//   char str[5];
//   if (channel > 9)
//   {
// 	str[0] = ((channel - (channel % 10)) / 10) + 48;
// 	str[1] = (channel % 10) + 48;
// 	str[2] = '\0';
//   }
//   else
//   {
// 	str[0] = channel + 48;
// 	str[1] = '\0';
//   }
//   sendCommand(MAC_TABLE, MAC_PREFIX, true, false);
//   sendCommand(MAC_TABLE, MAC_GET, true, false); // default "get " in between (same as radio get)
//   sendCommand(MAC_GET_SET_TABLE, MAC_CH, true, false);
//   sendCommand(MAC_CH_TABLE, MAC_CHANNEL_STATUS, true, false);
//   modemStream->write(str);
//   modemStream->write(SEND_MSG);

//   if (readLine(buffer, sizeof(buffer)))
// 	  return (pgmstrcmp(buffer, CMP_ON) == 0); // true if on, false if off or an error occurs
//   else
// 	  return false; // error
// }

// ttn_response_code_t TheThingsNetwork::getLastError(){

// 	int match, pos;
// 	for (pos=0; pos <= CMP_ERR_LAST; pos++){
// 		match = pgmstrcmp(buffer, pos, CMP_ERR_TABLE);
// 		if (match == 0)
// 			break;
// 	}

// 	return (ttn_response_code_t)(-1* pos); // code order is equal
// }

void TheThingsNetwork::debugPrintIndex(uint8_t index, const char *value)
{
#if defined(YES_DEBUG)
  char message[100];
  strcpy_P(message, (char *)pgm_read_word(&(show_table[index])));
  debugPrint(message);
  if (value)
  {
    debugPrintLn(value);
  }
#else
  // Do nothing
#endif
}

void TheThingsNetwork::debugPrintMessage(uint8_t type, uint8_t index, const char *value)
{
#if defined(YES_DEBUG)
  char message[100];
  switch (type)
  {
    case ERR_MESSAGE:
      strcpy_P(message, (char *)pgm_read_word(&(error_msg[index])));
      break;
    case SUCCESS_MESSAGE:
      strcpy_P(message, (char *)pgm_read_word(&(success_msg[index])));
      break;
  }
  debugPrint(message);
  if (value)
  {
    debugPrintLn(value);
  }
  else
  {
    debugPrintLn();
  }
#else
  // Do nothing, not debugging
#endif
}

void TheThingsNetwork::clearReadBuffer()
{
  while (modemStream->available())
  {
    modemStream->read();
  }
}

size_t TheThingsNetwork::readLine(char *buffer, size_t size, uint8_t attempts)
{
  size_t read = 0;
  while (!read && attempts--)
  {
    read = modemStream->readBytesUntil('\n', buffer, size);
  }
  if (!read)
  { // If attempts is activated return 0 and set RN state marker
    this->needsHardReset = true; // Inform the application about the radio module is not responsive.
#if defined(YES_DEBUG)
    debugPrintMessage(ERR_MESSAGE, ERR_NO_RESPONSE);
#endif
    return 0;
  }
  buffer[read - 1] = '\0'; // set \r to \0
  return read;
}

size_t TheThingsNetwork::readResponse(uint8_t prefixTable, uint8_t index, char *buffer, size_t size)
{
  clearReadBuffer();
  sendCommand(prefixTable, 0, true, false);
  sendCommand(prefixTable, index, false, false);
  modemStream->write(SEND_MSG);
  return readLine(buffer, size);
}

size_t TheThingsNetwork::readResponse(uint8_t prefixTable, uint8_t indexTable, uint8_t index, char *buffer, size_t size)
{
  clearReadBuffer();
  sendCommand(prefixTable, 0, true, false);
  sendCommand(MAC_TABLE, MAC_GET, true, false);
  sendCommand(indexTable, index, false, false);
  modemStream->write(SEND_MSG);
  return readLine(buffer, size);
}

void TheThingsNetwork::autoBaud()
{
  // Courtesy of @jpmeijers
  modemStream->setTimeout(2000);
  uint8_t attempts = 10;
  size_t length = 0;
  while (attempts-- && length == 0)
  {
    delay(100);
    modemStream->write((byte)0x00);
    modemStream->write(0x55);
    modemStream->write(SEND_MSG);
    sendCommand(SYS_TABLE, 0, true, false);
    sendCommand(SYS_TABLE, SYS_GET, true, false);
    sendCommand(SYS_TABLE, SYS_GET_VER, false, false);
    modemStream->write(SEND_MSG);
    length = modemStream->readBytesUntil('\n', buffer, sizeof(buffer));
  }
  delay(100);
  clearReadBuffer();
  modemStream->setTimeout(TTN_DEFAULT_TIMEOUT);
  baudDetermined = true;
}

void TheThingsNetwork::reset(bool adr)
{
  // autobaud and send "sys reset"
  autoBaud();
  readResponse(SYS_TABLE, SYS_RESET, buffer, sizeof(buffer));

  // autobaud (again, because baudrate was reset with "sys reset") and get HW model and SW version
  autoBaud();
  getVersion(buffer, sizeof(buffer));

  // buffer contains "RN2xx3[xx] x.x.x ...", splitting model from version
  char *model = strtok(buffer, " ");
  debugPrintIndex(SHOW_MODEL, model);
  char *version = strtok(NULL, " ");
  debugPrintIndex(SHOW_VERSION, version);

  // set DEVEUI as HWEUI
  readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_HWEUI, buffer, sizeof(buffer));
  sendMacSet(MAC_DEVEUI, buffer);
  // set ADR
  setADR(adr);
}

void TheThingsNetwork::resetHard(uint8_t resetPin) {
  digitalWrite(resetPin, LOW);
  delay(1000);
  digitalWrite(resetPin, HIGH);
}

void TheThingsNetwork::saveState()
{
#if defined(YES_DEBUG)
  debugPrint(SENDING);
#endif
  sendCommand(MAC_TABLE, MAC_PREFIX, true);
  sendCommand(MAC_TABLE, MAC_SAVE, false);
  modemStream->write(SEND_MSG);
  debugPrintLn();
  waitForOk();
}

void TheThingsNetwork::onMessage(void (*cb)(const uint8_t *payload, size_t size, port_t port))
{
  messageCallback = cb;
}

bool TheThingsNetwork::personalize(const char *devAddr, const char *nwkSKey, const char *appSKey, bool resetFirst)
{
  if (resetFirst) {
    reset(adr);
  }
  if (strlen(devAddr) != 8 || strlen(appSKey) != 32 || strlen(nwkSKey) != 32)
  {
#if defined(YES_DEBUG)
    debugPrintMessage(ERR_MESSAGE, ERR_KEY_LENGTH);
#endif
    return false;
  }
  sendMacSet(MAC_DEVADDR, devAddr);
  sendMacSet(MAC_NWKSKEY, nwkSKey);
  sendMacSet(MAC_APPSKEY, appSKey);
  return personalize();
}

bool TheThingsNetwork::personalize()
{
  //configureChannels(fsb);
  configureEU868();
  setSF(sf);
  sendJoinSet(MAC_JOIN_MODE_ABP);
  readLine(buffer, sizeof(buffer));
  if (pgmstrcmp(buffer, CMP_ACCEPTED) != 0)
  {
    debugPrintMessage(ERR_MESSAGE, ERR_PERSONALIZE_NOT_ACCEPTED, buffer);
    debugPrintMessage(ERR_MESSAGE, ERR_CHECK_CONFIGURATION);
    return false;
  }

  readResponse(MAC_TABLE, MAC_CH_TABLE, MAC_CHANNEL_STATUS, buffer, sizeof(buffer));
  debugPrintMessage(SUCCESS_MESSAGE, SCS_PERSONALIZE_ACCEPTED, buffer);
  return true;
}

bool TheThingsNetwork::provision(const char *appEui, const char *appKey, bool resetFirst)
{
  if (resetFirst) {
    reset(adr);
  }
  if (strlen(appEui) != 16 || strlen(appKey) != 32)
  {
    debugPrintMessage(ERR_MESSAGE, ERR_KEY_LENGTH);
    return false;
  }
  readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_HWEUI, buffer, sizeof(buffer));
  sendMacSet(MAC_DEVEUI, buffer);
  sendMacSet(MAC_APPEUI, appEui);
  sendMacSet(MAC_APPKEY, appKey);
  saveState();
  return true;
}
bool TheThingsNetwork::provision(const char *devEui, const char *appEui, const char appKey)
{
  reset(adr);
  if (strlen(appEui) != 16 || strlen(appKey) != 32)
  {
    debugPrintMessage(ERR_MESSAGE, ERR_KEY_LENGTH);
    return false;
  }
  sendMacSet(MAC_DEVEUI, devEui);
  sendMacSet(MAC_APPEUI, appEui);
  sendMacSet(MAC_APPKEY, appKey);
  saveState();
  return true;
}

bool TheThingsNetwork::join(const char *devEui, const char *appEui, const char *appKey, int8_t retries = -1, uint32_t retryDelay = 10000)
{
  return provision(devEui, appEui, appKey) && join(retries, retryDelay);
}
bool TheThingsNetwork::join(int8_t retries, uint32_t retryDelay)
{
  int8_t attempts = 0;
  //configureChannels(fsb); // Not neccessary with one region
  configureEU868();
  setSF(sf);
  while (retries == -1 || attempts <= retries)
  {
    attempts++;
    if (!sendJoinSet(MAC_JOIN_MODE_OTAA))
    {
      debugPrintMessage(ERR_MESSAGE, ERR_JOIN_FAILED);
      delay(retryDelay);
      continue;
    }
    readLine(buffer, sizeof(buffer));
    if (pgmstrcmp(buffer, CMP_ACCEPTED) != 0)
    {
      debugPrintMessage(ERR_MESSAGE, ERR_JOIN_NOT_ACCEPTED, buffer);
      debugPrintMessage(ERR_MESSAGE, ERR_CHECK_CONFIGURATION);
      delay(retryDelay);
      continue;
    }
    readResponse(MAC_TABLE, MAC_CH_TABLE, MAC_CHANNEL_STATUS, buffer, sizeof(buffer));
    debugPrintMessage(SUCCESS_MESSAGE, SCS_JOIN_ACCEPTED, buffer);
    readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_DEVADDR, buffer, sizeof(buffer));
    debugPrintIndex(SHOW_DEVADDR, buffer);
    return true;
  }
  return false;
}
// Only CLASS A is used and iets made the default
/*
  bool TheThingsNetwork::setClass(lorawan_class_t p_lw_class)
  {
  switch (p_lw_class)
  {

    case CLASS_A:
      {
        lw_class = p_lw_class;
        return sendMacSet(MAC_CLASS, "a");
      }

    // case CLASS_B: // Not yet supported. Use default case.

    // case CLASS_C:
    //   {
    //     bool result = sendMacSet(MAC_CLASS, "c");
    //     // Older firmware does not support Class C. If setting change fails, keep on using Class A.
    //     if(result) lw_class = p_lw_class;
    //     return result;
    //   }

    default:
      return false;

  }
  }
*/
bool TheThingsNetwork::join(const char *appEui, const char *appKey, int8_t retries, uint32_t retryDelay)
{
  return provision(appEui, appKey) && join(retries, retryDelay) && lw_class;
}

ttn_response_t TheThingsNetwork::parseBytes() {
  if (buffer[0] == '\0')
    return TTN_UNSUCCESSFUL_RECEIVE;

  if (pgmstrcmp(buffer, CMP_MAC_RX) == 0)
  {
    port_t downlinkPort = receivedPort(buffer + 7);
    char *data = buffer + 7 + digits(downlinkPort) + 1;
    size_t downlinkLength = strlen(data) / 2;
    if (downlinkLength > 0)
    {
      uint8_t downlink[downlinkLength];
      for (size_t i = 0, d = 0; i < downlinkLength; i++, d += 2)
      {
        downlink[i] = TTN_HEX_PAIR_TO_BYTE(data[d], data[d + 1]);
      }
      debugPrintMessage(SUCCESS_MESSAGE, SCS_SUCCESSFUL_TRANSMISSION_RECEIVED, data);
      if (messageCallback)
      {
        messageCallback(downlink, downlinkLength, downlinkPort);
      }
    }
    else
    {
      debugPrintMessage(SUCCESS_MESSAGE, SCS_SUCCESSFUL_TRANSMISSION);
    }
    return TTN_SUCCESSFUL_RECEIVE;
  }
  // not REC but buffer FULL
  debugPrintMessage(ERR_MESSAGE, ERR_UNEXPECTED_RESPONSE, buffer);
  return TTN_ERROR_UNEXPECTED_RESPONSE;
}

ttn_response_t TheThingsNetwork::sendBytes(const uint8_t *payload, size_t length, port_t port, bool confirm, uint8_t sf)
{
  if (sf != 0)
  {
    setSF(sf);
  }

  uint8_t mode = confirm ? MAC_TX_TYPE_CNF : MAC_TX_TYPE_UCNF;
  if (!sendPayload(mode, port, (uint8_t *)payload, length))
  {
    debugPrintMessage(ERR_MESSAGE, ERR_SEND_COMMAND_FAILED);
    return TTN_ERROR_SEND_COMMAND_FAILED;
  }

  // read modem response
  if (!readLine(buffer, sizeof(buffer)) && confirm) // Read response
    // confirmed and RX timeout -> ask to poll if necessary
    return TTN_UNSUCCESSFUL_RECEIVE;

  // TX only?
  if (pgmstrcmp(buffer, CMP_MAC_TX_OK) == 0)
  {
    debugPrintMessage(SUCCESS_MESSAGE, SCS_SUCCESSFUL_TRANSMISSION);
    return TTN_SUCCESSFUL_TRANSMISSION;
  }
  else if (pgmstrcmp(buffer, CMP_MAC_ERR) == 0)
    return TTN_UNSUCCESSFUL_RECEIVE;

  // Received downlink message?
  return parseBytes();
}

ttn_response_t TheThingsNetwork::poll(port_t port, bool confirm, bool modem_only)
{
  // switch (lw_class)
  // {
  // Class A is used and the only MUST Class that needs to be implemented
  //  case CLASS_A:
  if (!modem_only)
  {
    // Class A: send uplink and wait for rx windows
    uint8_t payload[] = {0x00};
    return sendBytes(payload, 1, port, confirm);
  }
  else
  {
    if (!readLine(buffer, sizeof(buffer)) && confirm) // Read response
      // confirmed and RX timeout -> ask to poll if necessary
      return TTN_UNSUCCESSFUL_RECEIVE;

    // Here we can have the result of pending TX, or pending RX (for confirmed messages)
    if (pgmstrcmp(buffer, CMP_MAC_TX_OK) == 0)
    {
      debugPrintMessage(SUCCESS_MESSAGE, SCS_SUCCESSFUL_TRANSMISSION);
      return TTN_SUCCESSFUL_TRANSMISSION;
    }
    else if (pgmstrcmp(buffer, CMP_MAC_ERR) == 0)
      return TTN_UNSUCCESSFUL_RECEIVE;

    // Receive Message
    return parseBytes();
  }


  // case CLASS_C:
  //   {
  // 	  // Class C: check rx buffer for any received data
  // 	  memset(buffer, 0, sizeof(buffer));

  // 	  uint32_t timeout = this->modemStream->getTimeout();
  // 	  this->modemStream->setTimeout(100);
  // 	  this->modemStream->readBytesUntil('\n', buffer, sizeof(buffer));
  // 	  this->modemStream->setTimeout(timeout);

  // 	  return parseBytes();
  //   }
  /*
      default:
        // case CLASS_B: // Not yet supported. Use default case.
        return TTN_UNSUCCESSFUL_RECEIVE;
  */
  //}
}

void TheThingsNetwork::showStatus()
{
  readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_HWEUI, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_EUI, buffer);
  readResponse(SYS_TABLE, SYS_TABLE, SYS_GET_VDD, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_BATTERY, buffer);
  readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_APPEUI, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_APPEUI, buffer);
  readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_DEVEUI, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_DEVEUI, buffer);
  readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_DR, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_DATA_RATE, buffer);
  readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_RXDELAY1, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_RX_DELAY_1, buffer);
  readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_RXDELAY2, buffer, sizeof(buffer));
  debugPrintIndex(SHOW_RX_DELAY_2, buffer);
}


// bool TheThingsNetwork::checkValidModuleConnected(bool autoBaudFirst)
// {
//   // check if we want to autobaud first
//   if(autoBaudFirst)
//   {
//     autoBaud();
//   }
//   // send "sys get ver" to check if (and what) module is connected
//   getVersion(buffer, sizeof(buffer));
//   // check if we got a response (whatever it might be)
//   // needsHardReset flag is set by readLine() (called at some point down the line by getVersion())
//   if(this->needsHardReset)
//   {
//     return false;                                               // no response
//   }
//   // buffer contains "RN2xx3[xx] x.x.x ...", getting only model (RN2xx3[xx])
//   char *model = strtok(buffer, " ");
//   debugPrintIndex(SHOW_MODEL, model);
//   // check if module is valid (must be RN2483, RN2483A, RN2903 or RN2903AS)
//   //if(pgmstrcmp(model, CMP_RN2483) == 0 || pgmstrcmp(model, CMP_RN2483A) == 0 || pgmstrcmp(model, CMP_RN2903) == 0 || pgmstrcmp(model, CMP_RN2903AS) == 0)
//  if(pgmstrcmp(model, CMP_RN2483) == 0)// Changed because the others are not in use
//   {
//     debugPrintMessage(SUCCESS_MESSAGE, SCS_VALID_MODULE);
//     return true;                                                // module responded and is valid (recognized/supported)
//   }
//   debugPrintMessage(ERR_MESSAGE, ERR_INVALID_MODULE);
//   return false;                                                 // module responded but is invalid (unrecognized/unsupported)
// }

void TheThingsNetwork::configureEU868()
{
  sendMacSet(MAC_RX2, 3, 869525000);
  sendChSet(MAC_CHANNEL_DRRANGE, 1, "0 6");

  char buf[10];
  uint32_t freq = 867100000;
  uint8_t ch;
  for (ch = 0; ch < 8; ch++)
  {
    if (ch > 2)
    {
      sprintf(buf, "%lu", freq);
      sendChSet(MAC_CHANNEL_FREQ, ch, buf);
      sendChSet(MAC_CHANNEL_DRRANGE, ch, "0 5");
      sendChSet(MAC_CHANNEL_DCYCLE, ch, 499); // 5*0.2% ETSI band G1, total 1%
      setChannelStatus(ch, true);;
      freq = freq + 200000;
    }
    else
      sendChSet(MAC_CHANNEL_DCYCLE, ch, 299); // 3*0.33% ETSI band G, total 1%
  }
  sendMacSet(MAC_PWRIDX, TTN_PWRIDX_EU868);
}

// void TheThingsNetwork::configureUS915(uint8_t fsb)
// {
//   uint8_t ch;
//   uint8_t chLow = fsb > 0 ? (fsb - 1) * 8 : 0;
//   uint8_t chHigh = fsb > 0 ? chLow + 7 : 71;
//   uint8_t ch500 = fsb + 63;
//   for (ch = 0; ch < 72; ch++)
//   {
//     if (ch == ch500 || (ch <= chHigh && ch >= chLow))
//     {
//       setChannelStatus(ch, true);
//       if (ch < 63)
//       {
//         sendChSet(MAC_CHANNEL_DRRANGE, ch, "0 3");
//       }
//     }
//     else
//     {
//     	setChannelStatus(ch, false);
//     }
//   }
//   sendMacSet(MAC_PWRIDX, TTN_PWRIDX_US915);
// }

// void TheThingsNetwork::configureAU915(uint8_t fsb)
// {
//   uint8_t ch;
//   uint8_t chLow = fsb > 0 ? (fsb - 1) * 8 : 0;
//   uint8_t chHigh = fsb > 0 ? chLow + 7 : 71;
//   uint8_t ch500 = fsb + 63;
//   for (ch = 0; ch < 72; ch++)
//   {
//     if (ch == ch500 || (ch <= chHigh && ch >= chLow))
//     {
//       setChannelStatus(ch, true);
//       if (ch < 63)
//       {
//         sendChSet(MAC_CHANNEL_DRRANGE, ch, "0 3");
//       }
//     }
//     else
//     {
//     	setChannelStatus(ch, false);
//     }
//   }
//   sendMacSet(MAC_PWRIDX, TTN_PWRIDX_AU915);
// }

// void TheThingsNetwork::configureAS920_923()
// {
//   /* RN2903AS 1.0.3rc9 defaults
//    * CH0 = 923.2MHz
//    * CH1 = 923.4MHz
//    */
//   setADR(false); // TODO: remove when ADR is implemented for this plan
//   sendMacSet(MAC_RX2, 2, 923200000);

//   char buf[10];
//   uint32_t freq = 922000000;
//   uint8_t ch;
//   for (ch = 0; ch < 8; ch++)
//   {
//     sendChSet(MAC_CHANNEL_DCYCLE, ch, 799);
//     if (ch > 1)
//     {
//       sprintf(buf, "%lu", freq);
//       sendChSet(MAC_CHANNEL_FREQ, ch, buf);
//       sendChSet(MAC_CHANNEL_DRRANGE, ch, "0 5");
//       setChannelStatus(ch, true);
//       freq = freq + 200000;
//     }
//   }
//   // TODO: SF7BW250/DR6 channel, not properly supported by RN2903AS yet
//   //sendChSet(MAC_CHANNEL_DCYCLE, 8, 799);
//   //sendChSet(MAC_CHANNEL_FREQ, 8, 922100000);
//   //sendChSet(MAC_CHANNEL_DRRANGE, 8, "6 6");
//   //setChannelStatus(8, true);
//   // TODO: Add FSK channel on 921800000
//   sendMacSet(MAC_PWRIDX, TTN_PWRIDX_AS920_923);
// }

// void TheThingsNetwork::configureAS923_925()
// {
//   /* RN2903AS 1.0.3rc9 defaults
//    * CH0 = 923.2MHz
//    * CH1 = 923.4MHz
//    */
//   setADR(false); // TODO: remove when ADR is implemented for this plan
//   sendMacSet(MAC_RX2, 2, 923200000);

//   char buf[10];
//   uint32_t freq = 923600000;
//   uint8_t ch;
//   for (ch = 0; ch < 8; ch++)
//   {
//     sendChSet(MAC_CHANNEL_DCYCLE, ch, 799);
//     if (ch > 1)
//     {
//       sprintf(buf, "%lu", freq);
//       sendChSet(MAC_CHANNEL_FREQ, ch, buf);
//       sendChSet(MAC_CHANNEL_DRRANGE, ch, "0 5");
//       setChannelStatus(ch, true);
//       freq = freq + 200000;
//     }
//   }
//   // TODO: SF7BW250/DR6 channel, not properly supported by RN2903AS yet
//   //sendChSet(MAC_CHANNEL_DCYCLE, 8, 799);
//   //sendChSet(MAC_CHANNEL_FREQ, 8, 924500000);
//   //sendChSet(MAC_CHANNEL_DRRANGE, 8, "6 6");
//   //setChannelStatus(8, true);
//   // TODO: Add FSK channel on 924800000
//   sendMacSet(MAC_PWRIDX, TTN_PWRIDX_AS923_925);
// }

// void TheThingsNetwork::configureKR920_923()
// {
//   setADR(false); // TODO: remove when ADR is implemented for this plan
//   sendMacSet(MAC_RX2, 0, 921900000); // KR still uses SF12 for now. Might change to SF9 later.

//   //disable two default LoRaWAN channels
//   setChannelStatus(0, false);
//   setChannelStatus(1, false);

//   char buf[10];
//   uint32_t freq = 922100000;
//   uint8_t ch;
//   for (ch = 2; ch < 9; ch++)
//   {
//     sendChSet(MAC_CHANNEL_DCYCLE, ch, 799);
//     sprintf(buf, "%lu", freq);
//     sendChSet(MAC_CHANNEL_FREQ, ch, buf);
//     sendChSet(MAC_CHANNEL_DRRANGE, ch, "0 5");
//     setChannelStatus(ch, true);
//     freq = freq + 200000;
//   }
//   sendMacSet(MAC_PWRIDX, TTN_PWRIDX_KR920_923);
// }

// void TheThingsNetwork::configureIN865_867()
// {
//   setADR(false); // TODO: remove when ADR is implemented for this plan
//   sendMacSet(MAC_RX2, 2, 866550000); // SF10

//   // Disable the three default LoRaWAN channels
//   setChannelStatus(0, false);
//   setChannelStatus(1, false);
//   setChannelStatus(2, false);

//   // Channel 3
//   sendChSet(MAC_CHANNEL_DCYCLE, 3, 299);
//   sendChSet(MAC_CHANNEL_FREQ, 3, 865062500);
//   sendChSet(MAC_CHANNEL_DRRANGE, 3, "0 5");
//   setChannelStatus(3, true);

//   // Channel 4
//   sendChSet(MAC_CHANNEL_DCYCLE, 4, 299);
//   sendChSet(MAC_CHANNEL_FREQ, 4, 865402500);
//   sendChSet(MAC_CHANNEL_DRRANGE, 4, "0 5");
//   setChannelStatus(4, true);

//   // Channel 5
//   sendChSet(MAC_CHANNEL_DCYCLE, 5, 299);
//   sendChSet(MAC_CHANNEL_FREQ, 5, 865985000);
//   sendChSet(MAC_CHANNEL_DRRANGE, 5, "0 5");
//   setChannelStatus(5, true);

//   sendMacSet(MAC_PWRIDX, TTN_PWRIDX_IN865_867);
// }


// We could delete this function, because one channel is used
/*void TheThingsNetwork::configureChannels(uint8_t fsb)
  {
  switch (fp)
  {
  case TTN_FP_EU868:
    configureEU868();
    break;
  // case TTN_FP_US915:
  //   configureUS915(fsb);
  //   break;
  // case TTN_FP_AU915:
  //   configureAU915(fsb);
  //   break;
  // case TTN_FP_AS920_923:
  //   configureAS920_923();
  //   break;
  // case TTN_FP_AS923_925:
  //   configureAS923_925();
  //   break;
  // case TTN_FP_KR920_923:
  //   configureKR920_923();
  //   break;
  // case TTN_FP_IN865_867:
  //   configureIN865_867();
  //   break;
  default:
    debugPrintMessage(ERR_MESSAGE, ERR_INVALID_FP);
    break;
  }
  sendMacSet(MAC_RETX, TTN_RETX);
  }
*/

/* Next two functions are not used in the code, as far as i know. So therefore deleted
  // bool TheThingsNetwork::setChannel(uint8_t channel, uint32_t frequency, uint8_t dr_min, uint8_t dr_max){

  //   bool done = true;

  //   if (channel > 15)
  // 	  return false;

  //   if (frequency){
  // 	char buf[11];
  // 	sprintf(buf, "%lu", frequency);
  // 	done &= sendChSet(MAC_CHANNEL_FREQ, channel, buf);
  //   }

  //   if (done && (dr_min < 16) && (dr_max < 16)){
  // 	char buf[11];
  // 	sprintf(buf, "%u %u", dr_min, dr_max);
  // 	done &= sendChSet(MAC_CHANNEL_DRRANGE, channel, buf);
  //   }
  //   else
  // 	done &= (dr_min == 255) && (dr_max == 255);

  //   return done;
  // }


  // bool TheThingsNetwork::setRx2Channel(uint32_t frequency, uint8_t dr){

  //   char buf[15];
  //   sprintf(buf, "%u %lu", dr, frequency);
  //   return sendMacSet(MAC_RX2, buf);
  // }
*/

bool TheThingsNetwork::setChannelStatus (uint8_t channel, bool status) {
  if (status)
    return sendChSet(MAC_CHANNEL_STATUS, channel, "on");
  else
    return sendChSet(MAC_CHANNEL_STATUS, channel, "off");
}

// bool TheThingsNetwork::setChannelDCycle (uint8_t channel, float duty_cycle){ // in percent
//   if (channel > 15 || duty_cycle > 100.0 || duty_cycle < 0.0)
// 	return false;

//   char buf[6]; // number 99999
//   if (0.0 == duty_cycle)
// 	  (void)sprintf(buf, "%u", 65535u);
//   else
// 	  (void)sprintf(buf, "%u", (uint16_t)((100.0/duty_cycle) - 1));

//   return sendChSet(MAC_CHANNEL_DCYCLE, channel, buf);
// }

// bool TheThingsNetwork::setPowerIndex(uint8_t index){
//   char buf[4];
//   sprintf(buf, "%u",index);
//   return sendMacSet(MAC_PWRIDX, buf);
// }

// bool TheThingsNetwork::setDR(uint8_t dr){
//   char buf[4];
//   sprintf(buf, "%u",dr);
//   return sendMacSet(MAC_DR, buf);
// }

bool TheThingsNetwork::setADR(bool adr) {
  bool ret;
  if (adr)
  {
    ret = sendMacSet(MAC_ADR, "on");
  }
  else
  {
    ret = sendMacSet(MAC_ADR, "off");
  }
  this->adr = adr;
  return ret;
}

bool TheThingsNetwork::setSF(uint8_t sf)
{
  //sf default = 7, maybe this function can be reduced more( you can find the define in the file TheThingsNetwork.h line 7)
  uint8_t dr;
  // The switch below is not in use because we use only the case of EU868
  /*switch (fp)
    {
    case TTN_FP_EU868:
    case TTN_FP_IN865_867:
    case TTN_FP_AS920_923:
    case TTN_FP_AS923_925:
    case TTN_FP_KR920_923:
    default:
    dr = 12 - sf;
    break;
    case TTN_FP_US915:
    case TTN_FP_AU915:
    dr = 10 - sf;
    break;
    }*/
  dr = 12 - sf;
  char s[2];
  s[0] = '0' + dr;
  s[1] = '\0';
  return sendMacSet(MAC_DR, s);
}

/* Next three functions not used
  bool TheThingsNetwork::setRX1Delay(uint16_t delay){
	  char buf[6];
	  sprintf(buf, "%u",delay);
	  return sendMacSet(MAC_RXDELAY1, buf);
  }

  bool TheThingsNetwork::setFCU(uint32_t fcu){
  char buf[10];
  sprintf(buf, "%lu", fcu);
  return sendMacSet(MAC_UPCTR, buf);
  }

  bool TheThingsNetwork::setFCD(uint32_t fcd){
  char buf[10];
  sprintf(buf, "%lu", fcd);
  return sendMacSet(MAC_DNCTR, buf);
  }
*/

void TheThingsNetwork::sendCommand(uint8_t table, uint8_t index, bool appendSpace, bool print)
{
  char command[100];
  switch (table)
  {
    case MAC_TABLE:
      strcpy_P(command, (char *)pgm_read_word(&(mac_table[index])));
      break;
    case MAC_GET_SET_TABLE:
      strcpy_P(command, (char *)pgm_read_word(&(mac_options[index])));
      break;
    case MAC_JOIN_TABLE:
      strcpy_P(command, (char *)pgm_read_word(&(mac_join_mode[index])));
      break;
    case MAC_CH_TABLE:
      strcpy_P(command, (char *)pgm_read_word(&(mac_ch_options[index])));
      break;
    case MAC_TX_TABLE:
      strcpy_P(command, (char *)pgm_read_word(&(mac_tx_table[index])));
      break;
    case SYS_TABLE:
      strcpy_P(command, (char *)pgm_read_word(&(sys_table[index])));
      break;
    // case RADIO_TABLE:
    //   strcpy_P(command, (char *)pgm_read_word(&(radio_table[index])));
    //   break;
    default:
      return;
  }
  modemStream->write(command);
  if (appendSpace)
  {
    modemStream->write(" ");
  }
#if defined(YES_DEBUG)
  if (print)
  {
    debugPrint(command);
    debugPrint(F(" "));
  }
#endif
}

bool TheThingsNetwork::sendMacSet(uint8_t index, uint8_t value1, unsigned long value2)
{
#if defined(YES_DEBUG)
  char buf[15];
  sprintf(buf, "%u %lu", value1, value2);
  return sendMacSet(index, buf);
#else
  // Do nothing, debug off
#endif
}

bool TheThingsNetwork::sendMacSet(uint8_t index, const char *value)
{
  clearReadBuffer();
#if defined(YES_DEBUG)
  debugPrint(SENDING);
#endif
  sendCommand(MAC_TABLE, MAC_PREFIX, true);
  sendCommand(MAC_TABLE, MAC_SET, true);
  sendCommand(MAC_GET_SET_TABLE, index, true);
  modemStream->write(value);
  modemStream->write(SEND_MSG);
#if defined(YES_DEBUG)
  debugPrintLn(value);
#endif
  return waitForOk();
}

bool TheThingsNetwork::waitForOk()
{
  readLine(buffer, sizeof(buffer));
  if (pgmstrcmp(buffer, CMP_OK) != 0)
  {
    debugPrintMessage(ERR_MESSAGE, ERR_RESPONSE_IS_NOT_OK, buffer);
    return false;
  }
  return true;
}

bool TheThingsNetwork::sendChSet(uint8_t index, uint8_t channel, unsigned long value)
{
  char buf[11];
  sprintf(buf, "%lu", value);
  return sendChSet(index, channel, buf);
}

bool TheThingsNetwork::sendChSet(uint8_t index, uint8_t channel, const char *value)
{
  clearReadBuffer();
  char ch[5];
  if (channel > 9)
  {
    ch[0] = ((channel - (channel % 10)) / 10) + 48;
    ch[1] = (channel % 10) + 48;
    ch[2] = '\0';
  }
  else
  {
    ch[0] = channel + 48;
    ch[1] = '\0';
  }
#if defined(YES_DEBUG)
  debugPrint(F(SENDING));
#endif
  sendCommand(MAC_TABLE, MAC_PREFIX, true);
  sendCommand(MAC_TABLE, MAC_SET, true);
  sendCommand(MAC_GET_SET_TABLE, MAC_CH, true);
  sendCommand(MAC_CH_TABLE, index, true);
  modemStream->write(ch);
  modemStream->write(" ");
  modemStream->write(value);
  modemStream->write(SEND_MSG);
#if defined(YES_DEBUG)
  debugPrint(channel);
  debugPrint(F(" "));
  debugPrintLn(value);
#endif
  return waitForOk();
}

bool TheThingsNetwork::sendJoinSet(uint8_t type)
{
  clearReadBuffer();
#if defined(YES_DEBUG)
  debugPrint(F(SENDING));
#endif
  sendCommand(MAC_TABLE, MAC_PREFIX, true);
  sendCommand(MAC_TABLE, MAC_JOIN, true);
  sendCommand(MAC_JOIN_TABLE, type, false);
  modemStream->write(SEND_MSG);
  debugPrintLn();
  return waitForOk();
}

bool TheThingsNetwork::sendPayload(uint8_t mode, uint8_t port, uint8_t *payload, size_t length)
{
  clearReadBuffer();
#if defined(YES_DEBUG)
  debugPrint(F(SENDING));
#endif
  sendCommand(MAC_TABLE, MAC_PREFIX, true);
  sendCommand(MAC_TABLE, MAC_TX, true);
  sendCommand(MAC_TX_TABLE, mode, true);
  char sport[4];
  if (port > 99)
  {
    sport[0] = ((port - (port % 100)) / 100) + 48;
    sport[1] = (((port % 100) - (port % 10)) / 10) + 48;
    sport[2] = (port % 10) + 48;
    sport[3] = '\0';
  }
  else if (port > 9)
  {
    sport[0] = ((port - (port % 10)) / 10) + 48;
    sport[1] = (port % 10) + 48;
    sport[2] = '\0';
  }
  else
  {
    sport[0] = port + 48;
    sport[1] = '\0';
  }
  modemStream->write(sport);
  modemStream->print(" ");
#if defined(YES_DEBUG)
  debugPrint(sport);
  debugPrint(F(" "));
#endif
  uint8_t i = 0;
  for (i = 0; i < length; i++)
  {
    if (payload[i] < 16)
    {
      modemStream->print("0");
      modemStream->print(payload[i], HEX);
#if defined(YES_DEBUG)
      debugPrint(F("0"));
      debugPrint(payload[i], HEX);
#endif
    }
    else
    {
      modemStream->print(payload[i], HEX);
#if defined(YES_DEBUG)
      debugPrint(payload[i], HEX);
#endif
    }
  }
  modemStream->write(SEND_MSG);
  debugPrintLn();
  return waitForOk();
}

void TheThingsNetwork::sleep(uint32_t mseconds)
{
  if (mseconds < 100)
  {
    return;
  }

#if defined(YES_DEBUG)
  debugPrint(F(SENDING));
#endif
  sendCommand(SYS_TABLE, SYS_PREFIX, true);
  sendCommand(SYS_TABLE, SYS_SLEEP, true);

  sprintf(buffer, "%lu", mseconds);
  modemStream->write(buffer);
  modemStream->write(SEND_MSG);
#if defined(YES_DEBUG)
  debugPrintLn(buffer);
#endif
}

void TheThingsNetwork::wake()
{
  autoBaud();
}

void TheThingsNetwork::linkCheck(uint16_t seconds)
{
  clearReadBuffer();
#if defined(YES_DEBUG)
  debugPrint(SENDING);
#endif
  sendCommand(MAC_TABLE, MAC_PREFIX, true);
  sendCommand(MAC_TABLE, MAC_SET, true);
  sendCommand(MAC_GET_SET_TABLE, MAC_LINKCHK, true);

  sprintf(buffer, "%u", seconds);
  modemStream->write(buffer);
  modemStream->write(SEND_MSG);
#if defined(YES_DEBUG)
  debugPrintLn(buffer);
#endif
  waitForOk();
}

uint8_t TheThingsNetwork::getLinkCheckGateways()
{
  if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_GWNB, buffer, sizeof(buffer))) {
    char **endptr = NULL;
    uint8_t gwnb = strtol(buffer, endptr, 10);
    if (endptr == NULL)
      return gwnb;
  }
  return 0; // Gateway number defaults to 0
}

uint8_t TheThingsNetwork::getLinkCheckMargin()
{
  if (readResponse(MAC_TABLE, MAC_GET_SET_TABLE, MAC_MRGN, buffer, sizeof(buffer))) {
    char **endptr = NULL;
    uint8_t mgn = strtol(buffer, endptr, 10);
    if (endptr == NULL)
      return mgn;
  }
  return 255; // Signal margin defaults to 255
}
