/** =========================================================================
 * @example{lineno} EnviroDIY_Monitoring_Kit.ino
 * @copyright Stroud Water Research Center
 * @license This example is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Example for DRWI CitSci LTE sites.
 *
 * Example sketch to be used with the [EnviroDIY Monitoring Station
 * Kit](https://www.envirodiy.org/product/envirodiy-monitoring-station-kit/).
 *
 * See [the walkthrough page](@ref example_envirodiy_monitoring_kit) for
 * detailed instructions.
 *
 * @m_examplenavigation{example_envirodiy_monitoring_kit,}
 * ======================================================================= */

// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// ==========================================================================
//  Configuration for the EnviroDIY Monitoring Station Kit
// ==========================================================================
/** Start [configuration] */

// Uncomment ONE of the following lines to select the "Bee" module you are
// using.  If you are not using a Bee module, you can skip this section and the
// sections for the modems in the code below.
// If you do not have a Bee module, you should comment out both.

#define USE_WIFI_BEE
// #define USE_CELLULAR_BEE

// Add your network information here.
// APN for cellular connection
#define CELLULAR_APN "add_your_cellular_apn"
// WiFi access point name
#define WIFI_ID "your_wifi_ssid"
// WiFi password (WPA2)
#define WIFI_PASSWD "your_wifi_password"

/** End [configuration] */


// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const int8_t loggingInterval = 15;
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!
/** End [logging_options] */


// ==========================================================================
//  UUID's and Registration Tokens for Monitor My Watershed
// ==========================================================================
/** Start [monitor_mw_uuids] */

// All UUID's, device registration, and sampling feature information can be
// pasted directly from Monitor My Watershed.
// To get the list, click the "View  token UUID list" button on the upper right
// of the site page.

// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***
// Check the order of your variables this list!!!
// They MUST be in EXACTLY this order:

// Specific conductance (Meter_Hydros21_Cond)
// Water depth (Meter_Hydros21_Depth)
// Temperature (Meter_Hydros21_Temp)
// Battery voltage (EnviroDIY_Mayfly_Batt)
// Percent full scale (EnviroDIY_LTEB_SignalPercent or ESP32_SignalPercent)
// Relative humidity (Sensirion_SHT40_Humidity)
// Temperature (Sensirion_SHT40_Temperature)
// Illuminance (Everlight_AnalogALS_Illuminance)
// Temperature (Maxim_DS3231_Temp)

// If your variables on Monitor My Watershed are in a different order than the
// variableList array, you will need to rearrange the UUIDs below to match this
// order. If you don't, your data will be sent to the wrong place on Monitor My
// Watershed and will be very difficult to fix after the fact.
// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***

// Replace all of the text in the following section with the UUID array from
// MonitorMyWatershed
/* clang-format off */
// ---------------------   Beginning of Token UUID List   ---------------------


const char* UUIDs[] =  // UUID array for device sensors
    {
        "12345678-abcd-1234-ef00-1234567890ab",  // Specific conductance (Meter_Hydros21_Cond)
        "12345678-abcd-1234-ef00-1234567890ab",  // Water depth (Meter_Hydros21_Depth)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Meter_Hydros21_Temp)
        "12345678-abcd-1234-ef00-1234567890ab",  // Battery voltage (EnviroDIY_Mayfly_Batt)
        "12345678-abcd-1234-ef00-1234567890ab",  // Percent full scale (EnviroDIY_LTEB_SignalPercent)
        "12345678-abcd-1234-ef00-1234567890ab",  // Relative humidity (Sensirion_SHT40_Humidity)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Sensirion_SHT40_Temperature)
        "12345678-abcd-1234-ef00-1234567890ab",  // Illuminance (Everlight_AnalogALS_Illuminance)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Maxim_DS3231_Temp)
};
const char* registrationToken = "12345678-abcd-1234-ef00-1234567890ab";  // Device registration token
const char* samplingFeature = "12345678-abcd-1234-ef00-1234567890ab";  // Sampling feature UUID


