/** =========================================================================
 * @file DRWI_SIM7080LTE.ino
 * @brief Example for DRWI CitSci LTE sites.
 *
 * This example shows proper settings for the following configuration:
 *
 * Mayfly v1.0 board
 * EnviroDIY SIM7080 LTE module (with Hologram SIM card)
 * Hydros21 CTD sensor
 * Campbell Scientific OBS3+ Turbidity sensor
 *
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @copyright (c) 2017-2022 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 *

 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

// ==========================================================================
//  Defines for the Arduino IDE
//  NOTE:  These are ONLY needed to compile with the Arduino IDE.
//         If you use PlatformIO, you should set these build flags in your
//         platformio.ini
// ==========================================================================
/** Start [defines] */
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
/** End [defines] */

// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>
/** End [includes] */


// ==========================================================================
//  Data Logging Options
// ==========================================================================
// Details for this build
extern const String build_ref = "a\\" __FILE__ " " __DATE__ " " __TIME__ " ";
#ifdef PIO_SRC_REV
const char git_branch[] = PIO_SRC_REV;
#else
const char git_branch[] = "brnch";
#endif
#ifdef PIO_SRC_USR
const char git_usr[] = PIO_SRC_USR;
#else
const char git_usr[] = "usr";
#endif
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "DRWI_SIM7080LTE.cpp";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 2;
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 115200;  // 57600 Baud rate for debugging 
const int8_t  greenLED   = 8;      // Pin for the green LED
const int8_t  redLED     = 9;      // Pin for the red LED
const int8_t  buttonPin  = 21;     // Pin for debugging mode (ie, button pin)
const int8_t  wakePin    = 31;     // MCU interrupt/alarm pin to wake from sleep
// Mayfly 0.x D31 = A7
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power
/** End [logging_options] */


// ==========================================================================
//  Wifi/Cellular Modem Options
// ==========================================================================
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
#if defined STREAMDEBUGGER_DBG
#include <StreamDebugger.h>
StreamDebugger modemDebugger(modemSerial, STANDARD_SERIAL_OUTPUT);
#define modemSerHw modemDebugger
#else
#define modemSerHw modemSerial
#endif  // STREAMDEBUGGER_DBG

#define sim_com_xbee_wifi
#if defined sim_com_sim7080
/** Start [sim_com_sim7080] */

// For almost anything based on the SIMCom SIM7080G
#include <modems/SIMComSIM7080.h>

// Create a reference to the serial port for the modem
const int32_t   modemBaud = 9600;  //  SIM7080 does auto-bauding by default, but
                                   //  for simplicity we set to 9600

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply

const int8_t modemVccPin =
    18;  // MCU pin controlling modem power ---
         //  Pin 18 is the power enable pin
         //  for the bee socket on Mayfly v1.0,
         //  use -1 if using Mayfly 0.5b or if the bee socket is constantly
         //  powered (ie you changed SJ18 on Mayfly 1.x to 3.3v)
const int8_t modemStatusPin  = 19;  // MCU pin used to read modem status
const int8_t modemSleepRqPin = 23;  // MCU pin for modem sleep/wake request
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem
                                    // status

// Network connection information
const char* apn =
    "hologram";  // APN connection name, typically Hologram unless you have a
                 // different provider's SIM card. Change as needed

// Create the modem object
SIMComSIM7080 modem7080(&modemSerHw, modemVccPin, modemStatusPin,
                        modemSleepRqPin, apn);
// Create an extra reference to the modem by a generic name
SIMComSIM7080 modem = modem7080;
/** End [sim_com_sim7080] */
#elif defined sim_com_xbee_wifi
/** Start [sim_com_xbee_wifi] */
// For the Digi Wifi XBee (S6B)
#include <modems/DigiXBeeWifi.h>
// Create a reference to the serial port for the modem

const int32_t   modemBaud   = 9600;     // All XBee's use 9600 by default

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply
const int8_t modemVccPin    = 18;    // Mayfly1.1 pin controlling modem power
const int8_t modemStatusPin = 19;    // MCU pin used to read modem status
const bool useCTSforStatus  = true;  // Flag to use the modem CTS pin for status
const int8_t modemResetPin  = 20;    // MCU pin connected to modem reset pin
const int8_t modemSleepRqPin = 23;   // MCU pin for modem sleep/wake request
const int8_t modemLEDPin = redLED;   // MCU pin connected an LED to show modem
                                     // status (-1 if unconnected)

// Network connection information
const char* wifiId  = "ArthurGuestSsid";  // WiFi access point, unnecessary for GPRS
const char* wifiPwd = "Arthur8166";  // WiFi password, unnecessary for GPRS

DigiXBeeWifi modemXBWF(&modemSerHw, modemVccPin, modemStatusPin,
                       useCTSforStatus, modemResetPin, modemSleepRqPin, wifiId,
                       wifiPwd);
// Create an extra reference to the modem by a generic name
DigiXBeeWifi modemPhy = modemXBWF;
/** End [sim_com_xbee_wifi] */
#endif //Modem options 

// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
/** Start [processor_sensor] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);
/** End [processor_sensor] */


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
/** Start [ds3231] */
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);
/** End [ds3231] */

//#define SENSORS_EXTERNAL
#if defined SENSORS_EXTERNAL
// ==========================================================================
//  Meter Hydros 21 Conductivity, Temperature, and Depth Sensor
// ==========================================================================
/** Start [hydros21] */
#include <sensors/MeterHydros21.h>

const char*   hydrosSDI12address = "1";  // The SDI-12 Address of the Hydros 21
const uint8_t hydrosNumberReadings = 6;  // The number of readings to average
const int8_t  SDI12Power = sensorPowerPin;  // Power pin (-1 if unconnected)
const int8_t  SDI12Data  = 7;               // The SDI12 data pin

// Create a Meter Hydros 21 sensor object
MeterHydros21 hydros(*hydrosSDI12address, SDI12Power, SDI12Data,
                     hydrosNumberReadings);
/** End [hydros21] */


// ==========================================================================
//  Campbell OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
/** Start [obs3] */
#include <sensors/CampbellOBS3.h>

const int8_t  OBS3Power = sensorPowerPin;  // Power pin (-1 if unconnected)
const uint8_t OBS3NumberReadings = 10;
const uint8_t ADSi2c_addr        = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ *Low* Range Calibration in Volts
const int8_t OBSLowADSChannel = 0;  // ADS channel for *low* range output
const float  OBSLow_A         = 0.000E+00;  // "A" value (X^2) [*low* range]
const float  OBSLow_B         = 1.000E+00;  // "B" value (X) [*low* range]
const float  OBSLow_C         = 0.000E+00;  // "C" value [*low* range]

// Create a Campbell OBS3+ *low* range sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowADSChannel, OBSLow_A, OBSLow_B, OBSLow_C,
                     ADSi2c_addr, OBS3NumberReadings);


// Campbell OBS 3+ *High* Range Calibration in Volts
const int8_t OBSHighADSChannel = 1;  // ADS channel for *high* range output
const float  OBSHigh_A         = 0.000E+00;  // "A" value (X^2) [*high* range]
const float  OBSHigh_B         = 1.000E+00;  // "B" value (X) [*high* range]
const float  OBSHigh_C         = 0.000E+00;  // "C" value [*high* range]

// Create a Campbell OBS3+ *high* range sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighADSChannel, OBSHigh_A, OBSHigh_B,
                      OBSHigh_C, ADSi2c_addr, OBS3NumberReadings);
/** End [obs3] */

#endif // SENSORS_EXTERNAL
// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
Variable* variableList[] = {
    #if defined SENSORS_EXTERNAL
    new MeterHydros21_Cond(&hydros),
    new MeterHydros21_Depth(&hydros),
    new MeterHydros21_Temp(&hydros),
    new CampbellOBS3_Turbidity(&osb3low, "", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "", "TurbHigh"),
    new ProcessorStats_Battery(&mcuBoard),
    #endif //SENSORS_EXTERNAL
    new MaximDS3231_Temp(&ds3231),
    new ProcessorStats_SampleNumber(&mcuBoard),
    // Fut Sensiron Temperature
    // Fut Sensirom Humidity 
    //new Modem_SignalPercent(&modem),
};

// All UUID's, device registration, and sampling feature information can be
// pasted directly from Monitor My Watershed.
// To get the list, click the "View  token UUID list" button on the upper right
// of the site page.

// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***
// Check the order of your variables in the variable list!!!
// Be VERY certain that they match the order of your UUID's!
// Rearrange the variables in the variable list ABOVE if necessary to match!
// Do not change the order of the variables in the section below.
// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***

// Replace all of the text in the following section with the UUID array from
// MonitorMyWatershed

/* clang-format off */
// ---------------------   Beginning of Token UUID List   ---------------------

//Site https://monitormywatershed.org/sites/bq_test01/
const char* UUIDs[] =  // UUID array for device sensors
    {
        #if defined SENSORS_EXTERNAL
        "12345678-abcd-1234-ef00-1234567890ab",  // Specific conductance (Meter_Hydros21_Cond)
        "12345678-abcd-1234-ef00-1234567890ab",  // Water depth (Meter_Hydros21_Depth)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Meter_Hydros21_Temp)
        "12345678-abcd-1234-ef00-1234567890ab",  // Turbidity (Campbell_OBS3_Turb) (Low)
        "12345678-abcd-1234-ef00-1234567890ab",  // Turbidity (Campbell_OBS3_Turb) (High)
        "12345678-abcd-1234-ef00-1234567890ab",  // Battery voltage (EnviroDIY_Mayfly_Batt)
        #endif // SENSORS_EXTERNAL
        "9fdcefc1-b43f-4c3c-8d46-ca0e90845153",  // Temperature (Maxim_DS3231_Temp)
        "e0d7b81b-0241-4017-b5dc-e90ecdb7c279",  // Sequence number (EnviroDIY_Mayfly_SampleNum)
        //"d73e060d-df4e-4f29-8b69-34891f518bdf",   // Temperature (Sensirion_SHT40_Temperature)
        //"acc456aa-1148-4385-a984-a68b6eb6b044",   // Relative humidity (Sensirion_SHT40_Humidity)
        //"97893988-6c2d-43ee-9cfe-3715d45019db"    // Percent full scale (Digi_Cellular_SignalPercent)
};
const char* registrationToken = "22752220-5925-4a2c-aeb1-a57b58e1c246";  // Device registration token
const char* samplingFeature   = "747478ef-4e80-4cc8-921e-89172d05ea42";  // Sampling feature UUID


