# 1 "c:\\users\\sdamia~1.str\\appdata\\local\\temp\\tmpu52nju"
#include <Arduino.h>
# 1 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
# 43 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#define TINY_GSM_MODEM_ESP8266 
# 55 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <Arduino.h>

#include <EnableInterrupt.h>

#include <LoggerEnviroDIY.h>
# 73 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
const char *sketchName = "data_saving.ino";





const char *LoggerID = "XXXXX";



const uint8_t loggingInterval = 5;



const int8_t timeZone = -5;







LoggerEnviroDIY loggerComplete;

LoggerEnviroDIY loggerToGo;
# 109 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <ProcessorStats.h>



const long serialBaud = 115200;

const int8_t greenLED = 8;

const int8_t redLED = 9;

const int8_t buttonPin = 21;

const int8_t wakePin = A7;





const int8_t sdCardPin = 12;





const char *MFVersion = "v0.5";

ProcessorStats mayfly(MFVersion) ;



Variable *mayflyBatt = new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");

Variable *mayflyRAM = new ProcessorStats_FreeRam(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");
# 153 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
HardwareSerial &ModemSerial = Serial1;



#if defined(TINY_GSM_MODEM_XBEE)

const int8_t modemSleepRqPin = 23;

const int8_t modemStatusPin = 19;

const int8_t modemVCCPin = -1;

ModemSleepType ModemSleepMode = modem_sleep_reverse;



#elif defined(TINY_GSM_MODEM_ESP8266)

const int8_t modemSleepRqPin = 19;

const int8_t modemStatusPin = -1;

const int8_t modemVCCPin = -1;

ModemSleepType ModemSleepMode = modem_always_on;



#else

const int8_t modemSleepRqPin = 23;

const int8_t modemStatusPin = 19;

const int8_t modemVCCPin = -1;

ModemSleepType ModemSleepMode = modem_sleep_held;

#endif
# 201 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#if defined(TINY_GSM_MODEM_ESP8266)

const long ModemBaud = 57600;

#elif defined(TINY_GSM_MODEM_SIM800)

const long ModemBaud = 9600;

#elif defined(TINY_GSM_MODEM_XBEE)

const long ModemBaud = 9600;

#else

const long ModemBaud = 9600;

#endif



const char *apn = "xxxxx";





const char *wifiId = "Stroud-Mobile";

const char *wifiPwd = "phone970";







loggerModem modem;





Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab");

Variable *modemSinalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab");
# 257 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <MaximDS3231.h>



MaximDS3231 ds3231(1);



Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab");







#include <AltSoftSerial.h>

AltSoftSerial modbusSerial;
# 287 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <YosemitechY504.h>

byte y504modbusAddress = 0x04;

const int8_t modbusPower = 22;

const int8_t max485EnablePin = -1;

const uint8_t y504NumberReadings = 5;

YosemitechY504 y504(y504modbusAddress, modbusSerial, modbusPower, max485EnablePin, y504NumberReadings);







Variable *y504DOpct = new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-efgh-1234567890ab");

Variable *y504DOmgL = new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-efgh-1234567890ab");

Variable *y504Temp = new YosemitechY504_Temp(&y504, "12345678-abcd-1234-efgh-1234567890ab");
# 321 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <YosemitechY511.h>

byte y511modbusAddress = 0x1A;





const uint8_t y511NumberReadings = 5;



YosemitechY511 y511(y511modbusAddress, modbusSerial, modbusPower, max485EnablePin, y511NumberReadings);



Variable *y511Turb = new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-efgh-1234567890ab");

Variable *y511Temp = new YosemitechY511_Temp(&y511, "12345678-abcd-1234-efgh-1234567890ab");
# 351 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <YosemitechY514.h>

byte y514modbusAddress = 0x14;





const uint8_t y514NumberReadings = 5;



YosemitechY514 y514(y514modbusAddress, modbusSerial, modbusPower, max485EnablePin, y514NumberReadings);



Variable *y514Chloro = new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-efgh-1234567890ab");

Variable *y514Temp = new YosemitechY514_Temp(&y514, "12345678-abcd-1234-efgh-1234567890ab");
# 381 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
#include <YosemitechY520.h>

byte y520modbusAddress = 0x20;





const uint8_t y520NumberReadings = 5;



YosemitechY520 y520(y520modbusAddress, modbusSerial, modbusPower, max485EnablePin, y520NumberReadings);



Variable *y520Cond = new YosemitechY520_Cond(&y520, "12345678-abcd-1234-efgh-1234567890ab");

Variable *y520Temp = new YosemitechY520_Temp(&y520, "12345678-abcd-1234-efgh-1234567890ab");
# 411 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
Variable *variableList_complete[] = {

    mayflyBatt,

    mayflyRAM,

    ds3231Temp,

    y504DOpct,

    y504DOmgL,

    y504Temp,

    y511Turb,

    y511Temp,

    y514Chloro,

    y514Temp,

    y520Cond,

    y520Temp,

    modemRSSI,

    modemSinalPct

};

int variableCount_complete = sizeof(variableList_complete) / sizeof(variableList_complete[0]);
# 455 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
Variable *variableList_toGo[] = {

    y504DOmgL,

    y504Temp,

    y511Turb,

    y514Chloro,

    y520Cond

};

int variableCount_toGo = sizeof(variableList_toGo) / sizeof(variableList_toGo[0]);
# 483 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";

const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";
# 501 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
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
# 535 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
void setup();
void loop();
#line 535 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
void setup()

{



    Serial.begin(serialBaud);





    ModemSerial.begin(ModemBaud);





    modbusSerial.begin(9600);





    pinMode(greenLED, OUTPUT);

    pinMode(redLED, OUTPUT);



    greenredflash();





    Serial.print(F("Now running "));

    Serial.print(sketchName);

    Serial.print(F(" on Logger "));

    Serial.println(LoggerID);







    Logger::setTimeZone(timeZone);



    Logger::setTZOffset(timeZone);





    loggerComplete.init(sdCardPin, wakePin, variableCount_complete, variableList_complete,

                loggingInterval, LoggerID);

    loggerToGo.init(sdCardPin, wakePin, variableCount_toGo, variableList_toGo,

                loggingInterval, LoggerID);
# 611 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
    #if defined(TINY_GSM_MODEM_ESP8266)

        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);

    #elif defined(TINY_GSM_MODEM_XBEE)

        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);



    #else

        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, apn);

    #endif
