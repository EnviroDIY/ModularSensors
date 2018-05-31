/*
 *YosemitechY4000.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Audfdenkampe <aaufdenkampe@limno.com>,
 based on other YosemiTech libs by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y4000 MultiParameter Sonde
 *It is dependent on the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor.
 *For details, see documentaition for individual sensor probes
 *
 * For Dissolved Oxygen:
 *     Resolution is 0.01 mg/L
 *     Accuracy is ± 0.3 mg/L
 *     Range is 0-20mg/L or 0-200% Air Saturation
 *
 * For Turbidity:
 *     Accuracy is ＜5% or 0.3NTU
 *     Range is 0.1~1000 NTU
 *
 * For Conductivity:
 *     Resolution is 0.1 µS/cm
 *     Accuracy is ± 1 % Full Scale
 *     Range is 1 µS/cm to 200 mS/cm
 *
 * For pH:
 *     Resolution is 0.01 pH units
 *     Accuracy is ± 0.1 pH units
 *     Range is 2 to 12 pH units
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * For ORP:
 *     Resolution is 1 mV
 *     Accuracy is ± 20 mV
 *     Range is -999 ~ 999 mV
 *
 * For Chlorophyll:
 *     Resolution is 0.1 µg/L / 0.1 RFU
 *     Accuracy is ± 1 %
 *     Range is 0 to 400 µg/L or 0 to 100 RFU
 *
 * For Blue-Green Algae:
 *     Resolution is 0.01 µg/L / 0.01 RFU
 *     Accuracy is ±  0.04ug/L PC
 *     Range is 0 to 100 µg/L or 0 to 100 RFU
 *
 * Time before sensor responds after power - 275-300ms (use 350 for safety)
 * Time between "StartMeasurement" command and stable reading depends on the
 * indindividual sensor probes, with Y520 Conductivity taking the longest
*/

#ifndef YosemitechY4000_h
#define YosemitechY4000_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y4000_NUM_VARIABLES 8
#define Y4000_WARM_UP_TIME_MS 1600         // Time for communication to begin
#define Y4000_STABILIZATION_TIME_MS 60000L  // Y4000 Modbus manual says 60s; we find Cond takes about that long to respond.
#define Y4000_MEASUREMENT_TIME_MS 3000

#define Y4000_DOMGL_RESOLUTION 2
#define Y4000_DOMGL_VAR_NUM 0

#define Y4000_TURB_RESOLUTION 2
#define Y4000_TURB_VAR_NUM 1

#define Y4000_COND_RESOLUTION 1
#define Y4000_COND_VAR_NUM 2

#define Y4000_PH_RESOLUTION 2
#define Y4000_PH_VAR_NUM 3

#define Y4000_TEMP_RESOLUTION 1
#define Y4000_TEMP_VAR_NUM 4

#define Y4000_ORP_RESOLUTION 0
#define Y4000_ORP_VAR_NUM 5

#define Y4000_CHLORO_RESOLUTION 1
#define Y4000_CHLORO_VAR_NUM 6

#define Y4000_BGA_RESOLUTION 2
#define Y4000_BGA_VAR_NUM 7




// The main class for the Decagon Y4000
class YosemitechY4000 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY4000(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, measurementsToAverage,
                        Y4000, F("YosemitechY4000"), Y4000_NUM_VARIABLES,
                        Y4000_WARM_UP_TIME_MS, Y4000_STABILIZATION_TIME_MS, Y4000_MEASUREMENT_TIME_MS)
    {}
    YosemitechY4000(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, measurementsToAverage,
                        Y4000, F("YosemitechY4000"), Y4000_NUM_VARIABLES,
                        Y4000_WARM_UP_TIME_MS, Y4000_STABILIZATION_TIME_MS, Y4000_MEASUREMENT_TIME_MS)
    {}
};


// Defines the Dissolved Oxygen Concentration
class YosemitechY4000_DOmgL : public Variable
{
public:
    YosemitechY4000_DOmgL(Sensor *parentSense,
                         String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_DOMGL_VAR_NUM,
                F("oxygenDissolved"), F("milligramPerLiter"),
                Y4000_DOMGL_RESOLUTION,
                F("Y4000DOmgL"), UUID, customVarCode)
    {}
};

// Defines the Turbidity
class YosemitechY4000_Turbidity : public Variable
{
public:
    YosemitechY4000_Turbidity(Sensor *parentSense, String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_TURB_VAR_NUM,
                F("turbidity"), F("nephelometricTurbidityUnit"),
                Y4000_TURB_RESOLUTION,
                F("Y4000Turbidity"), UUID, customVarCode)
    {}
};

// Defines the Conductivity
class YosemitechY4000_Cond : public Variable
{
public:
    YosemitechY4000_Cond(Sensor *parentSense, String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_COND_VAR_NUM,
                F("specificConductance"), F("microsiemenPerCentimeter"),
                Y4000_COND_RESOLUTION,
                F("Y4000Cond"), UUID, customVarCode)
    {}
};

// Defines the pH
class YosemitechY4000_pH : public Variable
{
public:
    YosemitechY4000_pH(Sensor *parentSense,
                      String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_PH_VAR_NUM,
                F("pH"), F("pH"),
                Y4000_PH_RESOLUTION,
                F("Y4000pH"), UUID, customVarCode)
    {}
};

// Defines the Temperature Variable
class YosemitechY4000_Temp : public Variable
{
public:
    YosemitechY4000_Temp(Sensor *parentSense,
                        String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y4000_TEMP_RESOLUTION,
                F("Y4000temp"), UUID, customVarCode)
    {}
};

// Defines the Electrode Electrical Potential
class YosemitechY4000_ORP : public Variable
{
public:
    YosemitechY4000_ORP(Sensor *parentSense,
                           String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_ORP_VAR_NUM,
                F("ORP"), F("millivolt"),
                Y4000_ORP_RESOLUTION,
                F("Y4000Potential"), UUID, customVarCode)
    {}
};

// Defines the Chlorophyll Concentration
class YosemitechY4000_Chlorophyll : public Variable
{
public:
    YosemitechY4000_Chlorophyll(Sensor *parentSense,
                               String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_CHLORO_VAR_NUM,
                F("chlorophyllFluorescence"), F("microgramPerLiter"),
                Y4000_CHLORO_RESOLUTION,
                F("Y4000Chloro"), UUID, customVarCode)
    {}
};

// Defines the Blue Green Algae (BGA) Concentration
class YosemitechY4000_BGA : public Variable
{
public:
    YosemitechY4000_BGA(Sensor *parentSense,
                               String& UUID = VAR_BASE_EMPTY, String& customVarCode = VAR_BASE_EMPTY)
     : Variable(parentSense, Y4000_BGA_VAR_NUM,
                F("blueGreenAlgaeCyanobacteriaPhycocyanin"), F("microgramPerLiter"),
                Y4000_BGA_RESOLUTION,
                F("Y4000BGA"), UUID, customVarCode)
    {}
};



#endif