// -----------------------   End of Token UUID List  -----------------------
/* clang-format on */
/** End [monitor_mw_uuids] */


// ==========================================================================
//  Defines for TinyGSM
// ==========================================================================
/** Start [defines] */
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 256
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
/** End [defines] */


// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
/** Start [includes] */
// Include the main header for ModularSensors
#include <ModularSensors.h>
/** End [includes] */


// ==========================================================================
//  Logger Pins and Options
// ==========================================================================
/** Start [logger_pins] */
// The name of this program file - this is used only for console printouts at
// start-up
const char* sketchName = "EnviroDIY_Monitoring_Kit.ino";

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud    = 115200;  // Baud rate for debugging
const int8_t  greenLED      = 8;       // Pin for the green LED
const int8_t  redLED        = 9;       // Pin for the red LED
const int8_t  buttonPin     = 21;     // Pin for debugging mode (ie, button pin)
uint8_t       buttonPinMode = INPUT;  // mode for debugging pin
const int8_t  wakePin       = 31;  // MCU interrupt/alarm pin to wake from sleep
uint8_t       wakePinMode   = INPUT_PULLUP;  // mode for wake pin
// Mayfly 0.x, 1.x D31 = A7
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t flashSSPin     = 20;  // onboard flash chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power
/** End [logger_pins] */


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create a new logger instance
Logger dataLogger(LoggerID, samplingFeature, loggingInterval);
/** End [loggers] */


// ==========================================================================
//  Wifi/Cellular Modem Options
// ==========================================================================
#ifdef USE_WIFI_BEE
/** Start [espressif_esp32] */
#include <modems/EspressifESP32.h>

// Create a reference to the serial port for the modem
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
const int32_t   modemBaud   = 115200;   // Communication speed of the modem
// NOTE:  This baud rate too fast for the Mayfly.  We'll slow it down in the
// setup.

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply
// Example pins here are for a EnviroDIY ESP32 Bluetooth/Wifi Bee with
// Mayfly 1.1
const int8_t modemVccPin   = 18;      // MCU pin controlling modem power
const int8_t modemResetPin = -1;      // MCU pin connected to modem reset pin
const int8_t modemLEDPin   = redLED;  // MCU pin connected an LED to show modem
                                      // status

// Network connection information
const char* wifiId  = WIFI_ID;      // WiFi access point name
const char* wifiPwd = WIFI_PASSWD;  // WiFi password (WPA2)

// Create the modem object
EspressifESP32 modemESP(&modemSerial, modemVccPin, modemResetPin, wifiId,
                        wifiPwd);
// Create an extra reference to the modem by a generic name
EspressifESP32 modem = modemESP;
/** End [espressif_esp32] */
#endif


#ifdef USE_CELLULAR_BEE
/** Start [sim_com_sim7080] */
// For almost anything based on the SIMCom SIM7080G
#include <modems/SIMComSIM7080.h>

// Create a reference to the serial port for the modem
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
const int32_t modemBaud = 57600;  //  SIM7080 does auto-bauding by default, but
                                  //  for simplicity we set to 57600

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply
const int8_t modemVccPin     = 18;
const int8_t modemStatusPin  = 19;  // MCU pin used to read modem status
const int8_t modemSleepRqPin = 23;  // MCU pin for modem sleep/wake request
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem
                                    // status

// Network connection information
const char* apn = CELLULAR_APN;  // APN for GPRS connection

// Create the modem object
SIMComSIM7080 modem7080(&modemSerial, modemVccPin, modemStatusPin,
                        modemSleepRqPin, apn);
// Create an extra reference to the modem by a generic name
SIMComSIM7080 modem = modem7080;
/** End [sim_com_sim7080] */
#endif

// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
/** Start [processor_stats] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion, 5);
/** End [processor_stats] */


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
//  Built in on Mayfly 0.x and 1.x
// ==========================================================================
/** Start [maxim_ds3231] */
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);
/** End [maxim_ds3231] */


// ==========================================================================
//  Everlight ALS-PT19 Ambient Light Sensor
//  Built in on Mayfly 1.x
// ==========================================================================
/** Start [everlight_alspt19] */
#include <sensors/EverlightALSPT19.h>

// NOTE: Use -1 for any pins that don't apply or aren't being used.
const int8_t  alsPower      = sensorPowerPin;  // Power pin
const int8_t  alsData       = A4;              // The ALS PT-19 data pin
const int8_t  alsSupply     = 3.3;  // The ALS PT-19 supply power voltage
const int8_t  alsResistance = 10;   // The ALS PT-19 loading resistance (in kΩ)
const uint8_t alsNumberReadings = 10;

// Create a Everlight ALS-PT19 sensor object
EverlightALSPT19 alsPt19(alsPower, alsData, alsSupply, alsResistance,
                         alsNumberReadings);
/** End [everlight_alspt19] */


// ==========================================================================
//  Sensirion SHT4X Digital Humidity and Temperature Sensor
//  Built in on Mayfly 1.x
// ==========================================================================
/** Start [sensirion_sht4x] */
#include <sensors/SensirionSHT4x.h>

// NOTE: Use -1 for any pins that don't apply or aren't being used.
const int8_t SHT4xPower     = sensorPowerPin;  // Power pin
const bool   SHT4xUseHeater = true;

// Create an Sensirion SHT4X sensor object
SensirionSHT4x sht4x(SHT4xPower, SHT4xUseHeater);
/** End [sensirion_sht4x] */


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


/* clang-format off */
// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variables_separate_uuids] */
Variable* variableList[] = {
    new MeterHydros21_Cond(&hydros),             // Specific conductance (Meter_Hydros21_Cond)
    new MeterHydros21_Depth(&hydros),            // Water depth (Meter_Hydros21_Depth)
    new MeterHydros21_Temp(&hydros),             // Temperature (Meter_Hydros21_Temp)
    new ProcessorStats_Battery(&mcuBoard),       // Battery voltage (EnviroDIY_Mayfly_Batt)
    #if defined(USE_CELLULAR_BEE) || defined(USE_WIFI_BEE)
    new Modem_SignalPercent(&modem),             // Percent full scale (EnviroDIY_LTEB_SignalPercent)
    #endif
    new SensirionSHT4x_Humidity(&sht4x),         // Relative humidity (Sensirion_SHT40_Humidity)
    new SensirionSHT4x_Temp(&sht4x),             // Temperature (Sensirion_SHT40_Temperature)
    new EverlightALSPT19_Illuminance(&alsPt19),  // Illuminance (Everlight_AnalogALS_Illuminance)
    new MaximDS3231_Temp(&ds3231),               // Temperature (Maxim_DS3231_Temp)
};
/* clang-format on */

// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object and attach the UUID's
VariableArray varArray(variableCount, variableList, UUIDs);
/** End [variables_separate_uuids] */
// ==========================================================================


#if defined(USE_CELLULAR_BEE) || defined(USE_WIFI_BEE)
// ==========================================================================
//  A Publisher to Monitor My Watershed
// ==========================================================================
/** Start [monitor_my_watershed_publisher] */
// Create a data publisher for the Monitor My Watershed POST endpoint
#include <publishers/MonitorMyWatershedPublisher.h>
MonitorMyWatershedPublisher MonitorMWPost(dataLogger, registrationToken);
/** End [monitor_my_watershed_publisher] */
#endif

