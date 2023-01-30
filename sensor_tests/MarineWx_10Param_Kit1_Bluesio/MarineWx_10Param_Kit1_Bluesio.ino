/*****************************************************************************
MarineWx_10Param_Kit1_Bluesio.ino
Written By:  Greg Cutrell (gcutrell@limno.com) barrowed from Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team
This shows most of the standard functions of the library at once.
DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/


// ==========================================================================
//    Defines for the Arduino IDE
//    In PlatformIO, set these build flags in your platformio.ini
// ==========================================================================
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 240
#endif

// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// To get all of the base classes for ModularSensors, include LoggerBase.
// NOTE:  Individual sensor definitions must be included separately.
#include <LoggerBase.h>
/** End [includes] */

// Include the main header for ModularSensors
#include <ModularSensors.h>


// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// The library version this example was written for
const char *libraryVersion = "0.33.2";
// The name of this program file
const char *sketchName = "MarineWx_10Param_Kit1_Bluesio.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "mwx10kit4"; //
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 10;
// Your logger's timezone.
const int8_t timeZone = -5;  // GMT
// NOTE:  Daylight savings time will not be applied!  Please use standard time!


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;   // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;        // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED = 9;          // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin  = 21;      // Pin for debugging mode (ie, button pin)
const int8_t wakePin    = 31;  // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin = -1;     // MCU SD card power pin (-1 if not applicable)
const int8_t sdCardSSPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power (-1 if not applicable)

const int8_t modemVccPin =  18;  // MCU pin controlling VCC power ---
//AltSoftSerial &modbusSerial = Serial1;  // For software serial if needed
HardwareSerial& loraSerial = Serial1;  // Use hardware serial if possible
const long loraBaud = 9600;    // Baud rate for RS232 serial out to RF module

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);

// Create sample number, battery voltage, and free RAM variable pointers for the processor
Variable *mcuBoardBatt = new ProcessorStats_Battery(&mcuBoard, "a");
//Variable *mcuBoardAvailableRAM = new ProcessorStats_FreeRam(&mcuBoard, "");
Variable *mcuBoardSampNo = new ProcessorStats_SampleNumber(&mcuBoard, "b");

// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================
// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;

// NeoSWSerial (https://github.com/SRGDamia1/NeoSWSerial) is the best software
// serial that can be used on any pin supporting interrupts.
// You can use as many instances of NeoSWSerial as you want.
// Not all AVR boards are supported by NeoSWSerial.
#include <NeoSWSerial.h>  // for the stream communication
const int8_t neoSSerial1Tx = 10;     // data out pin
const int8_t neoSSerial1Rx = 11;     // data in pin
NeoSWSerial neoSSerial1(neoSSerial1Rx, neoSSerial1Tx);
// To use NeoSWSerial in this library, we define a function to receive data
// This is just a short-cut for later
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(neoSSerial1Rx)));
}

// ==========================================================================
//  Assigning Serial Port Functionality
// ==========================================================================
// The Hydreon RG-15 is the only sensor that communicates over a serial port
// #define hydreonrg15Serial Serial3
#define hydreonrg15Serial altSoftSerial  // For AltSoftSerial
// #define hydreonrg15Serial neoSSerial1     // For Neo software serial

// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
/** Start [ds3231] */
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);
/** End [ds3231] */

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

Variable* encRH = new SensirionSHT4x_Humidity(&sht4x, "c");    // Relative humidity (Sensirion_SHT40_Humidity)
Variable* encTemp =  new SensirionSHT4x_Temp(&sht4x, "d");    // Temperature (Sensirion_SHT40_Temperature)
/** End [sensirion_sht4x] */


// ==========================================================================
//  Apogee SP-215 Incoming Solar Radiation Sensor
// ==========================================================================
/** Start [apogee_sp215] */
#include <sensors/ApogeeSP215.h>

// NOTE: Use -1 for any pins that don't apply or aren't being used.
const int8_t  SP215Power       = sensorPowerPin;  // Power pin
const int8_t  SP215ADSChannel  = 3;     // The ADS channel for the SQ212
const uint8_t SP215ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
//const uint8_t SP215NumberReadings = 5; //Doesn't work

// Create an Apogee SP215 sensor object
ApogeeSP215 SP215(SP215Power, SP215ADSChannel, SP215ADSi2c_addr);

// Create PAR and raw voltage variable pointers for the SQ212
Variable* sp215Solar = new ApogeeSP215_Solar(&SP215, "t");
Variable* sp215voltage = new ApogeeSP215_Voltage(&SP215, "v");
/** End [apogee_sq212] */


// ==========================================================================
//  Hydreon RG 15 Rain Gauge Sensor
// ==========================================================================
/** Start [hydreonrg15_raingauage] */
#include <sensors/HydreonRG15.h>

