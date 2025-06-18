/** =========================================================================
 * @example{lineno} AWS_IoT_Core.ino
 * @copyright Stroud Water Research Center
 * @license This example is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Example logging data and publishing to AWS IoT Core.
 *
 * Before using this example, you **MUST** obtain the certificates from AWS IoT
 * and upload them to your modem. This library does NOT support SSL connetions
 * natively - it offload the SSL connection process onto the modem.  This means
 * that the modem must be able to handle the SSL connection and must have the
 * certificates loaded onto it. Make sure you know the certificate file name as
 * you loaded it onto the module. For supported modems, you can use the
 * AWS_IoT_SetCertificates program in the extras folder to load your
 * certificates. Once you have confirmed your certificates are loaded and
 * working, there is no reason to rerun that program unless you have a new
 * modom, reset your modem, or your certificates change. Most modules store the
 * certificates in flash, which has a limited number of read/write cycles. To
 * avoid wearing out the flash unnecessarily, you should only run load/write the
 * certificates when necessarily, instead of every time you re-start your board.
 *
 * See [the walkthrough page](@ref example_aws_iot_core) for detailed
 * instructions.
 *
 * @m_examplenavigation{example_aws_iot_core,}
 * ======================================================================= */

// ==========================================================================
//  Defines for TinyGSM
// ==========================================================================
/** Start [defines] */
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 1024
#endif
/** End [defines] */

// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>
/** End [includes] */


// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
static const char AWS_IOT_ENDPOINT[] TINY_GSM_PROGMEM =
    "YOUR_ENDPOINT-ats.iot.YOUR_REGION.amazonaws.com";
#define THING_NAME "YOUR_THING_NAME"
// The name of this program file - this is used only for console printouts at
// start-up
const char* sketchName = "AWS_IoT_Core.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
// NOTE: Your LoggerID will be used as your Thing Name, primary topic, and
// client name when connecting to AWS IoT Core. _**Make sure it is unique!**_
const char* LoggerID = THING_NAME;
// Sampling feature UUID
// This is used as the sub-topic for AWS IOT Core
const char* samplingFeature = "YOUR_SAMPLING_FEATURE_ID";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 115200;  // Baud rate for debugging
const int8_t  greenLED   = 8;       // Pin for the green LED
const int8_t  redLED     = 9;       // Pin for the red LED
const int8_t  buttonPin  = 21;      // Pin for debugging mode (ie, button pin)
const int8_t  wakePin    = 31;  // MCU interrupt/alarm pin to wake from sleep
// Mayfly 0.x D31 = A7
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power
/** End [logging_options] */


// ==========================================================================
//  Wifi/Cellular Modem Options
// ==========================================================================
/** Start [espressif_esp32] */
// For almost anything based on the Espressif ESP32 using the AT command
// firmware
#include <modems/EspressifESP32.h>
// Create a reference to the serial port for the modem
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
const int32_t   modemBaud   = 115200;   // Communication speed of the modem
// NOTE:  This baud rate too fast for an 8MHz board, like the Mayfly!  The
// module should be programmed to a slower baud rate or set to auto-baud using
// the AT+UART_CUR or AT+UART_DEF command.

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply
const int8_t modemVccPin   = -2;  // MCU pin controlling modem power
const int8_t modemResetPin = 20;  // MCU pin connected to modem reset pin
const int8_t modemLEDPin =
    redLED;  // MCU pin connected an LED to show modem status

// Network connection information
const char* wifiId  = "xxxxx";  // The WiFi access point
const char* wifiPwd = "xxxxx";  // The password for connecting to WiFi

// Create the loggerModem object
EspressifESP32 modemESP(&modemSerial, modemVccPin, modemResetPin, wifiId,
                        wifiPwd);
// Create an extra reference to the modem by a generic name
EspressifESP32 modem = modemESP;
/** End [espressif_esp32] */

/** Start [modem_variables] */
// Create RSSI and signal strength variable pointers for the modem
/** End [modem_variables] */


// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
/** Start [processor_stats] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
#if defined(ENVIRODIY_STONEFLY_M4)
const char* mcuBoardVersion = "v0.1";
#elif defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
const char* mcuBoardVersion = "v1.1";
#else
const char* mcuBoardVersion = "unknown";
#endif
ProcessorStats mcuBoard(mcuBoardVersion, 5);
/** End [processor_stats] */


// ==========================================================================
//  Everlight ALS-PT19 Ambient Light Sensor
// ==========================================================================
/** Start [everlight_alspt19] */
#include <sensors/EverlightALSPT19.h>