// ==========================================================================
//  Working Functions
// ==========================================================================
/** Start [working_functions] */
// Flashes the LED's on the primary board
void greenRedFlash(uint8_t numFlash = 4, uint8_t rate = 75) {
    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
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

// Uses the processor sensor object to read the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage() {
    if (mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM] == -9999 ||
        mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM] == 0) {
        mcuBoard.update();
    }
    return mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM];
}


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
void setup() {
    /** Start [setup_flashing_led] */
    // Blink the LEDs to show the board is on and starting up
    greenRedFlash(3, 35);
    /** End [setup_flashing_led] */
    // Start the primary serial connection
    Serial.begin(serialBaud);

    greenRedFlash(5, 50);

    // Print a start-up note to the first serial port
    PRINTOUT("\n\n\n=============================");
    PRINTOUT("=============================");
    PRINTOUT("=============================");
    PRINTOUT(F("\n\nNow running"), sketchName, F("on Logger"), LoggerID, '\n');

    PRINTOUT(F("Using ModularSensors Library version"),
             MODULAR_SENSORS_VERSION);
#if defined(USE_CELLULAR_BEE) || defined(USE_WIFI_BEE)
    PRINTOUT(F("TinyGSM Library version"), TINYGSM_VERSION, '\n');
#endif
    PRINTOUT(F("Processor:"), mcuBoard.getSensorLocation());
    PRINTOUT(F("The most recent reset cause was"), mcuBoard.getLastResetCode(),
             '(', mcuBoard.getLastResetCause(), ")\n");
    /** End [setup_prints] */

/** Start [setup_serial_begins] */
// Start the serial connection with the modem
#if defined(USE_CELLULAR_BEE) || defined(USE_WIFI_BEE)
    PRINTOUT(F("Starting modem connection at"), modemBaud, F("baud"));
    modemSerial.begin(modemBaud);
#endif

    // Start the SPI library
    PRINTOUT(F("Starting SPI"));
    SPI.begin();

#if defined(EXTERNAL_FLASH_DEVICES)
    PRINTOUT(F("Setting onboard flash pin modes"));
    pinMode(flashSSPin,
            OUTPUT);  // for proper operation of the onboard flash memory
#endif

    PRINTOUT(F("Starting I2C (Wire)"));
    Wire.begin();

    /** Start [setup_logger] */

    // set the logger ID
    PRINTOUT(F("Setting logger id to"), LoggerID);
    dataLogger.setLoggerID(LoggerID);
    PRINTOUT(F("Setting the sampling feature UUID to"), LoggerID);
    dataLogger.setSamplingFeatureUUID(samplingFeature);
    // set the logging interval
    PRINTOUT(F("Setting logging interval to"), loggingInterval, F("minutes"));
    dataLogger.setLoggingInterval(loggingInterval);
    PRINTOUT(F("Setting number of initial 1 minute intervals to 10"));
    dataLogger.setInitialShortIntervals(10);
    // Attach the variable array to the logger
    PRINTOUT(F("Attaching the variable array"));
    dataLogger.setVariableArray(&varArray);
    // set logger pins
    PRINTOUT(F("Setting logger pins"));
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED, wakePinMode, buttonPinMode);

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    PRINTOUT(F("Setting logger time zone"));
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    loggerClock::setRTCOffset(0);

#if defined(USE_CELLULAR_BEE) || defined(USE_WIFI_BEE)
    // Attach the modem and information pins to the logger
    PRINTOUT(F("Attaching the modem"));
    dataLogger.attachModem(modem);
    PRINTOUT(F("Setting modem LEDs"));
    modem.setModemLED(modemLEDPin);
#endif

    // Begin the logger
    PRINTOUT(F("Beginning the logger"));
    dataLogger.begin();
    /** End [setup_logger] */

    /** Start [setup_sensors] */
    // Note:  Please change these battery voltages to match your battery
    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4) {
        PRINTOUT(F("Setting up sensors..."));
        varArray.sensorsPowerUp();
        varArray.setupSensors();
        varArray.sensorsPowerDown();
    }
    /** End [setup_sensors] */