// NOTE: Extra hardware and software serial ports are created in the "Settings
// for Additional Serial Ports" section

// NOTE: Use -1 for any pins that don't apply or aren't being used.
const int8_t HydreonRG15Power    = sensorPowerPin ;  // Continuous power
const uint8_t HydreonRG15NumberReadings = 1;  // The number of readings to average

// Create a MaxBotix Sonar sensor object
HydreonRG15 raingauge(hydreonrg15Serial, HydreonRG15Power, HydreonRG15NumberReadings);

// Create an ultrasonic range variable pointer
Variable* precipAccum = new HydreonRG15_Accum(&raingauge, "r");
Variable* precipInt = new HydreonRG15_precipInt(&raingauge, "s");
/** End [max_botix_sonar] */


// ==========================================================================
//    METER Atmos 14 Air temperature, relative humidity, air pressure, and vapor pressure sensor
// ==========================================================================
#include <sensors/MeterAtmos14.h>

const char *atmos14SDI12address  = "0";  // The SDI-12 Address of the CTD
const uint8_t atmos14NumberReadings = 1;  // The number of readings to average
const int8_t atmos14Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t atmos14Data = 7;  // The SDI12 data pin

// Create a Decagon CTD sensor object
MeterAtmos14 atmos14(*atmos14SDI12address, atmos14Power, atmos14Data,
                       atmos14NumberReadings);

// Create conductivity, temperature, and depth variable pointers for the Hydros21
Variable* atmos14atemp = new MeterAtmos14_aTemp(&atmos14, "h");
Variable* atmos14rh = new MeterAtmos14_RH(&atmos14, "j");
//Variable* atmos14vpress = new MeterAtmos14_vPress(&atmos22, "n");
Variable* atmos14apress = new MeterAtmos14_aPress(&atmos14, "m");

// ==========================================================================
//    METER Atmos 22 Wind Speed and Direction Sensor
// ==========================================================================
#include <sensors/MeterAtmos22.h>

const char *atmos22SDI12address  = "1";  // The SDI-12 Address of the CTD
const uint8_t atmos22NumberReadings = 1;  // The number of readings to average
const int8_t atmos22Power = -1;  // Pin to switch power on and off (-1 if unconnected)
const int8_t atmos22Data = 7;  // The SDI12 data pin

// Create a Decagon CTD sensor object
MeterAtmos22 atmos22(*atmos22SDI12address, atmos22Power, atmos22Data,
                       atmos22NumberReadings);

// Create conductivity, temperature, and depth variable pointers for the Hydros21
Variable* atmos22wspd = new MeterAtmos22_Wspd(&atmos22, "n");
Variable* atmos22wgst = new MeterAtmos22_Wgst(&atmos22, "o");
Variable* atmos22wdir = new MeterAtmos22_Wdir(&atmos22, "q");


// ==========================================================================
//    Calculated Variables
// ==========================================================================

// Create the function to give your calculated result.
// The function should take no input (void) and return a float.
// You can use any named variable pointers to access values by way of variable->getValue()

// Create the function to calculate water level / gage height variable
float calculatedDewPoint(void)
{
    float dewPoint = -9999;  // Always safest to start with a bad value
    float airtemp = atmos14atemp->getValue();
    float rh = atmos14rh->getValue();
    if (airtemp != -9999 && rh != -9999)  // make sure all inputs are good
    {
        dewPoint = 243.04*(log(rh/100)+((17.625*airtemp)/(243.04+airtemp)))/(17.625-log(rh/100)-((17.625*airtemp)/(243.04+airtemp)));
    }
    return dewPoint;
}

// Properties of the calculated water level / gage height variable
const uint8_t dewPointVarResolution = 1;  // The number of digits after the decimal place
const char *dewPointVarName = "dewPoint";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *dewPointVarUnit = "degC";  // This must be a value from http://vocabulary.odm2.org/units/
const char *dewPointVarCode = "atmos14DewPoint";  // A short code for the variable
const char *dewPointVarUUID = "i";

// Create the calculated water pressure variable pointer and return a variable pointer to it
Variable *atmos14DewPoint = new Variable(calculatedDewPoint, dewPointVarResolution,
                                        dewPointVarName, dewPointVarUnit,
                                        dewPointVarCode, dewPointVarUUID);

float calcSeaLevelPressure(void)
{
  float seaLevelPressure = -9999;
  float stationPress = atmos14apress->getValue();
  float airTemp = atmos14atemp->getValue();
  //Need to set to elevation of station in meters
  const float elev = 180;

  if (stationPress != -9999 || airTemp != -9999)
  {
    seaLevelPressure = (stationPress)/(exp(-(elev/((airTemp+273.15) * 29.263))));
  }
  return seaLevelPressure;
}

