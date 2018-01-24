/*****************************************************************************
multisensor_print.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of printing data from multiple sensors using
the modular sensor library.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// Some define statements
#define STANDARD_SERIAL_OUTPUT Serial  // Without this there will be no output

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <Sodaq_DS3231.h>    // Controls the DS3231 Real Time Clock (RTC) built into the EnviroDIY Mayfly.
#include <VariableArray.h>


// ==========================================================================
//    Basic Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "modular_sensors.ino";

// Your logger's timezone.
const int timeZone = -5;
// Create a new sensor array instance
VariableArray sensors;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorStats.h>

const long serialBaud = 57600;  // Baud rate for the primary serial port for debugging
const int greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int redLED = 9;  // Pin for the red LED (-1 if unconnected)

// Create the processor "sensor"
const char *MFVersion = "v0.5";
ProcessorStats mayfly(MFVersion) ;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
MaximDS3231 ds3231(1);


// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <AOSongAM2315.h>
const int I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302) Digital Humidity and Temperature
// ==========================================================================
#include <AOSongDHT.h>
const int DHTPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int DHTPin = 10;  // DHT data pin
DHTtype dhtType = DHT11;  // DHT type, either DHT11, DHT21, or DHT22
AOSongDHT dht(DHTPower, DHTPin, dhtType);


// ==========================================================================
//    Apogee SQ-212 Photosynthetically Active Radiation (PAR) Sensor
// ==========================================================================
#include <ApogeeSQ212.h>
const int SQ212Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const int SQ212Data = 2;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
ApogeeSQ212 SQ212(SQ212Power, SQ212Data);


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <BoschBME280.h>
uint8_t BMEi2c_addr = 0x76;  // The BME280 can be addressed either as 0x76 or 0x77
// const int I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
BoschBME280 bme280(I2CPower, BMEi2c_addr);


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <CampbellOBS3.h>
// Campbell OBS 3+ Low Range calibration in Volts
const int OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
const int OBS3Power = 22;  // Pin to switch power on and off (-1 if unconnected)
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C);
// Campbell OBS 3+ High Range calibration in Volts
const int OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C);


// ==========================================================================
//    Decagon 5TM Soil Moisture Sensor
// ==========================================================================
#include <Decagon5TM.h>
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int SDI12Data = 7;  // The pin the 5TM is attached to
const int SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 6;  // The number of readings to average
// const int SDI12Data = 7;  // The pin the CTD is attached to
// const int SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, numberReadings);


// ==========================================================================
//    Decagon ES2 Conductivity and Temperature Sensor
// ==========================================================================
#include <DecagonES2.h>
const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Maxbotix HRXL Ultrasonic Range Finder
// ==========================================================================
#include <MaxBotixSonar.h>

// Define a serial port for receiving data - in this case, using software serial
// Because the standard software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts and define
// the interrrupts for it using the enableInterrup library.

// If enough hardware serial ports are available on your processor, you should
// use one of those instead.  If the proper pins are avaialbe, AltSoftSerial
// by Paul Stoffregen is also superior to SoftwareSerial for this sensor.
// Neither hardware serial nor AltSoftSerial require any modifications to
// deal with interrupt conflicts.

const int SonarTrigger = -1;  // Trigger pin (-1 if unconnected)
const int SonarPower = 22;  // Excite (power) pin (-1 if unconnected)

#if defined __AVR__
#include <SoftwareSerial_ExtInts.h>  // for the stream communication
const int SonarData = 11;     // data receive pin
SoftwareSerial_ExtInts sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx
#else
HardwareSerial &sonarSerial = Serial1;
#endif
MaxBotixSonar sonar(SonarPower, sonarSerial, SonarTrigger) ;


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <MaximDS18.h>
// OneWire Address [array of 8 hex characters]
DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
DeviceAddress OneWireAddress2 = {0x28, 0xFF, 0x57, 0x90, 0x82, 0x16, 0x04, 0x67};
DeviceAddress OneWireAddress3 = {0x28, 0xFF, 0x74, 0x2B, 0x82, 0x16, 0x03, 0x57};
// DeviceAddress OneWireAddress4 = {0x28, 0xFF, 0xB6, 0x6E, 0x84, 0x16, 0x05, 0x9B};
// DeviceAddress OneWireAddress5 = {0x28, 0xFF, 0x3B, 0x07, 0x82, 0x16, 0x13, 0xB3};
const int OneWireBus = 4;  // Pin attached to the OneWire Bus (-1 if unconnected)
const int OneWirePower = 22;  // Pin to switch power on and off (-1 if unconnected)
MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
MaximDS18 ds18_2(OneWireAddress2, OneWirePower, OneWireBus);
MaximDS18 ds18_3(OneWireAddress3, OneWirePower, OneWireBus);
// MaximDS18 ds18_u(OneWirePower, OneWireBus);


// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <YosemitechY504.h>
byte y504modbusAddress = 0x04;  // The modbus address of the Y504
const int modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const int y504NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings

#if defined __AVR__
#include <AltSoftSerial.h>
AltSoftSerial modbusSerial;
#else
HardwareSerial &modbusSerial = Serial1;
#endif
YosemitechY504 y504(y504modbusAddress, modbusPower, modbusSerial, max485EnablePin, y504NumberReadings);


// ==========================================================================
//    Yosemitech Y510 or Y511 Turbidity Sensor
// ==========================================================================
#include <YosemitechY510.h>
byte y510modbusAddress = 0x0B;  // The modbus address of the Y510 or Y511
// const int modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const int y510NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings

// #if defined __AVR__
// #include <AltSoftSerial.h>
// AltSoftSerial modbusSerial;
// #else
// HardwareSerial &modbusSerial = Serial1;
// #endif
YosemitechY510 y510(y510modbusAddress, modbusPower, modbusSerial, max485EnablePin, y510NumberReadings);


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <YosemitechY514.h>
byte y514modbusAddress = 0x14;  // The modbus address of the Y514
// const int modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const int y514NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings

// #if defined __AVR__
// #include <AltSoftSerial.h>
// AltSoftSerial modbusSerial;
// #else
// HardwareSerial &modbusSerial = Serial1;
// #endif
YosemitechY514 y514(y514modbusAddress, modbusPower, modbusSerial, max485EnablePin, y514NumberReadings);


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <YosemitechY520.h>
byte y520modbusAddress = 0x20;  // The modbus address of the Y520
// const int modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const int y520NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings

// #if defined __AVR__
// #include <AltSoftSerial.h>
// AltSoftSerial modbusSerial;
// #else
// HardwareSerial &modbusSerial = Serial1;
// #endif
YosemitechY520 y520(y520modbusAddress, modbusPower, modbusSerial, max485EnablePin, y520NumberReadings);


// ==========================================================================
//    Yosemitech Y532 pH
// ==========================================================================
#include <YosemitechY532.h>
byte y532modbusAddress = 0x32;  // The modbus address of the Y532
// const int modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const int y532NumberReadings = 1;  // The manufacturer actually doesn't mention averaging for this one

// #if defined __AVR__
// #include <AltSoftSerial.h>
// AltSoftSerial modbusSerial;
// #else
// HardwareSerial &modbusSerial = Serial1;
// #endif
YosemitechY532 y532(y532modbusAddress, modbusPower, modbusSerial, max485EnablePin, y532NumberReadings);

// ==========================================================================
//    The array that contains all variables to print out
// ==========================================================================
Variable *variableList[] = {
    new ProcessorStats_Batt(&mayfly),
    new ProcessorStats_FreeRam(&mayfly),
    new MaximDS3231_Temp(&ds3231),
    new ApogeeSQ212_PAR(&SQ212),
    new MaxBotixSonar_Range(&sonar),
    new Decagon5TM_Ea(&fivetm),
    new Decagon5TM_Temp(&fivetm),
    new Decagon5TM_VWC(&fivetm),
    new DecagonES2_Cond(&es2),
    new DecagonES2_Temp(&es2),
    new DecagonCTD_Cond(&ctd),
    new DecagonCTD_Temp(&ctd),
    new DecagonCTD_Depth(&ctd),
    new MaximDS18_Temp(&ds18_1),
    new MaximDS18_Temp(&ds18_2),
    new MaximDS18_Temp(&ds18_3),
    new BoschBME280_Temp(&bme280),
    new BoschBME280_Humidity(&bme280),
    new BoschBME280_Pressure(&bme280),
    new BoschBME280_Altitude(&bme280),
    new AOSongDHT_Humidity(&dht),
    new AOSongDHT_Temp(&dht),
    new AOSongDHT_HI(&dht),
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315),
    new CampbellOBS3_Turbidity(&osb3low, "", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "", "TurbHigh"),
    new YosemitechY504_DOpct(&y504),
    new YosemitechY504_Temp(&y504),
    new YosemitechY504_DOmgL(&y504),
    new YosemitechY510_Turbidity(&y510),
    new YosemitechY510_Temp(&y510),
    new YosemitechY514_Chlorophyll(&y514),
    new YosemitechY514_Temp(&y514),
    new YosemitechY520_Cond(&y520),
    new YosemitechY520_Temp(&y520),
    new YosemitechY532_pH(&y532),
    new YosemitechY532_Temp(&y532),
    new YosemitechY532_Voltage(&y532),
    // new YOUR_variableName_HERE(&)
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


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

// Helper function to get the current date/time from the RTC
// as a unix timestamp - and apply the correct time zone.
long currentepochtime = 0;
uint32_t getNow()
{
  currentepochtime = rtc.now().getEpoch();
  currentepochtime += timeZone*3600;
  return currentepochtime;
}

// This function returns the datetime from the realtime clock as an ISO 8601 formated string
String getDateTime_ISO8601(void)
{
  String dateTimeStr;
  //Create a DateTime object from the current time
  DateTime dt(rtc.makeDateTime(getNow()));
  //Convert it to a String
  dt.addToString(dateTimeStr);
  dateTimeStr.replace(F(" "), F("T"));
  String tzString = String(timeZone);
  if (-24 <= timeZone && timeZone <= -10)
  {
      tzString += F(":00");
  }
  else if (-10 < timeZone && timeZone < 0)
  {
      tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");
  }
  else if (timeZone == 0)
  {
      tzString = F("Z");
  }
  else if (0 < timeZone && timeZone < 10)
  {
      tzString = "+0" + tzString + F(":00");
  }
  else if (10 <= timeZone && timeZone <= 24)
  {
      tzString = "+" + tzString + F(":00");
  }
  dateTimeStr += tzString;
  return dateTimeStr;
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Start the stream for the modbus sensors
    modbusSerial.begin(9600);

    // Start the SoftwareSerial stream for the sonar
    sonarSerial.begin(9600);
    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
    enableInterrupt(SonarData, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif

    // Start the Real Time Clock
    rtc.begin();
    delay(100);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.println(sketchName);
    Serial.print(F("Current Mayfly RTC time is: "));
    Serial.println(getDateTime_ISO8601());
    Serial.print(F("There are "));
    Serial.print(String(variableCount));
    Serial.println(F(" variables to be recorded."));

    // Initialize the sensor array;
    sensors.init(variableCount, variableList);

    // Set up all the sensors
    sensors.setupSensors();

    Serial.println(F("Setup finished!"));
    Serial.println(F("------------------------------------------\n"));
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Print a line to show new reading
    Serial.println(F("------------------------------------------"));
    // Power the sensors;
    digitalWrite(22, HIGH);
    // One second warm-up time
    delay(1000);
    // Turn on the LED to show we're taking a reading
    digitalWrite(greenLED, HIGH);
    // Update the sensor value(s)
    sensors.updateAllSensors();
    // Immediately cut Power to the sensors;
    digitalWrite(22, LOW);
    // Print the data to the screen
    Serial.print(F("Updated all sensors at "));
    Serial.println(getDateTime_ISO8601());
    sensors.printSensorData(&Serial);
    Serial.print(F("In CSV Format:  "));
    Serial.println(sensors.generateSensorDataCSV());
    // Turn off the LED to show we're done with the reading
    digitalWrite(greenLED, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(10000);
}