// NOTE: Use -1 for any pins that don't apply or aren't being used.
const int8_t alsPower = sensorPowerPin;  // Power pin
#if defined(ENVIRODIY_STONEFLY_M4)
const int8_t alsData = A8;  // The ALS PT-19 data pin
#else
const int8_t alsData = A4;  // The ALS PT-19 data pin
#endif
const int8_t  alsSupply     = 3.3;  // The ALS PT-19 supply power voltage
const int8_t  alsResistance = 10;   // The ALS PT-19 loading resistance (in kΩ)
const uint8_t alsNumberReadings = 10;

// Create a Everlight ALS-PT19 sensor object
EverlightALSPT19 alsPt19(alsPower, alsData, alsSupply, alsResistance,
                         alsNumberReadings);
/** End [everlight_alspt19] */

// ==========================================================================
//  Sensirion SHT4X Digital Humidity and Temperature Sensor
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
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
Variable* variableList[] = {
    new SensirionSHT4x_Humidity(&sht4x, "12345678-abcd-1234-ef00-1234567890ab"),
    new SensirionSHT4x_Temp(&sht4x, "12345678-abcd-1234-ef00-1234567890ab"),
    new EverlightALSPT19_Voltage(&alsPt19,
                                 "12345678-abcd-1234-ef00-1234567890ab"),
    new EverlightALSPT19_Current(&alsPt19,
                                 "12345678-abcd-1234-ef00-1234567890ab"),
    new EverlightALSPT19_Illuminance(&alsPt19,
                                     "12345678-abcd-1234-ef00-1234567890ab"),

    new Modem_RSSI(&modem, "12345678-abcd-1234-ef00-1234567890ab", "RSSI"),
    new Modem_SignalPercent(&modem, "12345678-abcd-1234-ef00-1234567890ab",
                            "signalPercent"),
#if defined(TINY_GSM_MODEM_HAS_BATTERY)
    new Modem_BatteryState(&modem, "12345678-abcd-1234-ef00-1234567890ab",
                           "modemBatteryCS"),
    new Modem_BatteryPercent(&modem, "12345678-abcd-1234-ef00-1234567890ab",
                             "modemBatteryPct"),
    new Modem_BatteryVoltage(&modem, "12345678-abcd-1234-ef00-1234567890ab",
                             "modemBatterymV"),
#endif
#if defined(TINY_GSM_MODEM_HAS_TEMPERATURE)

    new Modem_Temp(&modem, "12345678-abcd-1234-ef00-1234567890ab", "modemTemp"),
#endif
    new ProcessorStats_Battery(&mcuBoard,
                               "12345678-abcd-1234-ef00-1234567890ab"),
    new ProcessorStats_FreeRam(&mcuBoard,
                               "12345678-abcd-1234-ef00-1234567890ab"),
    new ProcessorStats_SampleNumber(&mcuBoard,
                                    "12345678-abcd-1234-ef00-1234567890ab"),
    new ProcessorStats_ResetCode(&mcuBoard,
                                 "12345678-abcd-1234-ef00-1234567890ab"),
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray;
/** End [variable_arrays] */


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create a logger instance
Logger dataLogger;
/** End [loggers] */


// ==========================================================================
//  AWS IoT Core MQTT Publisher
// ==========================================================================
/** Start [aws_io_t_publisher] */
// The endpoint for your AWS IoT instance
const char* awsIoTEndpoint = AWS_IOT_ENDPOINT;
// The name of your certificate authority certificate file
const char* caCertName = "AmazonRootCA1.pem";
// The name of your client certificate file
const char* clientCertName = THING_NAME "-certificate.pem.crt";
// The name of your client private key file
const char* clientKeyName = THING_NAME "-private-key.pem.key";

// Create a data publisher for AWS IoT Core
#include <publishers/AWS_IoT_Publisher.h>
AWS_IoT_Publisher awsIoTPub(dataLogger, awsIoTEndpoint, caCertName,
                            clientCertName, clientKeyName, samplingFeature);
/** End [aws_io_t_publisher] */


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
/** End [working_functions] */


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
/** Start [setup] */
void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
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

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    loggerClock::setRTCOffset(0);

    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modem);
    modem.setModemLED(modemLEDPin);
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);

    // Begin the variable array[s], logger[s], and publisher[s]
    varArray.begin(variableCount, variableList);
    dataLogger.begin(LoggerID, loggingInterval, &varArray);

    // Note:  Please change these battery voltages to match your battery
    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4) {
        Serial.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    // Sync the clock if it isn't valid or we have battery to spare
    if (getBatteryVoltage() > 3.55 || !loggerClock::isRTCSane()) {
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
    Serial.println(F("Putting processor to sleep"));
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
