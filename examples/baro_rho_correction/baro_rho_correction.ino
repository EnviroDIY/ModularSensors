/** =========================================================================
 * @file baro_rho_correction.ino
 * @brief Example demonstrating calculated variables.
 *
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @copyright (c) 2017-2022 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 *
 * Build Environment: Visual Studios Code with PlatformIO
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

// ==========================================================================
// Defines for TinyGSM
// NOTE:  These only work with TinyGSM.
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
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "baro_rho_correction.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 15;
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
/** Start [sodaq_2g_bee_r6] */
// For the Sodaq 2GBee R6 and R7 based on the SIMCom SIM800
// NOTE:  The Sodaq GPRSBee doesn't expose the SIM800's reset pin
#include <modems/Sodaq2GBeeR6.h>

// Create a reference to the serial port for the modem
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible

const int32_t modemBaud = 9600;  //  SIM800 does auto-bauding by default

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply
// Example pins are for a Sodaq GPRSBee R6 or R7 with a Mayfly
const int8_t modemVccPin     = 23;  // MCU pin controlling modem power
const int8_t modemStatusPin  = 19;  // MCU pin used to read modem status
const int8_t modemResetPin   = -1;  // MCU pin connected to modem reset pin
const int8_t modemSleepRqPin = -1;  // MCU pin for modem sleep/wake request
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem
                                    // status

// Network connection information
const char* apn = "xxxxx";  // APN for GPRS connection

// Create the modem object
Sodaq2GBeeR6 modem2GB(&modemSerial, modemVccPin, modemStatusPin, apn);
// Create an extra reference to the modem by a generic name
Sodaq2GBeeR6 modem = modem2GB;

// Create RSSI and signal strength variable pointers for the modem
Variable* modemRSSI =
    new Modem_RSSI(&modem, "12345678-abcd-1234-ef00-1234567890ab", "RSSI");
Variable* modemSignalPct = new Modem_SignalPercent(
    &modem, "12345678-abcd-1234-ef00-1234567890ab", "signalPercent");
/** End [sodaq_2g_bee_r6] */


// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
/** Start [processor_sensor] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);

// Create sample number, battery voltage, and free RAM variable pointers for the
// processor
Variable* mcuBoardBatt = new ProcessorStats_Battery(
    &mcuBoard, "12345678-abcd-1234-ef00-1234567890ab");
Variable* mcuBoardAvailableRAM = new ProcessorStats_FreeRam(
    &mcuBoard, "12345678-abcd-1234-ef00-1234567890ab");
Variable* mcuBoardSampNo = new ProcessorStats_SampleNumber(
    &mcuBoard, "12345678-abcd-1234-ef00-1234567890ab");
/** End [processor_sensor] */


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
/** Start [ds3231] */
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);

// Create a temperature variable pointer for the DS3231
Variable* ds3231Temp =
    new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-ef00-1234567890ab");
/** End [ds3231] */


// ==========================================================================
//  Bosch BME280 Environmental Sensor
// ==========================================================================
/** Start [bme280] */
#include <sensors/BoschBME280.h>

const int8_t I2CPower    = sensorPowerPin;  // Power pin (-1 if unconnected)
uint8_t      BMEi2c_addr = 0x77;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove
// default) Either can be physically mofidied for the other address

// Create a Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);

// Create four variable pointers for the BME280
Variable* bme280Humid =
    new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
Variable* bme280Temp =
    new BoschBME280_Temp(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
Variable* bme280Press =
    new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
Variable* bme280Alt =
    new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
/** End [bme280] */


// ==========================================================================
//  Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
/** Start [ds18] */
#include <sensors/MaximDS18.h>

const int8_t OneWirePower = sensorPowerPin;  // Power pin (-1 if unconnected)
const int8_t OneWireBus   = 4;  // OneWire Bus Pin (-1 if unconnected)

// Create a Maxim DS18 sensor object (use this form for a single sensor on bus
// with an unknown address)
MaximDS18 ds18(OneWirePower, OneWireBus);

// Create a temperature variable pointer for the DS18
Variable* ds18Temp = new MaximDS18_Temp(&ds18,
                                        "12345678-abcd-1234-ef00-1234567890ab");
/** End [ds18] */


// ==========================================================================
//  Measurement Specialties MS5803-14BA pressure sensor
// ==========================================================================
/** Start [ms5803] */
#include <sensors/MeaSpecMS5803.h>

const uint8_t MS5803i2c_addr =
    0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int16_t MS5803maxPressure =
    14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;

// Create a MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure,
                     MS5803ReadingsToAvg);

// Create pressure and temperature variable pointers for the MS5803
Variable* ms5803Press =
    new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-ef00-1234567890ab");
Variable* ms5803Temp =
    new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-ef00-1234567890ab");
/** End [ms5803] */


// ==========================================================================
//  Calculated Variable[s]
// ==========================================================================
/** Start [calculated_pressure] */
// Create the function to calculate the water pressure
// Water pressure = pressure from MS5803 (water+baro) - pressure from BME280
// (baro) The MS5803 reports pressure in millibar, the BME280 in pascal 1 pascal
// = 0.01 mbar
float calculateWaterPressure(void) {
    float totalPressureFromMS5803 = ms5803Press->getValue();
    float baroPressureFromBME280  = bme280Press->getValue();
    float waterPressure           = totalPressureFromMS5803 -
        (baroPressureFromBME280)*0.01;
    if (totalPressureFromMS5803 == -9999 || baroPressureFromBME280 == -9999) {
        waterPressure = -9999;
    }
    // Serial.print(F("Water pressure is "));  // for debugging
    // Serial.println(waterPressure);  // for debugging
    return waterPressure;
}
// Properties of the calculated water pressure variable
const char* waterPressureVarName =
    "pressureGauge";  // This must be a value from
                      // http://vocabulary.odm2.org/variablename/
