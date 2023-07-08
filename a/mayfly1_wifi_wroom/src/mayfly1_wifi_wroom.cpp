/** =========================================================================
 * @file DRWI_Mayfly1_WiFi_wroom.ino
 * @brief Adapted for for testing ReliableDelivery/ WiFi .
 *
 * This example shows proper settings for the following configuration:
 *
 * Mayfly v1.x board
 * EnviroDIY ESP32 Wifi Bee module
 * Internal sensors
 *
 * @author Neil Hancock & Sara Geleskie Damiano <sdamiano@stroudcenter.org>
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
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>
//Site https://monitormywatershed.org/sites/bq_test01/
#include "ms_cfg.h"

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
const char* sketchName = "mayfly1_wifi_wroom.cpp";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "reldlv1";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 2;
// Your logger's timezone.
const int8_t timeZone = -8;  // PST
// NOTE:  Daylight savings time will not be applied!  Please use standard time!
#define SerialStd STANDARD_SERIAL_OUTPUT

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 115200;  // Baud rate for debugging
const int8_t  greenLED   = 8;      // Pin for the green LED
const int8_t  redLED     = 9;      // Pin for the red LED
const int8_t  buttonPin  = 21;     // Pin for debugging mode (ie, button pin)
const int8_t  wakePin    = 31;     // MCU interrupt/alarm pin to wake from sleep
// Mayfly 0.x, 1.x D31 = A7
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power
/** End [logging_options] */


// ==========================================================================
//  Wifi/Cellular Modem Options
// ==========================================================================
/** Start [espressif_esp32] */
#include <modems/EspressifESP32.h>

// Create a reference to the serial port for the modem

HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
#define ESP32_MODEM_115K_BAUD 115200
#define ESP32_MODEM_57K_BAUD  57600
#define ESP32_MODEM_9K6_BAUD   9600
#define ESP32_MODEM_DEF_BAUD  ESP32_MODEM_57K_BAUD 
 const int32_t   modemBaud   = ESP32_MODEM_DEF_BAUD;   // Communication speed of the modem
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
const char* wifiId  = WIFIID_SSID_DEF;  // WiFi access point name
const char* wifiPwd = WIFIPWD_DEF;  // WiFi password (WPA2)

// Create the modem object
EspressifESP32 modemESP(&modemSerial, modemVccPin, modemResetPin, wifiId,
                        wifiPwd);
// Create an extra reference to the modem by a generic name
EspressifESP32 modemPhy = modemESP;
/** End [espressif_esp32] */


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
//  Built in on all versions of the Mayfly
// ==========================================================================
/** Start [ds3231] */
#if 0
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);
#endif
/** End [ds3231] */

#if defined SENSIRION_SHT4X_UUID
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
#endif //SENSIRION_SHT4X_UUID

// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
//#include "ms_cfg.h"
Variable* variableList[] = {
    // Should follow UUIDs
    new ProcessorStats_SampleNumber(&mcuBoard),
    new ProcessorStats_Battery(&mcuBoard),       // Battery voltage (EnviroDIY_Mayfly_Batt)
    new SensirionSHT4x_Temp(&sht4x),             // Temperature (Sensirion_SHT40_Temperature)
    new SensirionSHT4x_Humidity(&sht4x),         // Relative humidity (Sensirion_SHT40_Humidity)
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
// see ms_cfg.h  initially setup for https://monitormywatershed.org/sites/bq_test01/


// -----------------------   End of Token UUID List  -----------------------
/* clang-format on */

// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList,UUIDs);
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
    // Start the Debug serial connection
    SerialStd.begin(serialBaud);

    // Print a start-up note to the first serial port
    SerialStd.print(F("\n---Boot. Sw Build: "));
    SerialStd.print(build_ref);
    SerialStd.print(" ");
    SerialStd.println(git_usr);
    SerialStd.print(" ");
    SerialStd.println(git_branch);

    SerialStd.print(F("Sw Name: "));
    SerialStd.print(sketchName);
    SerialStd.print(F(" on Logger "));
    SerialStd.println(LoggerID);
    SerialStd.println();

    SerialStd.print(F("Using ModularSensors Library version "));
    SerialStd.println(MODULAR_SENSORS_VERSION);
    SerialStd.print(F("TinyGSM Library version "));
    SerialStd.println(TINYGSM_VERSION);
    SerialStd.println();

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
    //if (getBatteryVoltage() > 3.4) 
    {
        SerialStd.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    /** Start [setup_esp] */
    // Modem wroom default baud is 115200
    // Mayfly TinyGSM read() processing doesn't work at 115200.
    // It needs to be slowed down.
    // On a newly installed modem, it will be at 115200, 
    // however previously programmed modems could be 57600 or 9600
    uint32_t cfgMdmBaud = modemBaud;
    SerialStd.print("ModemESP32 init default ");
    SerialStd.println(cfgMdmBaud );
    //modemSerial.end();
    modemSerial.begin(cfgMdmBaud );

    for (uint8_t ntries = 0; ntries<5; ntries++) {
        // This will also verify communication and set up the modem
        if (modemPhy.modemWake())  break;

        // if that didn't work, try changing baud rate
        cfgMdmBaud= ESP32_MODEM_115K_BAUD;
        SerialStd.print(ntries);
        SerialStd.print("] ModemESP32 init ");
        SerialStd.println(cfgMdmBaud);
        modemPhy.gsmModem.sendAT(GF("+UART_DEF=115200,8,1,0,0"));
        modemPhy.gsmModem.waitResponse();
        modemSerial.end();
        modemSerial.begin(cfgMdmBaud);
        if (modemPhy.modemWake()) break;

        // if that didn't work, try changing baud rate
        cfgMdmBaud= ESP32_MODEM_57K_BAUD;
        SerialStd.print(ntries);
        SerialStd.print("] ModemESP32 init ");
        SerialStd.println(cfgMdmBaud);
        modemPhy.gsmModem.sendAT(GF("+UART_DEF=57600,8,1,0,0"));
        modemPhy.gsmModem.waitResponse();
        modemSerial.end();
        modemSerial.begin(cfgMdmBaud);
        if (modemPhy.modemWake()) break;


        cfgMdmBaud=ESP32_MODEM_9K6_BAUD;
        SerialStd.print(ntries);
        SerialStd.print("] ModemESP32 init ");
        SerialStd.println(cfgMdmBaud );
        modemPhy.gsmModem.sendAT(GF("+UART_DEF=9600,8,1,0,0"));
        modemPhy.gsmModem.waitResponse();
        modemSerial.end();
        modemSerial.begin(cfgMdmBaud);
    }
    // set BAUD if not expected value
    if (ESP32_MODEM_DEF_BAUD== cfgMdmBaud ) {
        cfgMdmBaud= ESP32_MODEM_57K_BAUD;
        modemPhy.gsmModem.sendAT(GF("+UART_DEF=57600,8,1,0,0"));
        modemPhy.gsmModem.waitResponse();
        modemSerial.end();
        modemSerial.begin(cfgMdmBaud);
    }
    SerialStd.print("ModemESP32 connected at baud ");
    SerialStd.println(cfgMdmBaud);

    modemPhy.gsmModem.sendAT(GF("+GMR"));
    //String MdmRsp;
    modemPhy.gsmModem.waitResponse();   
    modemPhy.gsmModem.sendAT(GF("+UART_DEF?"));
    modemPhy.gsmModem.waitResponse();   
    //modemPhy.gsmModem.sendAT(GF("+UART_DEF=115200,8,1,0,0"));
    //modemPhy.gsmModem.waitResponse();  
    modemPhy.gsmModem.sendAT(GF("+UART_CUR?"));
    modemPhy.gsmModem.waitResponse();     
    /** End [setup_esp] */


    // Sync the clock if it isn't valid or we have battery to spare
    //if (getBatteryVoltage() > 3.55 || !dataLogger.isRTCSane()) 
    {
        // Synchronize the RTC with NIST
        // This will also set up the modem
        dataLogger.syncRTC();
    }

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct
    // and all sensor names correct Writing to the SD card can be power
    // intensive, so if we're skipping the sensor setup we'll skip this too.
    //if (getBatteryVoltage() > 3.4) 
    {
        SerialStd.println(F("Setting up file on SD card"));
        dataLogger.turnOnSDcard(
            true);  // true = wait for card to settle after power up
        dataLogger.createLogFile(true);  // true = write a new header
        dataLogger.turnOffSDcard(
            true);  // true = wait for internal housekeeping after write
    }
    #if defined MS_NETWORK_LAYER
    EnviroDIYPOST.setQuedState(true);
    EnviroDIYPOST.setTimerPostTimeout_mS(9876); //9.876Sec
    EnviroDIYPOST.setTimerPostPacing_mS(500);

    dataLogger.setLoggingInterval(2); //Set every minute, default 5min
    dataLogger.setSendOffset(0);
    dataLogger._sendEveryX_cnt=1;
    dataLogger.setPostMax_num(5);
    dataLogger.logDataAndPubReliably(0x08 |0x03);
    #endif //    #if defined MS_NETWORK_LAYER
    // Call the processor sleep
    SerialStd.println(F("Putting processor to sleep\n"));
    dataLogger.systemSleep();
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */
// Use this short loop for simple data logging and sending
void loop() {

    {
        #if !defined MS_NETWORK_LAYER
        dataLogger.logDataAndPublish();
        #else
        dataLogger.logDataAndPubReliably();  //TCP / RTL !there
        #endif 
    }
}
/** End [loop] */
