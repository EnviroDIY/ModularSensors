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
// #define TINY_GSM_MODEM_SIM800  // Select for anything using a SIM800, SIM900, or variant thereof: Sodaq GPRSBees, Microduino GPRS chips, Adafruit Fona, etc
// #define TINY_GSM_MODEM_A6  // Select for A6 or A7 chips
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand XBee's, including WiFi or LTE-M1

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#define MODULAR_SENSORS_OUTPUT Serial  // Without this there will be no output
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#ifdef DreamHostPortalRX
#include <LoggerDreamHost.h>
#else
#include <LoggerEnviroDIY.h>
#endif

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "logging_to_EnviroDIY.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "Mayfly_160073";
// How frequently (in minutes) to log data
int LOGGING_INTERVAL = 1;
// Your logger's timezone.
const int TIME_ZONE = -5;
// Create a new logger instance
#ifdef DreamHostPortalRX
LoggerDreamHost EnviroDIYLogger;
#else
LoggerEnviroDIY EnviroDIYLogger;
#endif

// ==========================================================================
//    AOSong AM2315
// ==========================================================================
#include <AOSongAM2315.h>
const int I2CPower = 22;  // switched sensor power is pin 22 on Mayfly
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302)
// ==========================================================================
#include <AOSongDHT.h>
const int DHTPower = 22;  // switched sensor power is pin 22 on Mayfly
const int DHTPin = 6;
DHTtype dhtType = DHT11;    // Select DHT type, either DHT11, DHT21, or DHT22
AOSongDHT dht(DHTPower, DHTPin, dhtType);


// ==========================================================================
//    Apogee SQ-212
// ==========================================================================
#include <ApogeeSQ212.h>
const int SQ212Power = 22;  // switched sensor power is pin 22 on Mayfly
const int SQ212Data = 2;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
ApogeeSQ212 SQ212(SQ212Power, SQ212Data);


// ==========================================================================
//    Bosch BME280
// ==========================================================================
#include <BoschBME280.h>
uint8_t BMEi2c_addr = 0x76;  // The BME280 can be addressed either as 0x76 or 0x77
// const int I2CPower = 22;  // switched sensor power is pin 22 on Mayfly
BoschBME280 bme280(I2CPower, BMEi2c_addr);


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+
// ==========================================================================
#include <CampbellOBS3.h>
// Campbell OBS 3+ Low Range calibration in Volts
const int OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
const int OBS3Power = 22;  // switched sensor power is pin 22 on Mayfly
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C);
// Campbell OBS 3+ High Range calibration in Volts
const int OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C);


// ==========================================================================
//    Decagon 5TM
// ==========================================================================
#include <Decagon5TM.h>
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int SDI12Data = 7;  // The pin the 5TM is attached to
const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Decagon CTD
// ==========================================================================
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 6;  // The number of readings to average
// const int SDI12Data = 7;  // The pin the CTD is attached to
// const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, numberReadings);


// ==========================================================================
//    Decagon ES2
// ==========================================================================
#include <DecagonES2.h>
const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Maxbotix HRXL
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

const int SonarData = 11;     // data  pin
const int SonarTrigger = -1;   // Trigger pin
const int SonarPower = 22;   // excite (power) pin

#if defined __AVR__
#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx
MaxBotixSonar sonar(SonarPower, sonarSerial, SonarTrigger) ;

#else
HardwareSerial &sonarSerial = Serial1;
MaxBotixSonar sonar(SonarPower, sonarSerial, SonarTrigger) ;
#endif


// ==========================================================================
//    Maxim DS18 Temperature
// ==========================================================================
#include <MaximDS18.h>
// OneWire Address [array of 8 hex characters]
DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
DeviceAddress OneWireAddress2 = {0x28, 0xFF, 0x57, 0x90, 0x82, 0x16, 0x04, 0x67};
DeviceAddress OneWireAddress3 = {0x28, 0xFF, 0x74, 0x2B, 0x82, 0x16, 0x03, 0x57};
// DeviceAddress OneWireAddress4 = {0x28, 0xFF, 0xB6, 0x6E, 0x84, 0x16, 0x05, 0x9B};
// DeviceAddress OneWireAddress5 = {0x28, 0xFF, 0x3B, 0x07, 0x82, 0x16, 0x13, 0xB3};
const int OneWireBus = 4;   // Data pin
const int OneWirePower = 22;   // Power pin
MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
MaximDS18 ds18_2(OneWireAddress2, OneWirePower, OneWireBus);
MaximDS18 ds18_3(OneWireAddress3, OneWirePower, OneWireBus);
// MaximDS18 ds18_u(OneWirePower, OneWireBus);


