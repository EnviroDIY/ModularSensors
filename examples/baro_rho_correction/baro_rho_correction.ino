/*****************************************************************************
logging_to_EnviroDIY.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.16.0

This sketch is an example of logging data to an SD card and sending the data to
the EnviroDIY data portal.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "baro_rho_correction.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;   // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;        // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED = 9;          // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin = 21;      // MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22; // MCU pin controlling main sensor power (-1 if not applicable)

// Create and return the processor "sensor"
const char *MFVersion = "v0.5b";
ProcessorStats mayfly(MFVersion);
// Create the battery voltage and free RAM variable objects for the processor and return variable-type pointers to them
Variable *mayflyBatt = new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");
Variable *mayflyRAM = new ProcessorStats_FreeRam(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");
Variable *mayflySampNo = new ProcessorStats_SampleNumber(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================

// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

 // Set the serial port for the modem - software serial can also be used.
HardwareSerial &ModemSerial = Serial1;

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(ModemSerial);

// Use this to create a modem if you want to spy on modem communication through
// a secondary Arduino stream.  Make sure you install the StreamDebugger library!
// https://github.com/vshymanskyy/StreamDebugger
// #include <StreamDebugger.h>
// StreamDebugger modemDebugger(Serial1, Serial);
// TinyGsm *tinyModem = new TinyGsm(modemDebugger);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);

#if defined(TINY_GSM_MODEM_XBEE)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;        // Communication speed of the modem
const int8_t modemVccPin = -2;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = LOW;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool sleepFxn(void)
{
    if (modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        return true;
    }
    else return true;
}
bool wakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
        return true;
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);  // Because the XBee doesn't have any lights
        return true;
    }
    else return true;
}

#elif defined(TINY_GSM_MODEM_ESP8266)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 57600;        // Communication speed of the modem
const int8_t modemVccPin = -2;       // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemResetPin = -1;     // MCU Pin connected to ESP8266's RSTB pin (-1 if unconnected)
const int8_t espSleepRqPin = 13;     // ESP8266 GPIO pin used for wake from light sleep (-1 if not applicable)
const int8_t modemSleepRqPin = 19;   // MCU pin used for wake from light sleep (-1 if not applicable)
const int8_t espStatusPin = -1;      // ESP8266 GPIO pin used to give modem status (-1 if not applicable)
const int8_t modemStatusPin = -1;    // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool sleepFxn(void)
{
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake from deep sleep
    if (modemResetPin >= 0)
    {
        digitalWrite(redLED, LOW);
        return tinyModem->poweroff();
    }
    // Use this if you have GPIO16 connected to the reset pin to wake from deep sleep
    // but no other MCU pin connected to the reset pin.
    // NOTE:  This will NOT work nicely with things like "testingMode" and the
    // initial 2-minute logging interval at boot up.
    // if (loggingInterval > 1)
    // {
    //     uint32_t sleepSeconds = (((uint32_t)loggingInterval) * 60 * 1000) - 75000L;
    //     String sleepCommand = String(sleepSeconds);
    //     tinyModem->sendAT(GF("+GSLP="), sleepCommand);
    //     // Power down for 1 minute less than logging interval
    //     // Better:  Calculate length of loop and power down for logging interval - loop time
    //     return tinyModem->waitResponse() == 1;
    // }
    // Use this if you don't have access to the ESP8266's reset pin for deep sleep but you
    // do have access to another GPIO pin for light sleep.  This also sets up another
    // pin to view the sleep status.
    else if (modemSleepRqPin >= 0 && modemStatusPin >= 0)
    {
        tinyModem->sendAT(GF("+WAKEUPGPIO=1,"), String(espSleepRqPin), GF(",0,"),
                          String(espStatusPin), GF(","), modemStatusLevel);
        bool success = tinyModem->waitResponse() == 1;
        tinyModem->sendAT(GF("+SLEEP=1"));
        success &= tinyModem->waitResponse() == 1;
        digitalWrite(redLED, LOW);
        return success;
    }
    // Light sleep without the status pin
    else if (modemSleepRqPin >= 0 && modemStatusPin < 0)
    {
        tinyModem->sendAT(GF("+WAKEUPGPIO=1,"), String(espSleepRqPin), GF(",0"));
        bool success = tinyModem->waitResponse() == 1;
        tinyModem->sendAT(GF("+SLEEP=1"));
        success &= tinyModem->waitResponse() == 1;
        digitalWrite(redLED, LOW);
        return success;
    }
    else return true;  // DON'T go to sleep if we can't wake up!
}
bool wakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
    {
        digitalWrite(redLED, HIGH);  // Because the ESP8266 doesn't have any lights
        return true;
    }
    else if (modemResetPin >= 0)
    {
        digitalWrite(modemResetPin, LOW);
        delay(1);
        digitalWrite(modemResetPin, HIGH);
        digitalWrite(redLED, HIGH);
        return true;
    }
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        delay(1);
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, HIGH);
        return true;
    }
    else return true;
}

#elif defined(TINY_GSM_MODEM_UBLOX)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;         // Communication speed of the modem
const int8_t modemVccPin = 23;       // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 20;   // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;    // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool sleepFxn(void)
{
    if (modemVccPin >= 0 && modemSleepRqPin < 0)
        return tinyModem->poweroff();
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);  // A light to watch to verify pulse timing
        delay(1100);  // >1s pulse for power down
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        return true;
    }
    else return true;  // DON'T go to sleep if we can't wake up!
}
bool wakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
        return true;
    else if(modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);
        delay(200); // 0.15-3.2s pulse for wake on SARA R4/N4
        // delayMicroseconds(65); // 50-80µs pulse for wake on SARA/LISA U2/G2
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        return true;
    }
    else return true;
}

#else
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;         // Communication speed of the modem
const int8_t modemVccPin = -2;       // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;   // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;    // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool wakeFxn(void)
{
    digitalWrite(modemSleepRqPin, HIGH);
    digitalWrite(redLED, HIGH);  // A light just for show
    return true;
}
bool sleepFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    digitalWrite(redLED, LOW);
    return true;
}
#endif

// And we still need the connection information for the network
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
#include <LoggerModem.h>
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and functions for wake and sleep
#if defined(TINY_GSM_MODEM_ESP8266)
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, wifiId, wifiPwd);
#elif defined(TINY_GSM_MODEM_XBEE)
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, wifiId, wifiPwd);
// loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);
#elif defined(TINY_GSM_MODEM_UBLOX)
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);
#else
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);
#endif

// Create the RSSI and signal strength variable objects for the modem and return
// variable-type pointers to them
Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab");
Variable *modemSignalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);
// Create the temperature variable object for the DS3231
Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>
uint8_t BMEi2c_addr = 0x77;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address
const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);
// Create the four variable objects for the BME280 and return variable-type pointers to them
Variable *bme280Humid = new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bme280Temp = new BoschBME280_Temp(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bme280Press = new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bme280Alt = new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <sensors/MaximDS18.h>
// OneWire Address [array of 8 hex characters]
// DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
const int8_t OneWireBus = 4;  // Pin attached to the OneWire Bus (-1 if unconnected)
const int8_t OneWirePower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Maxim DS18 sensor object (use this form for a known address)
// MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
// Create and return the Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
MaximDS18 ds18_u(OneWirePower, OneWireBus);
// Create the temperature variable object for the DS18 and return a variable-type pointer to it
Variable *ds18Temp = new MaximDS18_Temp(&ds18_u, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <sensors/MeaSpecMS5803.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;
// Create and return the MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);
// Create the conductivity and temperature variable objects for the ES2 and return variable-type pointers to them
Variable *ms5803Press = new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");
Variable *ms5803Temp = new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Calculated Variables
// ==========================================================================

// Create the function to calculate the water pressure
// Water pressure = pressure from MS5803 (water+baro) - pressure from BME280 (baro)
// The MS5803 reports pressure in millibar, the BME280 in pascal
// 1 pascal = 0.01 mbar
float calculateWaterPressure(void)
{
    float totalPressureFromMS5803 = ms5803Press->getValue();
    float baroPressureFromBME280 = bme280Press->getValue();
    float waterPressure = totalPressureFromMS5803 - (baroPressureFromBME280)*0.01;
    if (totalPressureFromMS5803 == -9999 || baroPressureFromBME280 == -9999)
        waterPressure = -9999;
    // Serial.print(F("Water pressure is "));  // for debugging
    // Serial.println(waterPressure);  // for debugging
    return waterPressure;
}
// Properties of the calculated water pressure variable
const char *waterPressureVarName = "pressureGauge";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *waterPressureVarUnit = "millibar";  // This must be a value from http://vocabulary.odm2.org/units/
int waterPressureVarResolution = 3;
const char *waterPressureUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *waterPressureVarCode = "CorrectedPressure";
// Create the calculated water pressure variable objects and return a variable pointer to it
Variable *calcWaterPress = new Variable(calculateWaterPressure, waterPressureVarName,
                                        waterPressureVarUnit, waterPressureVarResolution,
                                        waterPressureUUID, waterPressureVarCode);

// Create the function to calculate the "raw" water depth
// For this, we're using the conversion between mbar and mm pure water at 4°C
// This calculation gives a final result in mm of water
float calculateWaterDepthRaw(void)
{
    float waterDepth = calculateWaterPressure()*10.1972;
    if (calculateWaterPressure() == -9999) waterDepth = -9999;
    // Serial.print(F("'Raw' water depth is "));  // for debugging
    // Serial.println(waterDepth);  // for debugging
    return waterDepth;
}
// Properties of the calculated water depth variable
const char *waterDepthVarName = "waterDepth";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *waterDepthVarUnit = "millimeter";  // This must be a value from http://vocabulary.odm2.org/units/
int waterDepthVarResolution = 3;
const char *waterDepthUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *waterDepthVarCode = "CalcDepth";
// Create the calculated raw water depth variable objects and return a variable pointer to it
Variable *calcRawDepth = new Variable(calculateWaterDepthRaw, waterDepthVarName,
                                      waterDepthVarUnit, waterDepthVarResolution,
                                      waterDepthUUID, waterDepthVarCode);

// Create the function to calculate the water depth after correcting water density for temperature
// This calculation gives a final result in mm of water
float calculateWaterDepthTempCorrected(void)
{
    const float gravitationalConstant = 9.80665; // m/s2, meters per second squared
    // First get water pressure in Pa for the calculation: 1 mbar = 100 Pa
    float waterPressurePa = 100 * calculateWaterPressure();
    float waterTempertureC = ms5803Temp->getValue();
    // Converting water depth for the changes of pressure with depth
    // Water density (kg/m3) from equation 6 from JonesHarris1992-NIST-DensityWater.pdf
    float waterDensity =  + 999.84847
                          + 6.337563e-2 * waterTempertureC
                          - 8.523829e-3 * pow(waterTempertureC,2)
                          + 6.943248e-5 * pow(waterTempertureC,3)
                          - 3.821216e-7 * pow(waterTempertureC,4)
                          ;
    // This calculation gives a final result in mm of water
    // from P = rho * g * h
    float rhoDepth = 1000 * waterPressurePa/(waterDensity * gravitationalConstant);
    if (calculateWaterPressure() == -9999 || waterTempertureC == -9999)
        rhoDepth = -9999;
    // Serial.print(F("Temperature corrected water depth is "));  // for debugging
    // Serial.println(rhoDepth);  // for debugging
    return rhoDepth;
}
// Properties of the calculated temperature corrected water depth variable
const char *rhoDepthVarName = "waterDepth";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *rhoDepthVarUnit = "millimeter";  // This must be a value from http://vocabulary.odm2.org/units/
int rhoDepthVarResolution = 3;
const char *rhoDepthUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *rhoDepthVarCode = "DensityDepth";
// Create the temperature corrected water depth variable objects and return a variable pointer to it
Variable *calcCorrDepth = new Variable(calculateWaterDepthTempCorrected, rhoDepthVarName,
                                       rhoDepthVarUnit, rhoDepthVarResolution,
                                       rhoDepthUUID, rhoDepthVarCode);


// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
#include <VariableArray.h>
// Put all of the variable pointers into an Array
// NOTE:  Since we've created all of the variable pointers above, we can just
// reference them by name here.
Variable *variableList[] = {
    mayflyBatt,
    mayflyRAM,
    mayflySampNo,
    ds3231Temp,
    bme280Temp,
    bme280Humid,
    bme280Press,
    bme280Alt,
    ms5803Temp,
    ms5803Press,
    ds18Temp,
    calcWaterPress,
    calcRawDepth,
    calcCorrDepth,
    modemRSSI,
    modemSignalPct
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray varArray(variableCount, variableList);

// Create a new logger instance
#include <LoggerEnviroDIY.h>
LoggerEnviroDIY EnviroDIYLogger(LoggerID, loggingInterval, sdCardPin, wakePin, &varArray);


// ==========================================================================
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
// ==========================================================================
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(int numFlash = 4, int rate = 75)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    delay(rate);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    delay(rate);
  }
  digitalWrite(redLED, LOW);
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
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

    // Start the serial connection with the modem
    ModemSerial.begin(ModemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up the sleep/wake pin for the modem and put its inital value as "off"
    #if defined(TINY_GSM_MODEM_XBEE)
        pinMode(modemSleepRqPin, OUTPUT);
        digitalWrite(modemSleepRqPin, LOW);  // Turn it on to talk, just in case
        if (tinyModem->commandMode())
        {
            tinyModem->sendAT(GF("SM"),1);  // Pin sleep
            tinyModem->waitResponse();
            tinyModem->sendAT(GF("DO"),0);  // Disable remote manager
            tinyModem->waitResponse();
            tinyModem->sendAT(GF("SO"),0);  // For Cellular - disconnected sleep
            tinyModem->waitResponse();
            tinyModem->sendAT(GF("SO"),200);  // For WiFi - Disassociate from AP for Deep Sleep
            tinyModem->waitResponse();
            tinyModem->writeChanges();
            tinyModem->exitCommand();
        }
        digitalWrite(modemSleepRqPin, HIGH);  // back to sleep
    #elif defined(TINY_GSM_MODEM_ESP8266)
        if (modemSleepRqPin >= 0)
        {
            pinMode(modemSleepRqPin, OUTPUT);
            digitalWrite(modemSleepRqPin, HIGH);
        }
        if (modemResetPin >= 0)
        {
            pinMode(modemResetPin, OUTPUT);
            digitalWrite(modemResetPin, HIGH);
        }
    #elif defined(TINY_GSM_MODEM_UBLOX)
        pinMode(modemSleepRqPin, OUTPUT);
        digitalWrite(modemSleepRqPin, HIGH);
    #else
        pinMode(modemSleepRqPin, OUTPUT);
        digitalWrite(modemSleepRqPin, LOW);
    #endif

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Attach the modem and information pins to the logger
    EnviroDIYLogger.attachModem(modem);
    EnviroDIYLogger.setAlertPin(greenLED);
    EnviroDIYLogger.setTestingModePin(buttonPin);

    // Enter the tokens for the connection with EnviroDIY
    EnviroDIYLogger.setToken(registrationToken);
    EnviroDIYLogger.setSamplingFeatureUUID(samplingFeature);

    // Begin the logger
    mayfly.update();
    Serial.print("Battery: ");
    Serial.println(mayflyBatt->getValue());
    // if (mayflyBatt->getValue() < 3.4) EnviroDIYLogger.begin(true);  // skip sensor set-up
    // else if (mayflyBatt->getValue() < 3.7) EnviroDIYLogger.begin();  // set up sensors
    // else EnviroDIYLogger.beginAndSync();  // set up sensors and synchronize clock with NIST
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Log the data
    Serial.print("Battery: ");
    Serial.println(mayflyBatt->getValue());
    if (mayflyBatt->getValue() < 3.4) EnviroDIYLogger.systemSleep();  // just go back to sleep
    else if (mayflyBatt->getValue() < 3.7) EnviroDIYLogger.logData();  // log data, but don't send
    else EnviroDIYLogger.logDataAndSend();  // send data
}
