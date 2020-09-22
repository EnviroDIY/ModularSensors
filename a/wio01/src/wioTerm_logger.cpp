
/*
 Name:		wioTerm_ntp.ino
 Sensors:
 Version:   2.0.0nh
 Created:	9/7/2020 04:30:00 PM
 Author:	Jim Hamilton modified Neil Hancock Sept/7/2020
 Company:   Sannox Systems Pty Ltd
 Details:   Example of setting a rtc via ntp using the Wio Terminal

******* Updates *******

Date:

        2020-07-09
        + initial code 

Notes:
        Uses Adafruit RTClib for DateTime functions and rtc control
        https://github.com/adafruit/RTClib

        Uses millisDelay for non blocking timers
        https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
        add to lib/
        Example based on the Arduino WifiUdpNtpClient example

        NTP servers can be called via name or ip address, use only servers that can
        repsond to IPv4 requests.

*/

// switch between local and remote time servers
// comment out to use remote server
//#define USELOCALNTP


//https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
#include <millisDelay.h>
#include <Wire.h>
#include <RTClib.h>
//#include "RTC_SAMD51.h"
#include <RTCZero.h>
//#define RADIO_WIFI
#if defined RADIO_WIFI
#include "ntpHelper.h"
ntpHelper ntph;
#else 
#define ntph
#endif //
#include "uiHelper.h"


uiHelper ui_display;


#if defined RADIO_WIFI
const char ssid[] = "ArthurGuestSsid"; // add your required ssid
const char password[] = "guest1234";//"your-passowrd"; // add your own netywork password
#endif // RADIO_WIFI

millisDelay updateDelay; //ntp periodic update.

DateTime now_dt; // time object


// localtime
unsigned long devicetime;
uint32_t readings_cnt =0;

//RTC_DS3231 rtcPhy;
//RTC_SAMD51 rtcPhy; .. in .h
RTCZero zero_sleep_rtc;

// for use by the Adafuit RTClib library
//char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


void readData() {
    //Temperature1 1W

    //Temperature2 Analog
    //Temperature3 Analog

    //AM2302 https://github.com/Seeed-Studio/Grove_Temperature_And_Humidity_Sensor
}


///***************************************************************
#if 0
#include "FreeRam.h"

void printFree() {

    Serial.print("[");
    Serial.print(FreeRam());
    Serial.print("]");
}
uint32_t heap_start;
#else 
#define printFree()
#endif //0
void setup() {

    Serial.begin(115200);

    while (!Serial); // wait for serial port to connect. Needed for native USB
    printFree();
    ui_display.begin();
    ui_display.fillscreen("Modular Sensors");
#if defined RADIO_WIFI
    // setup network before rtc check 
    ntph.connectToWiFi(ssid, password);

    // get the time via NTP (udp) call to time server
    // getNTPtime returns epoch UTC time adjusted for timezone but not daylight savings
    // time
    devicetime = ntph.getNTPtime();
#endif // RADIO_WIFI
    zero_sleep_rtc.begin();
    #if 0
    if (!rtcPhy.begin()) {
        Serial.println("Couldn't find RTC");
        while (1) {
            delay(5000); // stop operating
            Serial.print(" Halted. No RTC!");
        }
    }
    #endif //00
    DateTime ccTimeTZ(__DATE__, __TIME__);
    // check if rtc has lost power i.e. battery not present or flat or new device
    //now_dt = rtcPhy.now();
    now_dt= zero_sleep_rtc.getEpoch();
    if (! now_dt.isValid() ) {
        Serial.print("RTC lost power, set the time to ");
        // When time needs to be set on a new device, or after a power loss, 
        //DateTime ntp_dt(devicetime);
        zero_sleep_rtc.setTime(ccTimeTZ.hour(), ccTimeTZ.minute(), ccTimeTZ.second());
        zero_sleep_rtc.setDate(ccTimeTZ.date(), ccTimeTZ.month(), ccTimeTZ.year() - 2000);
        now_dt = zero_sleep_rtc.getEpoch();
        //Serial.println(now_dt.timestamp(DateTime::TIMESTAMP_FULL));        
    }
    // get and print the current rtc time
    //now_dt = rtcPhy.now();
    Serial.print("RTC time is: ");

    Serial.println(now_dt.timestamp(DateTime::TIMESTAMP_FULL));

    // adjust time using ntp time
    //rtcPhy.adjust(DateTime(devicetime));

    // print boot update details
    //Serial.println("RTC (boot) time updated.");
    // get and print the adjusted rtc time
    //now_dt = rtcPhy.now();
    //Serial.print("Adjusted RTC (boot) time is: ");
    //Serial.println(now_dt.timestamp(DateTime::TIMESTAMP_FULL));
    ui_display.fillscreen(now_dt.timestamp(DateTime::TIMESTAMP_FULL).c_str());

    // start millisdelays timers as required, adjust to suit requirements
    //updateDelay.start(12 * 60 * 60 * 1000); // update time via ntp every 12 hrs
    #define UPDATE_MINUTES 5
    Serial.print("Update every mins: ");
    Serial.println(UPDATE_MINUTES);
    updateDelay.start(UPDATE_MINUTES*60 * 1000); // update time via ntp

}

void loop() {
    //#define TMPBUF_SZ 37
    //char tmpBuf[TMPBUF_SZ];

    if (updateDelay.justFinished()) { // 12 hour loop
        
        updateDelay.repeat(); // timer

        Serial.print(++readings_cnt);
        Serial.print("[");
        printFree();
        Serial.print("]");
#if defined RADIO_WIFI
        // update rtc time
        devicetime = ntph.getNTPtime();
        #else
        devicetime = zero_sleep_rtc.getEpoch();
        #endif //RADIO_WIFI
        if (devicetime == 0) {
            Serial.println(" Failed to get time from network time server.");
        }
        else {
            //rtcPhy.adjust(DateTime(devicetime));
            //Serial.println("");

            //Serial.print("rtc time updated.");
            // get and print the adjusted rtc time
#if defined RADIO_WIFI
            now_dt = rtcPhy.now();
#else
            now_dt = zero_sleep_rtc.getEpoch();
#endif //RADIO_WIFI
            Serial.print(" time is: ");
            Serial.println(now_dt.timestamp(DateTime::TIMESTAMP_FULL));
            readData();

            ui_display.update3(now_dt.timestamp(DateTime::TIMESTAMP_FULL).c_str(),1.0,2.0,3.0);
        }
    }
}








