#pragma once
#include <SensorBase.h>


// -----------------------------------------------
// 1. Sensor Location Information
// -----------------------------------------------

// Set the pin to read the battery voltage
int batteryPin = A6;

// change to the proper pins for Decagon CTD
// sdi-12 data pin is usually, pin 7 on shield 3.0
const int CTDaddress = 1;  // The SDI-12 Address of the CTD
const int numberReadings = 10;  // The number of readings to average
const int SDI12_PIN = 7;
const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// Change to the proper excite (power) and recieve pin for MaxBotix Sonar.
const int SonarData = 11;
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// change to the proper pins for Decagon 5TM
// sdi-12 data pin is usually, pin 7 on shield 3.0
const int TMaddress = 1;  // The SDI-12 Address of the 5-TM
// const int SDI12_PIN = 7;
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// change to the proper pins for Campbell OSB 3+
const int OSBLowPin = 0;  // The low voltage analog pin
const int OSBHighPin = 1;  // The high voltage analog pin
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// -----------------------------------------------
// 2. Include all sensors and necessary files here
// -----------------------------------------------
#include <MayFlyOnboardSensors.h>
#include <DecagonCTD.h>
#include <Decagon5TM.h>
#include <MaxBotixSonar.h>
#include <CampbellOSB3.h>


// -----------------------------------------------
// 3. Device registration and sampling features
// -----------------------------------------------
// Skecth file name
const char *SKETCH_NAME = "modular_sensors.ino";

// Logger ID, for data file on SD card
const char *LoggerID = "Mayfly_160073";

// The file name to save data on the SD card as
// This MUST be no longer than 8 character + 3 character extension.
const char *FILE_NAME = "MF160073.csv";

// Register your site and get these tokens from data.envirodiy.org
const char *REGISTRATION_TOKEN = "5a3e8d07-8821-4240-91c9-26c610966b2c";
const char *SAMPLING_FEATURE = "39bf098f-d11d-4ea6-9be3-6a073969b019";
const int TIME_ZONE = -5;

const char *UUIDs[] =
{
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9",
"fec11d32-0658-4ef0-8a27-bdffa2104e31", "a7329b1b-b002-4fa8-afba-ae83b82ab8e9"
};

// -----------------------------------------------
// 4. Device Connection Options
// -----------------------------------------------
const char *BEE_TYPE = "GPRS";  // The type of XBee, either "GPRS" or "WIFI"
const char* APN = "apn.konekt.io";  // The APN for the GPRSBee


// -----------------------------------------------
// 5. The array that contains all valid sensors
// -----------------------------------------------
SensorBase* SENSOR_LIST[] = {
    // new DecagonCTD_Cond(numberReadings, CTDaddress, switchedPower, SDI12_PIN),
    // new DecagonCTD_Temp(numberReadings, CTDaddress, switchedPower, SDI12_PIN),
    // new DecagonCTD_Depth(numberReadings, CTDaddress, switchedPower, SDI12_PIN),
    // new Decagon5TM_Temp(TMaddress, switchedPower, SDI12_PIN),
    // new Decagon5TM_Ea(TMaddress, switchedPower, SDI12_PIN),
    // new Decagon5TM_VWC(TMaddress, switchedPower, SDI12_PIN),
    new MaxBotixSonar_Depth(switchedPower, SonarData),
    // new CampbellOSB3_TurbLow(switchedPower, OSBLowPin, OSBHighPin),
    // new CampbellOSB3_TurbHigh(switchedPower, OSBLowPin, OSBHighPin),
    new MayFlyOnboardTemp(batteryPin),
    new MayFlyOnboardBatt(batteryPin)
    // new YOUR_sensorName_HERE()
};


// -----------------------------------------------
// 6. Timing Options For Logging
// -----------------------------------------------
int LOGGING_INTERVAL = 1;  // How frequently (in minutes) to log data
int READ_DELAY = 1;  // How often (in minutes) the timer wakes up
int UPDATE_RATE = 200; // How frequently (in milliseconds) the logger checks if it should log
int COMMAND_TIMEOUT = 15000;  // How long (in milliseconds) to wait for a server response


// -----------------------------------------------
// 7. WebSDL Endpoints for POST requests
// -----------------------------------------------
const char *HOST_ADDRESS = "data.envirodiy.org";
const char *API_ENDPOINT = "/api/data-stream/";


// -----------------------------------------------
// 8. Board setup info
// -----------------------------------------------
const int SERIAL_BAUD = 9600;  // Serial port BAUD rate
const int BEE_BAUD = 9600;  // Bee BAUD rate (9600 is default)
const int BEE_DTR_PIN = 23;  // Bee DTR Pin (Data Terminal Ready - used for sleep)
const int BEE_CTS_PIN = 19;   // Bee CTS Pin (Clear to Send)
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

const int RTC_PIN = A7;  // RTC Interrupt pin
#define RTC_INT_PERIOD EveryMinute  //The interrupt period on the RTC

const int SD_SS_PIN = 12;  // SD Card Pin