#ifdef USE_WIFI_BEE
    /** Start [setup_esp] */
    PRINTOUT(F("Waking the modem.."));
    PRINTOUT(F("Attempting to begin modem communication at"), modemBaud,
             F("baud.  This will fail if the baud is mismatched.."));
    modemSerial.begin(modemBaud);
    modem.modemWake();  // NOTE:  This will also set up the modem
    // WARNING: PLEASE REMOVE AUTOBAUDING FOR PRODUCTION CODE!
    if (!modem.gsmModem.testAT()) {
        PRINTOUT(F("Attempting to force the modem baud rate."));
        modem.gsmModem.forceModemBaud(modemSerial,
                                      static_cast<uint32_t>(modemBaud));
    }
/** End [setup_esp] */
#endif

#ifdef USE_CELLULAR_BEE
    /** Start [setup_sim7080] */
    modem.setModemWakeLevel(HIGH);   // ModuleFun Bee inverts the signal
    modem.setModemResetLevel(HIGH);  // ModuleFun Bee inverts the signal
    PRINTOUT(F("Waking modem and setting Cellular Carrier Options..."));
    modem.modemWake();  // NOTE:  This will also set up the modem
    // WARNING: PLEASE REMOVE AUTOBAUDING FOR PRODUCTION CODE!
    if (!modem.gsmModem.testAT()) {
        PRINTOUT(F("Attempting to force the modem baud rate."));
        modem.gsmModem.forceModemBaud(modemSerial,
                                      static_cast<uint32_t>(modemBaud));
    }
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
#endif

    /** Start [setup_clock] */
    // Sync the clock if it isn't valid or we have battery to spare
    if (getBatteryVoltage() > 3.55 || !loggerClock::isRTCSane()) {
        // Set up the modem, synchronize the RTC with NIST, and publish
        // configuration information to publishers that support it.
        dataLogger.makeInitialConnections();
    }
    /** End [setup_clock] */

    /** Start [setup_file] */
    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct.
    // Writing to the SD card can be power intensive, so if we're skipping the
    // sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4) {
        PRINTOUT(F("Setting up file on SD card"));
        dataLogger.turnOnSDcard(true);
        // true = wait for card to settle after power up
        dataLogger.createLogFile(true);  // true = write a new header
        dataLogger.turnOffSDcard(true);
        // true = wait for internal housekeeping after write
    }
    /** End [setup_file] */

    /** Start [setup_sleep] */
    // Call the processor sleep
    PRINTOUT(F("Putting processor to sleep\n"));
    dataLogger.systemSleep();
    /** End [setup_sleep] */
}


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [simple_loop] */
// Use this short loop for simple data logging and sending
void loop() {
    // Note:  Please change these battery voltages to match your battery
    // At very low battery, just go back to sleep
    if (getBatteryVoltage() < 3.4) {
        PRINTOUT(F("Battery too low, ("),
                 mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM],
                 F("V) going back to sleep."));
        dataLogger.systemSleep();
#if defined(USE_CELLULAR_BEE) || defined(USE_WIFI_BEE)
    } else if (getBatteryVoltage() < 3.55) {
        // At moderate voltage, log data but don't send it over the modem
        PRINTOUT(F("Battery at"),
                 mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM],
                 F("V; high enough to log, but will not publish!"));
        dataLogger.logData();
    } else {
        // If the battery is good, send the data to the world
        PRINTOUT(F("Battery at"),
                 mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM],
                 F("V; high enough to log and publish data"));
        dataLogger.logDataAndPublish();
    }
#else
    } else {
        // If the battery is good enough to log, log the data but we have no
        // modem so we can't publish
        PRINTOUT(F("Battery at"),
                 mcuBoard.sensorValues[PROCESSOR_BATTERY_VAR_NUM],
                 F("V; high enough to log data"));
        dataLogger.logData();
    }
#endif
}
/** End [simple_loop] */
