# 1 "c:\\users\\sdamia~1.str\\appdata\\local\\temp\\tmpeaba52"
#include <Arduino.h>
# 1 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
# 39 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <Arduino.h>

#include <Sodaq_DS3231.h>

#include <SensorBase.h>
# 57 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
const char *SKETCH_NAME = "modular_sensors.ino";





const int TIME_ZONE = -5;



VariableArray sensors;
# 77 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <AOSongAM2315.h>



const int I2CPower = 22;

AOSongAM2315 am2315(I2CPower);
# 95 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <CampbellOBS3.h>



const int OBSLowPin = 0;

const float OBSLow_A = 4.0749E+00;

const float OBSLow_B = 9.1011E+01;

const float OBSLow_C = -3.9570E-01;

const int OBS3Power = 22;

CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C);



const int OBSHighPin = 1;

const float OBSHigh_A = 5.2996E+01;

const float OBSHigh_B = 3.7828E+02;

const float OBSHigh_C = -1.3927E+00;

CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C);
# 133 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <Decagon5TM.h>

const char *TMSDI12address = "2";

const int SDI12Data = 7;

const int SDI12Power = 22;

Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);
# 153 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <DecagonCTD.h>

const char *CTDSDI12address = "1";

const int numberReadings = 10;





DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, numberReadings);
# 175 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <DecagonES2.h>

const char *ES2SDI12address = "3";





DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data);
# 195 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <MaxBotixSonar.h>

const int SonarData = 10;

const int SonarTrigger = -1;

const int SonarPower = 22;

MaxBotixSonar sonar(SonarPower, SonarData, SonarTrigger) ;
# 215 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
#include <MayflyOnboardSensors.h>

const char *MFVersion = "v0.3";

EnviroDIYMayfly mayfly(MFVersion) ;
# 229 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
Variable *variableList[] = {

    new AOSongAM2315_Humidity(&am2315),

    new AOSongAM2315_Temp(&am2315),

    new CampbellOBS3_Turbidity(&osb3low),

    new CampbellOBS3_TurbHigh(&osb3high),

    new Decagon5TM_Ea(&fivetm),

    new Decagon5TM_Temp(&fivetm),

    new Decagon5TM_VWC(&fivetm),

    new DecagonCTD_Depth(&ctd),

    new DecagonCTD_Temp(&ctd),

    new DecagonCTD_Cond(&ctd),

    new DecagonES2_Cond(&es2),

    new DecagonES2_Temp(&es2),

    new MaxBotixSonar_Range(&sonar),

    new EnviroDIYMayfly_Temp(&mayfly),

    new EnviroDIYMayfly_Batt(&mayfly),

    new EnviroDIYMayfly_FreeRam(&mayfly)



};

int variableCount = sizeof(variableList) / sizeof(variableList[0]);
# 279 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
const int SERIAL_BAUD = 9600;

const int GREEN_LED = 8;

const int RED_LED = 9;
# 299 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void greenred4flash();
uint32_t getNow();
String getDateTime_ISO8601(void);
void setup();
void loop();
#line 299 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
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







long currentepochtime = 0;

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
# 421 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void setup()

{



    Serial.begin(SERIAL_BAUD);





    rtc.begin();

    delay(100);





    pinMode(GREEN_LED, OUTPUT);

    pinMode(RED_LED, OUTPUT);



    greenred4flash();





    Serial.print(F("Now running "));

    Serial.println(SKETCH_NAME);

    Serial.print(F("Current Mayfly RTC time is: "));

    Serial.println(getDateTime_ISO8601());

    Serial.print(F("There are "));

    Serial.print(String(variableCount));

    Serial.println(F(" variables being recorded"));





    sensors.init(variableCount, variableList);





    sensors.setupSensors();



    Serial.println(F("Setup finished!"));

    Serial.println(F("------------------------------------------\n"));

}
# 497 "C:/Users/sdamiano.STROUDWATER/Documents/GitHub/EnviroDIY/ModularSensors/examples/multisensor_print/multisensor_print.ino"
void loop()

{



    Serial.println(F("------------------------------------------"));



    digitalWrite(22, HIGH);



    delay(1000);



    digitalWrite(GREEN_LED, HIGH);



    sensors.updateAllSensors();



    digitalWrite(22, LOW);



    Serial.print(F("Updated all sensors at "));

    Serial.println(getDateTime_ISO8601());

    sensors.printSensorData(&Serial);



    digitalWrite(GREEN_LED, LOW);



    Serial.println(F("------------------------------------------\n"));





    delay(10000);

}