# 635 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
    loggerComplete.attachModem(&modem);

    loggerToGo.attachModem(&modem);







    loggerComplete.setToken(registrationToken);

    loggerComplete.setSamplingFeatureUUID(samplingFeature);

    loggerToGo.setToken(registrationToken);

    loggerToGo.setSamplingFeatureUUID(samplingFeature);
# 663 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
    loggerComplete.begin();
# 675 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
    pinMode(buttonPin, INPUT_PULLUP);

    enableInterrupt(buttonPin, Logger::testingISR, CHANGE);

    Serial.print(F("Push button on pin "));

    Serial.print(buttonPin);

    Serial.println(F(" at any time to enter sensor testing mode."));





    greenredflash(6, 25);

}
# 709 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
void loop()

{





    if (loggerComplete.checkInterval())

    {



        Serial.print(F("------------------------------------------\n"));



        digitalWrite(greenLED, HIGH);





        modem.powerUp();

        modem.wake();





        Serial.print(F("Powering sensors...\n"));

        loggerComplete.sensorsPowerUp();



        Serial.print(F("Waking sensors...\n"));

        loggerComplete.sensorsWake();



        Serial.print(F("Updating sensor values...\n"));

        loggerComplete.updateAllSensors();



        Serial.print(F("Putting sensors back to sleep...\n"));

        loggerComplete.sensorsSleep();



        Serial.print(F("Cutting sensor power...\n"));

        loggerComplete.sensorsPowerDown();





        loggerComplete.logToSD(loggerComplete.generateSensorDataCSV());





        Serial.print(F("Connecting to the internet...\n"));

        if (modem.connectInternet())

        {



            loggerToGo.postDataEnviroDIY();





            modem.disconnectInternet();

        }



        modem.off();





        digitalWrite(greenLED, LOW);



        Serial.print(F("------------------------------------------\n\n"));

    }
# 821 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/data_saving/data_saving.ino"
    if (Logger::startTesting) loggerComplete.testingMode();





    if (Logger::markedEpochTime % 86400 == 0)

    {



        modem.powerUp();

        modem.wake();



        if (modem.connectInternet())

        {



            loggerComplete.syncRTClock(modem.getNISTTime());



            modem.disconnectInternet();

        }



        modem.off();

    }







    loggerComplete.systemSleep();

}