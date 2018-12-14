/*****************************************************************************
simple_logging.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.18.0

This sketch is an example of logging data to an SD card

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerBase.h>


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "simple_logging.ino";
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

const long serialBaud = 115200;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int8_t redLED = 9;  // Pin for the red LED (-1 if unconnected)
const int8_t buttonPin = 21;  // Pin for a button to use to enter debugging mode (-1 if unconnected)
const int8_t wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

// Create and return the processor "sensor"
const char *MFVersion = "v0.5b";
ProcessorStats mayfly(MFVersion);


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>
const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302) Digital Humidity and Temperature
// ==========================================================================
#include <sensors/AOSongDHT.h>
const int8_t DHTPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t DHTPin = 10;  // DHT data pin
DHTtype dhtType = DHT11;  // DHT type, either DHT11, DHT21, or DHT22
// Create and return the AOSong DHT sensor object
AOSongDHT dht(DHTPower, DHTPin, dhtType);


// ==========================================================================
//    Apogee SQ-212 Photosynthetically Active Radiation (PAR) Sensor
// ==========================================================================
#include <sensors/ApogeeSQ212.h>
const int8_t SQ212Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SQ212Data = 2;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
const uint8_t SQ212_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Create and return the Apogee SQ212 sensor object
ApogeeSQ212 SQ212(SQ212Power, SQ212Data);


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>
uint8_t BMEi2c_addr = 0x76;  // The BME280 can be addressed either as 0x76 or 0x77
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>
const int8_t OBS3Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
const uint8_t OBS3_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
// Create and return the Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C, OBS3_ADS1115Address, OBS3numberReadings);
// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
// Create and return the Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C, OBS3_ADS1115Address, OBS3numberReadings);


// ==========================================================================
//    Decagon 5TM Soil Moisture Sensor
// ==========================================================================
#include <sensors/Decagon5TM.h>
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int8_t SDI12Data = 7;  // The pin the 5TM is attached to
const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Decagon 5TM sensor object
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDnumberReadings = 6;  // The number of readings to average
// const int8_t SDI12Data = 7;  // The pin the CTD is attached to
// const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDnumberReadings);


// ==========================================================================
//    Decagon ES2 Conductivity and Temperature Sensor
// ==========================================================================
#include <sensors/DecagonES2.h>
const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int8_t SDI12Data = 7;  // The pin the ES2 is attached to
// const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t ES2NumberReadings = 3;
// Create and return the Decagon ES2 sensor object
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data, ES2NumberReadings);


// ==========================================================================
//    External Voltage via TI ADS1115
// ==========================================================================
#include <sensors/ExternalVoltage.h>
const int8_t VoltPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t VoltData = 0;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float VoltGain = 10; // Default 1/gain for grove voltage divider is 10x
const uint8_t Volt_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
const uint8_t VoltReadsToAvg = 1; // Only read one sample
// Create and return the External Voltage sensor object
ExternalVoltage extvolt(VoltPower, VoltData, VoltGain, Volt_ADS1115Address, VoltReadsToAvg);


// ==========================================================================
//    Freescale Semiconductor MPL115A2 Barometer
// ==========================================================================
#include <sensors/FreescaleMPL115A2.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MPL115A2ReadingsToAvg = 1;
// Create and return the MPL115A2 barometer sensor object
MPL115A2 mpl115a2(I2CPower, MPL115A2ReadingsToAvg);


// ==========================================================================
//    Maxbotix HRXL Ultrasonic Range Finder
// ==========================================================================

// Set up a serial port for receiving sonar data - in this case, using software serial
// Because the standard software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts and define
// the interrrupts for it using the enableInterrup library.

// If enough hardware serial ports are available on your processor, you should
// use one of those instead.  If the proper pins are avaialbe, AltSoftSerial
// by Paul Stoffregen is also superior to SoftwareSerial for this sensor.
// Neither hardware serial nor AltSoftSerial require any modifications to
// deal with interrupt conflicts.

const int8_t SonarData = 11;     // data receive pin

#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx

// #include <NeoSWSerial.h>  // for the stream communication
// NeoSWSerial sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx
// void NeoSWSISR()
// {
//   NeoSWSerial::rxISR( *portInputRegister( digitalPinToPort( SonarData ) ) );
// }

#include <sensors/MaxBotixSonar.h>
const int8_t SonarPower = 22;  // Excite (power) pin (-1 if unconnected)
const int8_t Sonar1Trigger = A1;  // Trigger pin (a negative number if unconnected) (A1 = 25)
const int8_t Sonar2Trigger = A2;  // Trigger pin (a negative number if unconnected) (A2 = 26)
// Create and return the MaxBotix Sonar sensor object
MaxBotixSonar sonar1(sonarSerial, SonarPower, Sonar1Trigger) ;
MaxBotixSonar sonar2(sonarSerial, SonarPower, Sonar2Trigger) ;


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <sensors/MaximDS18.h>
// OneWire Address [array of 8 hex characters]
DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
DeviceAddress OneWireAddress2 = {0x28, 0xFF, 0x57, 0x90, 0x82, 0x16, 0x04, 0x67};
DeviceAddress OneWireAddress3 = {0x28, 0xFF, 0x74, 0x2B, 0x82, 0x16, 0x03, 0x57};
DeviceAddress OneWireAddress4 = {0x28, 0xFF, 0xB6, 0x6E, 0x84, 0x16, 0x05, 0x9B};
DeviceAddress OneWireAddress5 = {0x28, 0xFF, 0x3B, 0x07, 0x82, 0x16, 0x03, 0xB3};
const int8_t OneWireBus = A0;  // Pin attached to the OneWire Bus (-1 if unconnected)
const int8_t OneWirePower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Maxim DS18 sensor objects (use this form for a known address)
MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
MaximDS18 ds18_2(OneWireAddress2, OneWirePower, OneWireBus);
MaximDS18 ds18_3(OneWireAddress3, OneWirePower, OneWireBus);
MaximDS18 ds18_4(OneWireAddress4, OneWirePower, OneWireBus);
MaximDS18 ds18_5(OneWireAddress5, OneWirePower, OneWireBus);
// Create and return the Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
// MaximDS18 ds18_u(OneWirePower, OneWireBus);


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <sensors/MeaSpecMS5803.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 or 0x77
const int16_t MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;
// Create and return the MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);


// ==========================================================================
//    External I2C Rain Tipping Bucket Counter
// ==========================================================================
#include <sensors/RainCounterI2C.h>
const uint8_t RainCounterI2CAddress = 0x08;  // I2C Address for external tip counter
const float depthPerTipEvent = 0.2;  // rain depth in mm per tip event
// Create and return the Rain Counter sensor object
RainCounterI2C tbi2c(RainCounterI2CAddress, depthPerTipEvent);


// Set up a serial port for modbus communication - in this case, using AltSoftSerial
#include <AltSoftSerial.h>
AltSoftSerial modbusSerial;

// ==========================================================================
//    Keller Acculevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#include <sensors/KellerAcculevel.h>
byte acculevelModbusAddress = 0x01;  // The modbus address of KellerAcculevel
const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t acculevelNumberReadings = 5;  // The manufacturer recommends taking and averaging a few readings
// Create and return the Keller Acculevel sensor object
KellerAcculevel acculevel(acculevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, acculevelNumberReadings);

// ==========================================================================
//    Keller Nanolevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#include <sensors/KellerNanolevel.h>
byte nanolevelModbusAddress = 0x01;  // The modbus address of KellerNanolevel
// const int8_t rs485AdapterPower = -1;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = -1;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t nanolevelNumberReadings = 3;  // The manufacturer recommends taking and averaging a few readings
// Create and return the Keller Nanolevel sensor object
KellerNanolevel nanolevel(nanolevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, nanolevelNumberReadings);

// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/YosemitechY504.h>
byte y504ModbusAddress = 0x04;  // The modbus address of the Y504
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y504NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Yosemitech Y504 dissolved oxygen sensor object
YosemitechY504 y504(y504ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y504NumberReadings);


// ==========================================================================
//    Yosemitech Y510 Turbidity Sensor
// ==========================================================================
#include <sensors/YosemitechY510.h>
byte y510ModbusAddress = 0x0B;  // The modbus address of the Y510
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y510NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y510-B Turbidity sensor object
YosemitechY510 y510(y510ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y510NumberReadings);


// ==========================================================================
//    Yosemitech Y511 Turbidity Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY511.h>
byte y511ModbusAddress = 0x1A;  // The modbus address of the Y511
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y511NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y511-A Turbidity sensor object
YosemitechY511 y511(y511ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y511NumberReadings);


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <sensors/YosemitechY514.h>
byte y514ModbusAddress = 0x14;  // The modbus address of the Y514
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y514NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y514 chlorophyll sensor object
YosemitechY514 y514(y514ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y514NumberReadings);


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <sensors/YosemitechY520.h>
byte y520ModbusAddress = 0x20;  // The modbus address of the Y520
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y520NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y520 conductivity sensor object
YosemitechY520 y520(y520ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y520NumberReadings);


// ==========================================================================
//    Yosemitech Y532 pH
// ==========================================================================
#include <sensors/YosemitechY532.h>
byte y532ModbusAddress = 0x32;  // The modbus address of the Y532
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y532NumberReadings = 1;  // The manufacturer actually doesn't mention averaging for this one
// Create and return the Yosemitech Y532 pH sensor object
YosemitechY532 y532(y532ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y532NumberReadings);


// ==========================================================================
//    Yosemitech Y550 COD Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY550.h>
byte y550ModbusAddress = 0x50;  // The modbus address of the Y550
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y550NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y550 conductivity sensor object
YosemitechY550 y550(y550ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y550NumberReadings);


// ==========================================================================
//    Yosemitech Y4000 Multiparameter Sonde (DOmgL, Turbidity, Cond, pH, Temp, ORP, Chlorophyll, BGA)
// ==========================================================================
#include <sensors/YosemitechY4000.h>
byte y4000ModbusAddress = 0x05;  // The modbus address of the Y4000
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y4000NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Yosemitech Y4000 multi-parameter sensor object
YosemitechY4000 y4000(y4000ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y4000NumberReadings);


// ==========================================================================
//    Zebra Tech D-Opto Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/ZebraTechDOpto.h>
const char *DOptoDI12address = "5";  // The SDI-12 Address of the Zebra Tech D-Opto
// const int8_t SDI12Data = 7;  // The pin the D-Opto is attached to
// const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Zebra Tech DOpto dissolved oxygen sensor object
ZebraTechDOpto dopto(*DOptoDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
// Create pointers for all of the variables from the sensors
// at the same time putting them into an array
Variable *variableList[] = {
    new ProcessorStats_SampleNumber(&mayfly),
    new ApogeeSQ212_PAR(&SQ212),
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315),
    new AOSongDHT_Humidity(&dht),
    new AOSongDHT_Temp(&dht),
    new AOSongDHT_HI(&dht),
    new BoschBME280_Temp(&bme280),
    new BoschBME280_Humidity(&bme280),
    new BoschBME280_Pressure(&bme280),
    new BoschBME280_Altitude(&bme280),
    new CampbellOBS3_Turbidity(&osb3low, "", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "", "TurbHigh"),
    new Decagon5TM_Ea(&fivetm),
    new Decagon5TM_Temp(&fivetm),
    new Decagon5TM_VWC(&fivetm),
    new DecagonCTD_Cond(&ctd),
    new DecagonCTD_Temp(&ctd),
    new DecagonCTD_Depth(&ctd),
    new DecagonES2_Cond(&es2),
    new DecagonES2_Temp(&es2),
    new ExternalVoltage_Volt(&extvolt),
    new MaxBotixSonar_Range(&sonar1),
    new MaxBotixSonar_Range(&sonar2),
    new MaximDS18_Temp(&ds18_1),
    new MaximDS18_Temp(&ds18_2),
    new MaximDS18_Temp(&ds18_3),
    new MaximDS18_Temp(&ds18_4),
    new MaximDS18_Temp(&ds18_5),
    // new MaximDS18_Temp(&ds18_u),
    new MeaSpecMS5803_Temp(&ms5803),
    new MeaSpecMS5803_Pressure(&ms5803),
    new MPL115A2_Temp(&mpl115a2),
    new MPL115A2_Pressure(&mpl115a2),
    new RainCounterI2C_Tips(&tbi2c),
    new RainCounterI2C_Depth(&tbi2c),
    new KellerAcculevel_Pressure(&acculevel),
    new KellerAcculevel_Temp(&acculevel),
    new KellerAcculevel_Height(&acculevel),
    new KellerNanolevel_Pressure(&nanolevel),
    new KellerNanolevel_Temp(&nanolevel),
    new KellerNanolevel_Height(&nanolevel),
    new YosemitechY504_DOpct(&y504),
    new YosemitechY504_Temp(&y504),
    new YosemitechY504_DOmgL(&y504),
    new YosemitechY510_Temp(&y510),
    new YosemitechY510_Turbidity(&y510),
    new YosemitechY511_Temp(&y511),
    new YosemitechY511_Turbidity(&y511),
    new YosemitechY514_Temp(&y514),
    new YosemitechY514_Chlorophyll(&y514),
    new YosemitechY520_Temp(&y520),
    new YosemitechY520_Cond(&y520),
    new YosemitechY532_Temp(&y532),
    new YosemitechY532_Voltage(&y532),
    new YosemitechY532_pH(&y532),
    new YosemitechY4000_DOmgL(&y4000),
    new YosemitechY4000_Turbidity(&y4000),
    new YosemitechY4000_Cond(&y4000),
    new YosemitechY4000_pH(&y4000),
    new YosemitechY4000_Temp(&y4000),
    new YosemitechY4000_ORP(&y4000),
    new YosemitechY4000_Chlorophyll(&y4000),
    new YosemitechY4000_BGA(&y4000),
    new ZebraTechDOpto_Temp(&dopto),
    new ZebraTechDOpto_DOpct(&dopto),
    new ZebraTechDOpto_DOmgL(&dopto),
    new ProcessorStats_FreeRam(&mayfly),
    new ProcessorStats_Batt(&mayfly),
    new MaximDS3231_Temp(&ds3231),
    // new YOUR_variableName_HERE(&)
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray varArray(variableCount, variableList);
// Create a new logger instance
Logger logger(LoggerID, loggingInterval, sdCardPin, wakePin, &varArray);


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

    // Start the stream for the modbus sensors
    modbusSerial.begin(9600);

    // Start the SoftwareSerial stream for the sonar
    sonarSerial.begin(9600);
    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
        enableInterrupt(SonarData, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif
    #if defined NeoSWSerial_h
        enableInterrupt(SonarData, NeoSWSISR, CHANGE);
    #endif

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Set information pins
    logger.setAlertPin(greenLED);
    logger.setTestingModePin(buttonPin);

    // Begin the logger
    logger.begin();
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Log the data
    logger.logData();
}
