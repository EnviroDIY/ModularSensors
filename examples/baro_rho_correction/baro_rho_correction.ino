/*****************************************************************************
logging_to_EnviroDIY.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data to an SD card and sending the data to
the EnviroDIY data portal.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_A6  // Select for a AI-Thinker A6 or A7 chip
// #define TINY_GSM_MODEM_M590  // Select for a Neoway M590
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerEnviroDIY.h>


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
const int8_t timeZone = -5;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorStats.h>

const long serialBaud = 115200;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int8_t redLED = 9;  // Pin for the red LED (-1 if unconnected)
const int8_t buttonPin = 21;  // Pin for a button to use to enter debugging mode (-1 if unconnected)
const int8_t wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

// Create and return the processor "sensor"
const char *MFVersion = "v0.5";
ProcessorStats mayfly(MFVersion) ;
// Create the battery voltage and free RAM variable objects for the Y504 and return variable-type pointers to them
Variable *mayflyBatt = new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");
Variable *mayflyRAM = new ProcessorStats_FreeRam(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.

#if defined(TINY_GSM_MODEM_XBEE)
const long ModemBaud = 9600;  // Default for XBee is 9600, I've sped mine up to 57600
const int8_t modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_sleep_reverse;  // How the modem is put to sleep

#elif defined(TINY_GSM_MODEM_ESP8266)
const long ModemBaud = 57600;  // Default for ESP8266 is 115200, but the Mayfly itself stutters above 57600
const int8_t modemSleepRqPin = 19;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = -1;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_always_on;  // How the modem is put to sleep

#elif defined(TINY_GSM_MODEM_UBLOX)
const long ModemBaud = 9600;
const int8_t modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_sleep_held;  // How the modem is put to sleep

#else
const long ModemBaud = 9600;  // SIM800 auto-detects, but I've had trouble making it fast (19200 works)
const int8_t modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_sleep_held;  // How the modem is put to sleep
// Use "modem_sleep_held" if the DTR pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.
// Use "modem_sleep_pulsed" if the DTR pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.
// Use "modem_sleep_reverse" if the DTR pin is held LOW to keep the modem awake, as with all XBees.
// Use "modem_always_on" if you do not want the library to control the modem power and sleep or if none of the above apply.
#endif

const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a TinyGSM Client, and an on/off method
loggerModem modem;
// Create the RSSI and signal strength variable objects for the modem and return
// variable-type pointers to them
Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab");
Variable *modemSinalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);
// Create the temperature variable object for the DS3231
Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <BoschBME280.h>
uint8_t BMEi2c_addr = 0x77;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address
const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);
// Create the four variable objects for the BME and return variable pointers to them
Variable *bTemp = new BoschBME280_Temp(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bHumid = new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bPress = new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bAlt = new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <MaximDS18.h>
// OneWire Address [array of 8 hex characters]
// DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
const int8_t OneWireBus = 4;  // Pin attached to the OneWire Bus (-1 if unconnected)
const int8_t OneWirePower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Maxim DS18 sensor object (use this form for a known address)
// MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
// Create and return the Maxim DS18 sensor object (use this form for a single sensor on bus with an unknow address)
MaximDS18 ds18_u(OneWirePower, OneWireBus);
// Create the temperature variable object for the DS18 and return a variable-type pointer to it
Variable *ds18Temp = new MaximDS18_Temp(&ds18_u, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <MeaSpecMS5803.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;
// Create and return the MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);
// Create the temperature and pressure variable objects for the MS5803 and return variable pointers to them
Variable *msTemp = new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");
Variable *msPress = new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Calculated Variables
// ==========================================================================

// Create the function to calculate the water pressure
// Water pressure = pressure from MS5803 (water+baro) - pressure from BME280 (baro)
// The MS5803 reports pressure in millibar, the BME280 in pascal
// 1 pascal = 0.01 mbar
float calculateWaterPressure(void)
{
    float totalPressureFromMS5803 = msPress->getValue();
    float baroPressureFromBME280 = bPress->getValue();
    float waterPressure = totalPressureFromMS5803 - (baroPressureFromBME280)*0.01;
    if (totalPressureFromMS5803 == -9999 || baroPressureFromBME280 == -9999)
        waterPressure = -9999;
    // Serial.print(F("Water pressure is "));  // for debugging
    // Serial.println(waterPressure);  // for debugging
    return waterPressure;
}
// Properties of the calculated water pressure variable
const char *waterPresureVarName = "pressureGauge";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *waterPresureVarUnit = "millibar";  // This must be a value from http://vocabulary.odm2.org/units/
int waterPresureVarResolution = 3;
const char *waterPressureUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *waterPresureVarCode = "CorrectedPressure";
// Create the calculated water pressure variable objects and return a variable pointer to it
Variable *calcWaterPress = new Variable(calculateWaterPressure, waterPresureVarName,
                                        waterPresureVarUnit, waterPresureVarResolution,
                                        waterPressureUUID, waterPresureVarCode);

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
    float waterTempertureC = msTemp->getValue();
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
// Put all of the variable pointers into an Array
// NOTE:  Since we've created all of the variable pointers above, we can just
// reference them by name here.
Variable *variableList[] = {
    mayflyBatt,
    mayflyRAM,
    ds3231Temp,
    bTemp,
    bHumid,
    bPress,
    bAlt,
    msTemp,
    msPress,
    ds18Temp,
    calcWaterPress,
    calcRawDepth,
    calcCorrDepth,
    modemRSSI,
    modemSinalPct
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray varArray(variableCount, variableList);
// Create a new logger instance
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

    // Start the serial connection with the modem
    ModemSerial.begin(ModemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Setup the logger modem
    #if defined(TINY_GSM_MODEM_ESP8266)
        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);
    #elif defined(TINY_GSM_MODEM_XBEE)
        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);
        // modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, apn);
    #else
        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, apn);
    #endif

    // Attach the modem and information pins to the logger
    EnviroDIYLogger.attachModem(modem);
    EnviroDIYLogger.setAlertPin(greenLED);
    EnviroDIYLogger.setTestingModePin(buttonPin);

    // Enter the tokens for the connection with EnviroDIY
    EnviroDIYLogger.setToken(registrationToken);
    EnviroDIYLogger.setSamplingFeatureUUID(samplingFeature);

    // Begin the logger
    EnviroDIYLogger.begin();
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Log the data
    EnviroDIYLogger.log();
}
