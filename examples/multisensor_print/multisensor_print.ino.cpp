# 1 "c:\\users\\sdamia~1.str\\appdata\\local\\temp\\tmpamqu9y"
#include <Arduino.h>
# 1 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
# 37 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <avr/sleep.h>

#include <SPI.h>

#include <SdFat.h>

#include <RTCTimer.h>

#include <Sodaq_DS3231.h>

#include <Sodaq_PcInt_PCINT0.h>

#include <GPRSbee.h>

#include "Config.h"





RTCTimer timer;





long currentepochtime = 0;

char currentTime[26] = "";





SdFat SD;

String fileName = String(FILE_NAME);





int sensorCount = 0;
# 95 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
int freeRam ();
uint32_t getNow();
String getDateTime_ISO8601(void);
bool setupSensors();
void setupLogFile();
void greenred4flash();
bool updateAllSensors();
String generateSensorDataCSV(void);
String checkSensorLocations(void);
void logData(String rec);
void setup();
void loop();
#line 95 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
int freeRam ()

{

  extern int __heap_start, *__brkval;

  int v;

  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

}







uint32_t getNow()

{

  currentepochtime = rtc.now().getEpoch();

  currentepochtime += TIME_ZONE*3600;

  return currentepochtime;

}





String getDateTime_ISO8601(void)

{

  String dateTimeStr;



  DateTime dt(rtc.makeDateTime(getNow()));



  dt.addToString(dateTimeStr);

  dateTimeStr.replace(F(" "), F("T"));

  String tzString = String(TIME_ZONE);

  if (-24 <= TIME_ZONE && TIME_ZONE <= -10)

  {

      tzString += F(":00");

  }

  else if (-10 < TIME_ZONE && TIME_ZONE < 0)

  {

      tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");

  }

  else if (TIME_ZONE == 0)

  {

      tzString = F("Z");

  }

  else if (0 < TIME_ZONE && TIME_ZONE < 10)

  {

      tzString = "+0" + tzString + F(":00");

  }

  else if (10 <= TIME_ZONE && TIME_ZONE <= 24)

  {

      tzString = "+" + tzString + F(":00");

  }

  dateTimeStr += tzString;

  return dateTimeStr;

}





bool setupSensors()

{

    bool success = true;

    for (int i = 0; i < sensorCount; i++)

    {

        success &= SENSOR_LIST[i]->setup();

    }



    return success;

}





void setupLogFile()

{



    if (!SD.begin(SD_SS_PIN))

    {Serial.println(F("Error: SD card failed to initialise or is missing.")); }



  fileName += String(LoggerID) + F("_") + getDateTime_ISO8601().substring(0,10) + F(".txt");



  bool oldFile = SD.exists(fileName.c_str());





  File logFile = SD.open(fileName, FILE_WRITE);





  if (!oldFile)

  {

    logFile.println(LoggerID);

    logFile.print(F("Sampling Feature UUID: "));

    logFile.println(SAMPLING_FEATURE);



    String dataHeader = F("\"Timestamp\", ");

    for (int i = 0; i < sensorCount; i++)

    {

        dataHeader += "\"" + String(SENSOR_LIST[i]->getSensorName());

        dataHeader += " " + String(SENSOR_LIST[i]->getVarName());

        dataHeader += " " + String(SENSOR_LIST[i]->getVarUnit());

        dataHeader += " (" + String(UUIDs[i]) + ")\"";

        if (i + 1 != sensorCount)

        {

            dataHeader += F(", ");

        }

    }





    logFile.println(dataHeader);

  }





  logFile.close();

}





void greenred4flash()

{

  for (int i = 1; i <= 4; i++) {

    digitalWrite(GREEN_LED, HIGH);

    digitalWrite(RED_LED, LOW);

    delay(50);

    digitalWrite(GREEN_LED, LOW);

    digitalWrite(RED_LED, HIGH);

    delay(50);

  }

  digitalWrite(RED_LED, LOW);

}





bool updateAllSensors()

{



    getDateTime_ISO8601().toCharArray(currentTime, 26) ;



    bool success = true;

    for (int i = 0; i < sensorCount; i++)

    {

        success &= SENSOR_LIST[i]->update();



        Serial.print(F("--- Updated "));

        Serial.print(SENSOR_LIST[i]->getSensorName());

        Serial.print(F(" for "));

        Serial.print(SENSOR_LIST[i]->getVarName());





        for (int j = i+1; j < sensorCount; j++)

        {

            if (SENSOR_LIST[i]->getSensorName() == SENSOR_LIST[j]->getSensorName() &&

                SENSOR_LIST[i]->getSensorLocation() == SENSOR_LIST[j]->getSensorLocation())

            {



                Serial.print(F(" and "));

                Serial.print(SENSOR_LIST[i+1]->getVarName());

                i++;

            }

            else {break;}

        }

        Serial.println(F(" ---"));

        delay(250);

    }



    return success;

}



String generateSensorDataCSV(void)

{

    String csvString = String(currentTime) + F(", ");



    for (int i = 0; i < sensorCount; i++)

    {

        csvString += String(SENSOR_LIST[i]->getValue());

        if (i + 1 != sensorCount)

        {

            csvString += F(", ");

        }

    }



    return csvString;

}



String checkSensorLocations(void)

{

    String locationString = String(currentTime) + F(", ");



    for (int i = 0; i < sensorCount; i++)

    {

        locationString += String(SENSOR_LIST[i]->getSensorLocation());

        if (i + 1 != sensorCount)

        {

            locationString += F(", ");

        }

    }



    return locationString;

}





void logData(String rec)

{



  File logFile = SD.open(fileName, FILE_WRITE);





  logFile.println(rec);





  logFile.close();

}
# 499 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void setup()

{



    Serial.begin(SERIAL_BAUD);



    Serial1.begin(BEE_BAUD);





    rtc.begin();

    delay(100);





    pinMode(GREEN_LED, OUTPUT);

    pinMode(RED_LED, OUTPUT);



    greenred4flash();





    Serial.println(F("WebSDL Device: EnviroDIY Mayfly"));

    Serial.print(F("Now running "));

    Serial.println(SKETCH_NAME);

    Serial.print(F("Free RAM: "));

    Serial.println(freeRam());

    Serial.print(F("Current Mayfly RTC time is: "));

    Serial.println(getDateTime_ISO8601());





    sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);

    Serial.print(F("There are "));

    Serial.print(String(sensorCount));

    Serial.println(F(" variables being recorded"));





    int setupTries = 0;

    bool success = false;

    while (setupTries < 5)

    {

        if (setupSensors() == true) {

            success = true;

            break;

        }

        else {setupTries++;}

    }

    if (success != true)

    {

        Serial.println(F("Set up failed!"));



        digitalWrite(RED_LED, HIGH);

    }





    setupLogFile();

}
# 613 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void loop()

{



    Serial.println(F("------------------------------------------"));



    digitalWrite(GREEN_LED, HIGH);



    digitalWrite(switchedPower, HIGH);



    updateAllSensors();



    Serial.println(generateSensorDataCSV());



    logData(generateSensorDataCSV());



    digitalWrite(switchedPower, LOW);



    digitalWrite(GREEN_LED, LOW);



    Serial.println(F("------------------------------------------\n"));





    delay(10000);

}