// -----------------------   End of Token UUID List  -----------------------
/* clang-format on */

// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList, UUIDs);
/** End [variable_arrays] */


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create a new logger instance
Logger dataLogger(LoggerID, loggingInterval, &varArray);
/** End [loggers] */


// ==========================================================================
//  Creating Data Publisher[s]
// ==========================================================================
/** Start [publishers] */
// Create a data publisher for the Monitor My Watershed/EnviroDIY POST endpoint
#include <publishers/EnviroDIYPublisher.h>
EnviroDIYPublisher EnviroDIYPOST(dataLogger, &modemPhy.gsmClient,
                                 registrationToken, samplingFeature);
/** End [publishers] */


// ==========================================================================
//  Working Functions
// ==========================================================================
/** Start [working_functions] */
// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75) {
    for (uint8_t i = 0; i < numFlash; i++) {
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
        delay(rate);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        delay(rate);
    }
    digitalWrite(redLED, LOW);
}

// Reads the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage() {
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
}


// ==========================================================================
// Arduino Setup Function
// ==========================================================================
/** Start [setup] */
void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);
    Serial.print(F("\n---Boot. Sw Build: "));
    Serial.print(build_ref);
    Serial.print(" ");
    Serial.println(git_usr);
    Serial.print(" ");
    Serial.println(git_branch);

    // Print a start-up note to the first serial port
    Serial.print(F("\nNow running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);
    Serial.println();

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);
    Serial.print(F("TinyGSM Library version "));
    Serial.println(TINYGSM_VERSION);
    Serial.println();

    // Start the serial connection with the modem
    modemSerial.begin(modemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    pinMode(20, OUTPUT);  // for proper operation of the onboard flash memory
                          // chip's ChipSelect (Mayfly v1.0 and later)

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modemPhy);
    modemPhy.setModemLED(modemLEDPin);
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);

    // Begin the logger
    dataLogger.begin();

    // Note:  Please change these battery voltages to match your battery
    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4) {
        Serial.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    #if defined sim_com_sim700
    /** Start [setup_sim7080] */
    modem.setModemWakeLevel(HIGH);   // ModuleFun Bee inverts the signal
    modem.setModemResetLevel(HIGH);  // ModuleFun Bee inverts the signal
    Serial.println(F("Waking modem and setting Cellular Carrier Options..."));
    modem.modemWake();  // NOTE:  This will also set up the modem
    modem.gsmModem.setBaud(modemBaud);   // Make sure we're *NOT* auto-bauding!
    modem.gsmModem.setNetworkMode(38);   // set to LTE only
                                         // 2 Automatic
                                         // 13 GSM only
                                         // 38 LTE only
                                         // 51 GSM and LTE only
    modem.gsmModem.setPreferredMode(1);  // set to CAT-M
                                         // 1 CAT-M
                                         // 2 NB-IoT
                                         // 3 CAT-M and NB-IoT
    /** End [setup_sim7080] */
    #elif defined sim_com_xbee_wifi
    /** Start [setup_sim7080] */

    Serial.println(F("Waking modem WiFi  ..."));
    modemPhy.modemWake();  // NOTE:  This will also set up the modem
    modemPhy.gsmModem.setBaud(modemBaud);   // Make sure we're *NOT* auto-bauding!
    #endif //Modem setup

    // Sync the clock if it isn't valid or we have battery to spare
    if (getBatteryVoltage() > 3.55 || !dataLogger.isRTCSane()) {
        // Synchronize the RTC with NIST
        // This will also set up the modem
        dataLogger.syncRTC();
    }

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    // Writing to the SD card can be power intensive, so if we're skipping
    // the sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4) {
        Serial.println(F("Setting up file on SD card"));
        dataLogger.turnOnSDcard(
            true);  // true = wait for card to settle after power up
        dataLogger.createLogFile(true);  // true = write a new header
        dataLogger.turnOffSDcard(
            true);  // true = wait for internal housekeeping after write
    }

    // Call the processor sleep
    Serial.println(F("Putting processor to sleep\n"));
    dataLogger.systemSleep();
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */
// Use this short loop for simple data logging and sending
void loop() {
    // Note:  Please change these battery voltages to match your battery
    // At very low battery, just go back to sleep
    if (getBatteryVoltage() < 3.4) {
        dataLogger.systemSleep();
    }
    // At moderate voltage, log data but don't send it over the modem
    else if (getBatteryVoltage() < 3.55) {
        dataLogger.logData();
    }
    // If the battery is good, send the data to the world
    else {
        dataLogger.logDataAndPublish();
    }
}
/** End [loop] */
