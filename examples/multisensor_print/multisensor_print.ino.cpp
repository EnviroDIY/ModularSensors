# 1 "c:\\users\\sdamia~1.str\\appdata\\local\\temp\\tmpqmyisk"
#include <Arduino.h>
# 1 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
# 39 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <Arduino.h>

#include <Sodaq_DS3231.h>



#include <SensorBase.h>



#include <DecagonCTD.h>

#include <Decagon5TM.h>

#include <DecagonES2.h>

#include <CampbellOBS3.h>

#include <MaxBotixSonar.h>

#include <MaximDS18.h>

#include <AOSongAM2315.h>

#include <BoschBME280.h>

#include <MayflyOnboardSensors.h>
# 79 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
const char *SKETCH_NAME = "modular_sensors.ino";





const char *MFVersion = "v0.3";





const int TIME_ZONE = -5;





const char *CTDSDI12address = "1";

const int numberReadings = 10;

const int SDI12Data = 7;

const int switchedPower = 22;







const char *TMSDI12address = "2";
# 123 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
const char *ES2SDI12address = "3";
# 133 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
const int SonarData = 10;



const int SonarTrigger = -1;







const int OBSLowPin = 0;

const float OBSLow_A = 4.0749E+00;

const float OBSLow_B = 9.1011E+01;

const float OBSLow_C = -3.9570E-01;



const int OBSHighPin = 1;

const float OBSHigh_A = 5.2996E+01;

const float OBSHigh_B = 3.7828E+02;

const float OBSHigh_C = -1.3927E+00;







DeviceAddress DS18Address{0x28, 0xFF, 0xB6, 0x6E, 0x84, 0x16, 0x05, 0x9B};

const int DS18data = 5;
# 183 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
SensorBase *SENSOR_LIST[] = {

    new DecagonCTD_Depth(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),

    new DecagonCTD_Temp(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),

    new DecagonCTD_Cond(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),

    new Decagon5TM_Ea(*TMSDI12address, switchedPower, SDI12Data),

    new Decagon5TM_Temp(*TMSDI12address, switchedPower, SDI12Data),

    new Decagon5TM_VWC(*TMSDI12address, switchedPower, SDI12Data),

    new DecagonES2_Cond(*ES2SDI12address, switchedPower, SDI12Data),

    new DecagonES2_Temp(*ES2SDI12address, switchedPower, SDI12Data),

    new MaxBotixSonar_Range(switchedPower, SonarData, SonarTrigger),

    new CampbellOBS3_Turbidity(switchedPower, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C),

    new CampbellOBS3_TurbHigh(switchedPower, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C),

    new MaximDS18_Temp(DS18Address, switchedPower, DS18data),

    new AOSongAM2315_Temp(switchedPower),

    new AOSongAM2315_Humidity(switchedPower),

    new BoschBME280_Temp(switchedPower),

    new BoschBME280_Humidity(switchedPower),

    new BoschBME280_Pressure(switchedPower),

    new BoschBME280_Altitude(switchedPower),

    new MayflyOnboardTemp(MFVersion),

    new MayflyOnboardBatt(MFVersion),

    new MayflyFreeRam()



};

int sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);
# 243 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
const int SERIAL_BAUD = 9600;

const int GREEN_LED = 8;

const int RED_LED = 9;





long currentepochtime = 0;
# 269 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void greenred4flash();
uint32_t getNow();
String getDateTime_ISO8601(void);
void setup();
void loop();
#line 269 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
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





SensorArray sensors;
# 395 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void setup()

{



    Serial.begin(SERIAL_BAUD);





    rtc.begin();

    delay(100);





    pinMode(GREEN_LED, OUTPUT);

    pinMode(RED_LED, OUTPUT);



    greenred4flash();





    Serial.println(F("WebSDL Device: EnviroDIY Mayfly"));

    Serial.print(F("Now running "));

    Serial.println(SKETCH_NAME);

    Serial.print(F("Current Mayfly RTC time is: "));

    Serial.println(getDateTime_ISO8601());





    Serial.print(F("There are "));

    Serial.print(String(sensorCount));

    Serial.println(F(" variables being recorded"));





    sensors.init(sensorCount, SENSOR_LIST);





    sensors.setupSensors();



    Serial.println(F("Setup finished!"));

    Serial.println(F("------------------------------------------\n"));

}
# 477 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void loop()

{



    Serial.println(F("------------------------------------------"));



    digitalWrite(switchedPower, HIGH);



    delay(1000);



    digitalWrite(GREEN_LED, HIGH);



    sensors.updateAllSensors();



    Serial.print(F("Updated all sensors at "));

    Serial.println(getDateTime_ISO8601());

    sensors.printSensorData(&Serial);



    digitalWrite(GREEN_LED, LOW);



    digitalWrite(switchedPower, LOW);



    Serial.println(F("------------------------------------------\n"));





    delay(10000);

}