// ==========================================================================
//    EnviroDIY Mayfly
// ==========================================================================
#include <MayflyOnboardSensors.h>
const char *MFVersion = "v0.5";
EnviroDIYMayfly mayfly(MFVersion) ;

// ---------------------------------------------------------------------------
// The array that contains all valid variables
// ---------------------------------------------------------------------------
Variable *variableList[] = {
    new EnviroDIYMayfly_Batt(&mayfly),
    new EnviroDIYMayfly_FreeRam(&mayfly),
    new EnviroDIYMayfly_Temp(&mayfly),
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
    new CampbellOBS3_Turbidity(&osb3low, "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "TurbHigh"),
    new Modem_RSSI(&EnviroDIYLogger.modem),
    new Modem_SignalPercent(&EnviroDIYLogger.modem),
    // new YOUR_variableName_HERE(&)
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


// ---------------------------------------------------------------------------
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
//   You can copy the entire code snippet directly into this block below.
// ---------------------------------------------------------------------------
const char *REGISTRATION_TOKEN = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *SAMPLING_FEATURE = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
const char *UUIDs[] =                                                      // UUID array for device sensors
{
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab"
};


// ---------------------------------------------------------------------------
// Device Connection Options and WebSDL Endpoints for POST requests
// ---------------------------------------------------------------------------
const int modemDTRPin = 23;  // Modem DTR Pin (Data Terminal Ready - used for sleep) (-1 if unconnected)
const int modemCTSPin = 19;   // Modem CTS Pin (Clear to Send) (-1 if unconnected)
const int modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (else -1)

DTRSleepType ModemSleepMode = held;  // How the modem is put to sleep
// Use "held" if the DTR pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.
// Use "pulsed" if the DTR pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.
// Use "reverse" if the DTR pin is held LOW to keep the modem awake, as with all XBees.
// Use "always_on" if you do not want the library to control the modem power and sleep or if none of the above apply.
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.
const long ModemBaud = 9600;  // Modem BAUD rate (9600 is default), can use higher for SIM800 (19200 works)
const char *APN = "apn.konekt.io";  // The APN for the gprs connection, unnecessary for WiFi
const char *SSID = "XXXXXXX";  // The WiFi access point, unnecessary for gprs
const char *PWD = "XXXXXXX";  // The password for connecting to WiFi, unnecessary for gprs


// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const long SERIAL_BAUD = 57600;  // Serial port baud rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED
const int BUTTON_PIN = 21;  // Pin for the button
const int RTC_PIN = A7;  // RTC Interrupt/Alarm pin
const int SD_SS_PIN = 12;  // SD Card Chip Select/Slave Select Pin


// ---------------------------------------------------------------------------
// Working Functions
// ---------------------------------------------------------------------------

// Flashes to Mayfly's LED's
void greenredflash(int numFlash = 4)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(75);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(75);
  }
  digitalWrite(RED_LED, LOW);
}


// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);
    // Start the serial connection with the *bee
    ModemSerial.begin(ModemBaud);
    // Start the stream for the sonar
    sonarSerial.begin(9600);
    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
    enableInterrupt(SonarData, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(SKETCH_NAME);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    Logger::setTimeZone(TIME_ZONE);  // Logging in the given time zone
    // EnviroDIYLogger.setTZOffset(0);  // Also set the clock in that time zone
    Logger::setTZOffset(TIME_ZONE);  // Set the clock in UTC

    // Initialize the logger;
    EnviroDIYLogger.init(SD_SS_PIN, RTC_PIN, variableCount, variableList,
                LOGGING_INTERVAL, LoggerID);
    EnviroDIYLogger.setAlertPin(GREEN_LED);

    // Set up the connection with EnviroDIY
    EnviroDIYLogger.setToken(REGISTRATION_TOKEN);
    EnviroDIYLogger.setSamplingFeature(SAMPLING_FEATURE);
    EnviroDIYLogger.setUUIDs(UUIDs);

    #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
        EnviroDIYLogger.modem.setupModem(&ModemSerial, modemVCCPin, modemCTSPin, modemDTRPin, ModemSleepMode, SSID, PWD);
    #else
        EnviroDIYLogger.modem.setupModem(&ModemSerial, modemVCCPin, modemCTSPin, modemDTRPin, ModemSleepMode, APN);
    #endif

    #ifdef DreamHostPortalRX
        EnviroDIYLogger.setDreamHostPortalRX(DreamHostPortalRX);
    #endif

    // Begin the logger
    EnviroDIYLogger.begin();

    // Check for debugging mode
    EnviroDIYLogger.checkForDebugMode(BUTTON_PIN, &Serial);
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Log the data
    EnviroDIYLogger.log();
}
