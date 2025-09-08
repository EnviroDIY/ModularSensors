/**
 * @file ModSensorDebugConfig.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief This file is used to configure the *code* debugging to be used by
 * the Modular Sensors Library.
 *
 * The debugging functionality provided within the library is mostly useful for
 * testing the workings of the library itself. It may not help to find problems
 * with misbehaving sensors.
 *
 * For the Arduino IDE, this is the only way to configure these settings. If you
 * are using PlatformIO, you have the option of using this file or changing your
 * configuration using build flags in your platformio.ini. Compile times will be
 * shorter if you use this file instead of build flags, but it is much easier to
 * track changes in your platformio.ini file than in this file.
 */

// Header Guards
#ifndef SRC_MODSENSORDEBUGCONFIG_H_
#define SRC_MODSENSORDEBUGCONFIG_H_

//==============================================================
// Configure the output streams for debugging
// If these are not defined, most output will go to either "Serial" or
// "SerialUSB" and there will be no secondary output.
/// The primary output stream for printouts.
#define MS_OUTPUT Serial
// #define MS_2ND_OUTPUT Serial2
// #define TINY_GSM_DEBUG Serial
// #define TINY_GSM_DEBUG_DEEP Serial1
// Or use this to turn of *ALL* output
// #define MS_SILENT
//==============================================================


//==============================================================
// Uncomment any debugging options you might want to use

// #define MS_LOGGERBASE_DEBUG
// #define MS_LOGGERBASE_DEBUG_DEEP

// #define MS_CLOCKSUPPORT_DEBUG
// #define MS_CLOCKSUPPORT_DEBUG_DEEP

// #define MS_WATCHDOGAVR_DEBUG
// #define MS_WATCHDOGAVR_DEBUG_DEEP
// #define MS_WATCHDOGSAMD_DEBUG
// #define MS_WATCHDOGSAMD_DEBUG_DEEP

// #define MS_SENSORBASE_DEBUG
// #define MS_VARIABLEBASE_DEBUG
// #define MS_VARIABLEARRAY_DEBUG
// #define MS_VARIABLEARRAY_DEBUG_DEEP

// #define MS_DATAPUBLISHERBASE_DEBUG
// #define MS_DATAPUBLISHERBASE_DEBUG_DEEP

// #define MS_ENVIRODIYPUBLISHER_DEBUG
// #define MS_THINGSPEAKPUBLISHER_DEBUG
// #define MS_UBIDOTSPUBLISHER_DEBUG
// #define MS_DREAMHOSTPUBLISHER_DEBUG
// #define MS_AWS_IOT_PUBLISHER_DEBUG
// #define MS_S3PRESIGNEDPUBLISHER_DEBUG

// #define MS_LOGGERMODEM_DEBUG
// #define MS_LOGGERMODEM_DEBUG_DEEP

// #define MS_DIGIXBEE_DEBUG
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
// #define MS_DIGIXBEE3GBYPASS_DEBUG
// #define MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
// #define MS_DIGIXBEELTEBYPASS_DEBUG
// #define MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
// #define MS_DIGIXBEEWIFI_DEBUG
// #define MS_DIGIXBEEWIFI_DEBUG_DEEP

// #define MS_ESPRESSIFESP8266_DEBUG
// #define MS_ESPRESSIFESP8266_DEBUG_DEEP

// #define MS_ESPRESSIFESP32_DEBUG
// #define MS_ESPRESSIFESP32_DEBUG_DEEP

// #define MS_SIMCOMSIM7080_DEBUG
// #define MS_SIMCOMSIM7080_DEBUG_DEEP

// #define MS_QUECTELBG96_DEBUG
// #define MS_QUECTELBG96_DEBUG_DEEP
// #define MS_SEQUANSMONARCH_DEBUG
// #define MS_SEQUANSMONARCH_DEBUG_DEEP
// #define MS_SIMCOMSIM800_DEBUG
// #define MS_SIMCOMSIM800_DEBUG_DEEP
// #define MS_SIMCOMSIM7000_DEBUG
// #define MS_SIMCOMSIM7000_DEBUG_DEEP
// #define MS_SODAQ2GBEER6_DEBUG
// #define MS_SODAQ2GBEER6_DEBUG_DEEP
// #define MS_SODAQUBEER410M_DEBUG
// #define MS_SODAQUBEER410M_DEBUG_DEEP
// #define MS_SODAQUBEEU201_DEBUG
// #define MS_SODAQUBEEU201_DEBUG_DEEP

// #define MS_ALPHASENSECO2_DEBUG
// #define MS_ANALOGELECCONDUCTIVITY_DEBUG
// #define MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
// #define MS_ANB_SENSORS_PH_DEBUG
// #define MS_ANB_SENSORS_PH_DEBUG_DEEP
// #define MS_AOSONGAM2315_DEBUG
// #define MS_AOSONGDHT_DEBUG
// #define MS_APOGEESQ212_DEBUG
// #define MS_BOSCHBME280_DEBUG
// #define MS_BOSCHBMP3XX_DEBUG
// #define MS_CAMPBELLOBS3_DEBUG
// #define MS_EVERLIGHTALSPT19_DEBUG
// #define MS_FREESCALEMPL115A2_DEBUG

// #define MS_GEOLUXHYDROCAM_DEBUG
// #define GEOLUX_DEBUG Serial2

// #define MS_MAXBOTIXSONAR_DEBUG
// #define MS_MAXIMDS18_DEBUG
// #define MS_MAXIMDS3231_DEBUG
// #define MS_MEASPECMS5803_DEBUG
// #define MS_PALEOTERRAREDOX_DEBUG
// #define MS_PROCESSORSTATS_DEBUG
// #define MS_RAINCOUNTERI2C_DEBUG
// #define MS_SENSIRION_SHT4X_DEBUG
// #define MS_TALLYCOUNTERI2C_DEBUG
// #define MS_TIADS1X15_DEBUG
// #define MS_TIINA219_DEBUG
// #define MS_TURNERCYCLOPS_DEBUG
// #define MS_TURNERTURBIDITYPLUS_DEBUG

// #define MS_SDI12SENSORS_DEBUG
// #define MS_SDI12SENSORS_DEBUG_DEEP
// #define MS_DECAGON5TM_DEBUG
// #define MS_METERTEROS11_DEBUG

// #define MS_ATLASPARENT_DEBUG
// #define MS_ATLASSCIENTIFICCO2_DEBUG
// #define MS_ATLASSCIENTIFICDO_DEBUG
// #define MS_ATLASSCIENTIFICEC_DEBUG

// #define MS_GROPOINTPARENT_DEBUG
// #define MS_GROPOINTPARENT_DEBUG_DEEP

// #define MS_KELLERPARENT_DEBUG
// #define MS_KELLERPARENT_DEBUG_DEEP

// #define MS_YOSEMITECHPARENT_DEBUG
// #define MS_YOSEMITECHPARENT_DEBUG_DEEP

//==============================================================

#endif  // SRC_MODSENSORDEBUGCONFIG_H_