const char* waterPressureVarUnit =
    "millibar";  // This must be a value from http://vocabulary.odm2.org/units/
int         waterPressureVarResolution = 3;
const char* waterPressureUUID          = "12345678-abcd-1234-ef00-1234567890ab";
const char* waterPressureVarCode       = "CorrectedPressure";
// Create the calculated water pressure variable objects and return a variable
// pointer to it
Variable* calcWaterPress = new Variable(
    calculateWaterPressure, waterPressureVarResolution, waterPressureVarName,
    waterPressureVarUnit, waterPressureVarCode, waterPressureUUID);
/** End [calculated_pressure] */

/** Start [calculated_uncorrected_depth] */
// Create the function to calculate the "raw" water depth
// For this, we're using the conversion between mbar and mm pure water at 4°C
// This calculation gives a final result in mm of water
float calculateWaterDepthRaw(void) {
    float waterDepth = calculateWaterPressure() * 10.1972;
    if (calculateWaterPressure() == -9999) waterDepth = -9999;
    // Serial.print(F("'Raw' water depth is "));  // for debugging
    // Serial.println(waterDepth);  // for debugging
    return waterDepth;
}
// Properties of the calculated water depth variable
const char* waterDepthVarName =
    "waterDepth";  // This must be a value from
                   // http://vocabulary.odm2.org/variablename/
const char* waterDepthVarUnit =
    "millimeter";  // This must be a value from
                   // http://vocabulary.odm2.org/units/
int         waterDepthVarResolution = 3;
const char* waterDepthUUID          = "12345678-abcd-1234-ef00-1234567890ab";
const char* waterDepthVarCode       = "CalcDepth";
// Create the calculated raw water depth variable objects and return a variable
// pointer to it
Variable* calcRawDepth = new Variable(
    calculateWaterDepthRaw, waterDepthVarResolution, waterDepthVarName,
    waterDepthVarUnit, waterDepthVarCode, waterDepthUUID);
/** End [calculated_uncorrected_depth] */

/** Start [calculated_corrected_depth] */
// Create the function to calculate the water depth after correcting water
// density for temperature This calculation gives a final result in mm of water
float calculateWaterDepthTempCorrected(void) {
    const float gravitationalConstant =
        9.80665;  // m/s2, meters per second squared
    // First get water pressure in Pa for the calculation: 1 mbar = 100 Pa
    float waterPressurePa  = 100 * calculateWaterPressure();
    float waterTempertureC = ms5803Temp->getValue();
    // Converting water depth for the changes of pressure with depth
    // Water density (kg/m3) from equation 6 from
    // JonesHarris1992-NIST-DensityWater.pdf
    float waterDensity = +999.84847 + 6.337563e-2 * waterTempertureC -
        8.523829e-3 * pow(waterTempertureC, 2) +
        6.943248e-5 * pow(waterTempertureC, 3) -
        3.821216e-7 * pow(waterTempertureC, 4);
    // This calculation gives a final result in mm of water
    // from P = rho * g * h
    float rhoDepth = 1000 * waterPressurePa /
        (waterDensity * gravitationalConstant);
    if (calculateWaterPressure() == -9999 || waterTempertureC == -9999) {
        rhoDepth = -9999;
    }
    // Serial.print(F("Temperature corrected water depth is "));  // for
    // debugging Serial.println(rhoDepth);  // for debugging
    return rhoDepth;
}
// Properties of the calculated temperature corrected water depth variable
const char* rhoDepthVarName =
    "waterDepth";  // This must be a value from
                   // http://vocabulary.odm2.org/variablename/
const char* rhoDepthVarUnit =
    "millimeter";  // This must be a value from
                   // http://vocabulary.odm2.org/units/
int         rhoDepthVarResolution = 3;
const char* rhoDepthUUID          = "12345678-abcd-1234-ef00-1234567890ab";
const char* rhoDepthVarCode       = "DensityDepth";
// Create the temperature corrected water depth variable objects and return a
// variable pointer to it
Variable* calcCorrDepth = new Variable(
    calculateWaterDepthTempCorrected, rhoDepthVarResolution, rhoDepthVarName,
    rhoDepthVarUnit, rhoDepthVarCode, rhoDepthUUID);
/** End [calculated_corrected_depth] */


// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
// Fill array with already created and named variable pointers
Variable* variableList[] = {mcuBoardSampNo, mcuBoardBatt,  mcuBoardAvailableRAM,
                            ds3231Temp,     bme280Temp,    bme280Humid,
                            bme280Press,    bme280Alt,     ms5803Temp,
                            ms5803Press,    ds18Temp,      calcWaterPress,
                            calcRawDepth,   calcCorrDepth, modemRSSI,
                            modemSignalPct};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);
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
// A Publisher to Monitor My Watershed / EnviroDIY Data Sharing Portal
// Device registration and sampling feature information can be obtained after
// registration at https://monitormywatershed.org or https://data.envirodiy.org
const char* registrationToken =
    "12345678-abcd-1234-ef00-1234567890ab";  // Device registration token
const char* samplingFeature =
    "12345678-abcd-1234-ef00-1234567890ab";  // Sampling feature UUID

// Create a data publisher for the Monitor My Watershed/EnviroDIY POST endpoint
#include <publishers/EnviroDIYPublisher.h>
EnviroDIYPublisher EnviroDIYPOST(dataLogger, &modem.gsmClient,
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

// Uses the processor sensor to read the battery voltage
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
    Logger::setRTCTimeZone(0);

    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modem);
    modem.setModemLED(modemLEDPin);
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
