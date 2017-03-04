#pragma once
#include <SensorBase.h>


// -----------------------------------------------
// 1. Sensor pin locations, addresses and related settings
// -----------------------------------------------

// Mayfly version number
const char *MFVersion = "v0.3";

// Decagon CTD: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 10;  // The number of readings to average
const int SDI12Data = 7;  // The pin the CTD is attached to
const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// Decagon 5TM: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// Decagon ES2: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *ES2DI12address = "3";  // The SDI-12 Address of the 5-TM
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// MaxBotix Sonar: pin settings
// The power must be continually on for the MaxBotix.
const int SonarData = 10;     // recieve pin
// const int SonarExcite = 11;   // excite (power) pin
const int SonarExcite = 22;   // excite (power) pin
const int SonarTrigger = 11;   // excite (power) pin

// Campbell OBS 3+: pin settings
//   Campbell OBS 3+ Low Range calibration
const int OBSLowPin = 0;  // The low voltage analog pin
const float OBSLow_A = -2.4763E-07;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 1.0569E-01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -2.9928E-01;  // The "C" value from the low range calibration
//   Campbell OBS 3+ High Range calibration
const int OBSHighPin = 1;  // The high voltage analog pin
const float OBSHigh_A = 3.5310E-05;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 4.0111E-01;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = 2.0709E-01;  // The "C" value from the high range calibration
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly


// -----------------------------------------------
// 2. Include all required libraries
// -----------------------------------------------
#include <MayflyOnboardSensors.h>
// #include <DecagonCTD.h>
// #include <Decagon5TM.h>
// #include <DecagonES2.h>
// #include <CampbellOBS3.h>
#include <MaxBotixSonar.h>


// -----------------------------------------------
// 3. Device registration and sampling features
// -----------------------------------------------
// Skecth file name
const char *SKETCH_NAME = "modular_sensors.ino";

// Logger ID, for data file on SD card
const char *LoggerID = "Mayfly_160073";

// The header for file name to save data on the SD card as
// The date the file was started will be appended to this .
const char *FILE_NAME = "MF160073";

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
    // new DecagonCTD_Depth(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    // new DecagonCTD_Temp(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    // new DecagonCTD_Cond(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    // new Decagon5TM_Ea(*TMSDI12address, switchedPower, SDI12Data),
    // new Decagon5TM_Temp(*TMSDI12address, switchedPower, SDI12Data),
    // new Decagon5TM_VWC(*TMSDI12address, switchedPower, SDI12Data),
    // new DecagonES2_Cond(*ES2DI12address, switchedPower, SDI12Data),
    // new DecagonES2_Temp(*ES2DI12address, switchedPower, SDI12Data),
    new MaxBotixSonar_Range(SonarExcite, SonarData, SonarTrigger),
    // new CampbellOBS3_Turbidity(switchedPower, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C),
    // new CampbellOBS3_TurbHigh(switchedPower, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C),
    new MayflyOnboardTemp(MFVersion),
    new MayflyOnboardBatt(MFVersion),
    new MayflyFreeRam()
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