const char *seaLvlPressVarName = "barometricPressure";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *seaLvlPressVarUnit = "Millibar";  // This must be a value from http://vocabulary.odm2.org/units/
uint8_t seaLvlPressResolution = 1;
const char *seaLvlPressUUID = "l";
const char *seaLvlPressVarCode = "sealevelpressure";
Variable *seaLvlPress = new Variable(calcSeaLevelPressure,seaLvlPressResolution,
                                      seaLvlPressVarName, seaLvlPressVarUnit,
                                      seaLvlPressVarCode, seaLvlPressUUID);


// ==========================================================================
//     Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================

// Create pointers for all of the variables from the sensors
// at the same time putting them into an array
Variable *variableList[] = {
  mcuBoardBatt,
  encRH,
  encTemp,
  mcuBoardSampNo,
  sp215Solar,
  sp215voltage,
  atmos14apress,
  atmos14atemp,
  atmos14rh,
  atmos14DewPoint,
  //seaLvlPress,
  atmos22wdir,
  atmos22wspd,
  atmos22wgst,
  precipAccum,
  precipInt
};

// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create a new logger instance
Logger dataLogger(LoggerID, loggingInterval, &varArray);
/** End [loggers] */


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75)
{
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

//Send payload bluesio notecarrier
bool payload_send(String payload, int waitTime) {
  String response = "";
  Serial1.println(payload);
  Serial.println("Blues.io Command: " + payload);
  delay(waitTime);
  response = Serial1.readString();
  Serial.println("Blues.io Response:" + response);
  Serial1.println("{\"req\":\"hub.sync\"}");
  delay(waitTime);
  response = Serial1.readString();
  Serial.println("Sync Response: " + response);
  return true;
}

//Send card.time request to notecard and derserialize the json reponse. 
//If time is not inlcuded in the json object, then ask again for up to 2 minute.
#include <ArduinoJson.h>
uint32_t epochTime;
uint32_t bluesioTime;
uint32_t bluesio_getepochtime(void) {
  int counter = 0;
  uint32_t startTime; 
  bluesioTime = 0;
  String response = "";
  String payload = "{\"req\":\"card.time\"}";
  do {
    counter++;
    StaticJsonDocument<200> doc;
    response = "";
    if (counter == 1)
    {
      startTime = millis();
    }
    Serial1.println(payload);
    Serial.println("Blues.io Command: " + payload);
    delay(100);
    response = Serial1.readString();
    deserializeJson(doc, response);
    Serial.println("Blues.io Response:" + response);
    if (doc["time"]){
      bluesioTime = doc["time"];
    }
    else {
      Serial.println("No time from notecard.");
    }
    delay(5000);
  } while ((bluesioTime == 0) && (millis() - startTime < 120000)); 
  return bluesioTime;
}

// Read's the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage()
{
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
}

// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
  // Start the primary serial connection
  Serial.begin(serialBaud);

  //Start the serial connection with the LoRa RAK Module
  loraSerial.begin(loraBaud);


  //Default baudrate for Hydreon RG-15
  neoSSerial1.begin(9600);

  //Default baudrate for Hydreon RG-15
  altSoftSerial.begin(9600);

  //Power Blues.io Notecard
  digitalWrite(modemVccPin, HIGH);

  // Print a start-up note to the first serial port
  Serial.print(F("Now running "));
  Serial.print(sketchName);
  Serial.print(F(" on Logger "));
  Serial.println(LoggerID);
  Serial.println();

  // Allow interrupts for software serial
  #if defined SoftwareSerial_ExtInts_h
      enableInterrupt(softSerialRx, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
  #endif
  #if defined NeoSWSerial_h
      enableInterrupt(neoSSerial1Rx, neoSSerial1ISR, CHANGE);
  #endif

  Serial.print(F("Using ModularSensors Library version "));
  Serial.println(MODULAR_SENSORS_VERSION);

  if (String(MODULAR_SENSORS_VERSION) !=  String(libraryVersion))
      Serial.println(F(
          "WARNING: THIS EXAMPLE WAS WRITTEN FOR A DIFFERENT VERSION OF MODULAR SENSORS!!"));

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

  // Set up some of the power pins so the board boots up with them off
  // NOTE:  This isn't necessary at all.  The logger begin() function
  // should leave all power pins off when it finishes.
  if (sensorPowerPin >= 0)
  {
      pinMode(sensorPowerPin, OUTPUT);
      digitalWrite(sensorPowerPin, LOW);
  }


  // Set the timezones for the logger/data and the RTC
  // Logging in the given time zone
  Logger::setLoggerTimeZone(timeZone);
  // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
  Logger::setRTCTimeZone(0);

  // Attach information pins to the logger
  dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin, greenLED);
  //dataLogger.setSamplingFeatureUUID(samplingFeature);

  // Begin the logger
  dataLogger.begin();

  // Set up the sensors, except at lowest battery level
  if (getBatteryVoltage() > 3.0)
  {
      Serial.println(F("Setting up sensors..."));
      varArray.setupSensors();
  }

  /** Start [setup_clock] */
  // Sync the clock if it isn't valid or we have battery to spare
  if (getBatteryVoltage() > 3.0 || !dataLogger.isRTCSane()) {
    // Synchronize the RTC with NIST
    epochTime = bluesio_getepochtime();
    //Serial.println(epochTime);
    if (epochTime > 0){
      dataLogger.setRTClock(epochTime);
    }
    else {
      Serial.println(F("Unable to retrieve time from Blues.io notehub"));
    }
  }


  // Create the log file, adding the default header to it
  // Do this last so we have the best chance of getting the time correct and
  // all sensor names correct
  // Writing to the SD card can be power intensive, so if we're skipping
  // the sensor setup we'll skip this too.
  if (getBatteryVoltage() > 3.0) {
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

// ==========================================================================
// Main loop function
// ==========================================================================

// Use this short loop for simple data logging and sending
void loop()
{
  // If the battery is OK, log data and send
  if (dataLogger.checkInterval() && getBatteryVoltage() > 3.0)
  {
      // Flag to notify that we're in already awake and logging a point
      Logger::isLoggingNow = true;

      // Print a line to show new reading
      Serial.println(F("------------------------------------------"));
      // Turn on the LED to show we're taking a reading
      dataLogger.alertOn();
      // Power up the SD Card, but skip any waits after power up
      dataLogger.turnOnSDcard(false);

      // Do a complete update on the variable array.
      // This includes powering all of the sensors, getting updated values,
      // and turning them back off.
      // NOTE:  The wake function for each sensor should force sensor setup
      // to run if the sensor was not previously set up.
      varArray.completeUpdate();

      // Create a csv data record and save it to the log file
      dataLogger.logToSD();

      // Sync the clock at noon.
      dataLogger.watchDogTimer.resetWatchDog();
      if (Logger::markedLocalEpochTime != 0 && Logger::markedLocalEpochTime % 86400 == 43200) {
        Serial.println(F("Running a daily clock sync..."));
        // Synchronize the RTC with NIST
        epochTime = bluesio_getepochtime();
        if (epochTime > 0){
          dataLogger.setRTClock(epochTime);
        }
        else {
          Serial.println(F("Unable to retrieve time from Blues.io notehub"));
        }
      }

      //======== Get Data in an accessible string for serial output ============
      //Sourced from printSensorDataCSV function in LoggerBase.cpp
      String bluesioString = "";
      Serial1.readString();                   // clear the serial buffer of the wake-up response
                                              // assume delay caused by sensors will be sufficient
      bluesioString = "";                     //Reset String
      char tempBuffer[50];
      String mayflyepochTime = ltoa((Logger::markedUTCEpochTime), tempBuffer, 10);
      bluesioString = "{\"req\":\"note.add\",\"body\":{\"";
      bluesioString += LoggerID;
      bluesioString += "\":{\"v3\":{";                            //Using lora coding 3.0              
      bluesioString += "\"" + mayflyepochTime;
      bluesioString += "\":{"; 
      for (uint8_t i = 0; i < dataLogger.getArrayVarCount(); i++) //generate csv
      {
        bluesioString += "\""+dataLogger.getVarUUIDAtI(i)+"\":";

        //Convert to float value
        float value = dataLogger.getValueStringAtI(i).toFloat();
        bluesioString += String(value);

        //If it is not the last value in array then append comma
        if (i + 1 != dataLogger.getArrayVarCount())
        {
            bluesioString += ',';
        } else {
          bluesioString += "}";
          continue;
        }
      }
      
      bluesioString += "}}}}";
      //Close json payload
      //Serial.println(bluesioString);

      //======================== Output Data to bluesIO ===========================
      //Send data amd wait for response
      payload_send(bluesioString,5000);


    // Cut power from the SD card - without additional housekeeping wait
    dataLogger.turnOffSDcard(false);
    // Turn off the LED
    dataLogger.alertOff();
    // Print a line to show reading ended
    Serial.println(F("------------------------------------------\n"));

    // Unset flag
    Logger::isLoggingNow = false;

  }

  // Check if it was instead the testing interrupt that woke us up
  if (Logger::startTesting) dataLogger.testingMode();

  // Call the processor sleep
  dataLogger.systemSleep();
}
