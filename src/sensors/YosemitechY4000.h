/*
 *YosemitechY4000.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Audfdenkampe
 <aaufdenkampe@limno.com>, based on other YosemiTech libs by Sara Damiano
 (sdamiano@stroudcenter.org).
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

// Header Guards
#ifndef YosemitechY4000_h
#define YosemitechY4000_h

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y4000_NUM_VARIABLES 8
#define Y4000_WARM_UP_TIME_MS 1600
// Time for communication to begin
#define Y4000_STABILIZATION_TIME_MS 60000
// Y4000 Modbus manual says 60s; we find Cond takes about that long to respond.
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

// The main class for the Yosemitech Y4000
class YosemitechY4000 : public YosemitechParent {
public:
  // Constructors with overloads
  YosemitechY4000(byte modbusAddress, Stream *stream, int8_t powerPin,
                  int8_t powerPin2 = -1, int8_t enablePin = -1,
                  uint8_t measurementsToAverage = 1)
      : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin,
                         measurementsToAverage, Y4000, "YosemitechY4000",
                         Y4000_NUM_VARIABLES, Y4000_WARM_UP_TIME_MS,
                         Y4000_STABILIZATION_TIME_MS,
                         Y4000_MEASUREMENT_TIME_MS) {}
  YosemitechY4000(byte modbusAddress, Stream &stream, int8_t powerPin,
                  int8_t powerPin2 = -1, int8_t enablePin = -1,
                  uint8_t measurementsToAverage = 1)
      : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin,
                         measurementsToAverage, Y4000, "YosemitechY4000",
                         Y4000_NUM_VARIABLES, Y4000_WARM_UP_TIME_MS,
                         Y4000_STABILIZATION_TIME_MS,
                         Y4000_MEASUREMENT_TIME_MS) {}
  ~YosemitechY4000() {}
};

// Defines the Dissolved Oxygen Concentration
class YosemitechY4000_DOmgL : public Variable {
public:
  YosemitechY4000_DOmgL(Sensor *parentSense, const char *uuid = "",
                        const char *varCode = "Y4000DOmgL")
      : Variable(parentSense, (const uint8_t)Y4000_DOMGL_VAR_NUM,
                 (uint8_t)Y4000_DOMGL_RESOLUTION, "oxygenDissolved",
                 "milligramPerLiter", varCode, uuid) {}
  YosemitechY4000_DOmgL()
      : Variable((const uint8_t)Y4000_DOMGL_VAR_NUM,
                 (uint8_t)Y4000_DOMGL_RESOLUTION, "oxygenDissolved",
                 "milligramPerLiter", "Y4000DOmgL") {}
  ~YosemitechY4000_DOmgL() {}
};

// Defines the Turbidity
class YosemitechY4000_Turbidity : public Variable {
public:
  YosemitechY4000_Turbidity(Sensor *parentSense, const char *uuid = "",
                            const char *varCode = "Y4000Turbidity")
      : Variable(parentSense, (const uint8_t)Y4000_TURB_VAR_NUM,
                 (uint8_t)Y4000_TURB_RESOLUTION, "turbidity",
                 "nephelometricTurbidityUnit", varCode, uuid) {}
  YosemitechY4000_Turbidity()
      : Variable((const uint8_t)Y4000_TURB_VAR_NUM,
                 (uint8_t)Y4000_TURB_RESOLUTION, "turbidity",
                 "nephelometricTurbidityUnit", "Y4000Turbidity") {}
  ~YosemitechY4000_Turbidity() {}
};

// Defines the Conductivity
class YosemitechY4000_Cond : public Variable {
public:
  YosemitechY4000_Cond(Sensor *parentSense, const char *uuid = "",
                       const char *varCode = "Y4000Cond")
      : Variable(parentSense, (const uint8_t)Y4000_COND_VAR_NUM,
                 (uint8_t)Y4000_COND_RESOLUTION, "specificConductance",
                 "microsiemenPerCentimeter", varCode, uuid) {}
  YosemitechY4000_Cond()
      : Variable((const uint8_t)Y4000_COND_VAR_NUM,
                 (uint8_t)Y4000_COND_RESOLUTION, "specificConductance",
                 "microsiemenPerCentimeter", "Y4000Cond") {}
  ~YosemitechY4000_Cond() {}
};

// Defines the pH
class YosemitechY4000_pH : public Variable {
public:
  YosemitechY4000_pH(Sensor *parentSense, const char *uuid = "",
                     const char *varCode = "Y4000pH")
      : Variable(parentSense, (const uint8_t)Y4000_PH_VAR_NUM,
                 (uint8_t)Y4000_PH_RESOLUTION, "pH", "pH", varCode, uuid) {}
  YosemitechY4000_pH()
      : Variable((const uint8_t)Y4000_PH_VAR_NUM, (uint8_t)Y4000_PH_RESOLUTION,
                 "pH", "pH", "Y4000pH") {}
  ~YosemitechY4000_pH() {}
};

// Defines the Temperature Variable
class YosemitechY4000_Temp : public Variable {
public:
  YosemitechY4000_Temp(Sensor *parentSense, const char *uuid = "",
                       const char *varCode = "Y4000Temp")
      : Variable(parentSense, (const uint8_t)Y4000_TEMP_VAR_NUM,
                 (uint8_t)Y4000_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                 varCode, uuid) {}
  YosemitechY4000_Temp()
      : Variable((const uint8_t)Y4000_TEMP_VAR_NUM,
                 (uint8_t)Y4000_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                 "Y4000Temp") {}
  ~YosemitechY4000_Temp() {}
};

// Defines the Electrode Electrical Potential
class YosemitechY4000_ORP : public Variable {
public:
  YosemitechY4000_ORP(Sensor *parentSense, const char *uuid = "",
                      const char *varCode = "Y4000Potential")
      : Variable(parentSense, (const uint8_t)Y4000_ORP_VAR_NUM,
                 (uint8_t)Y4000_ORP_RESOLUTION, "ORP", "millivolt", varCode,
                 uuid) {}
  YosemitechY4000_ORP()
      : Variable((const uint8_t)Y4000_ORP_VAR_NUM,
                 (uint8_t)Y4000_ORP_RESOLUTION, "ORP", "millivolt",
                 "Y4000Potential") {}
  ~YosemitechY4000_ORP() {}
};

// Defines the Chlorophyll Concentration
class YosemitechY4000_Chlorophyll : public Variable {
public:
  YosemitechY4000_Chlorophyll(Sensor *parentSense, const char *uuid = "",
                              const char *varCode = "Y4000Chloro")
      : Variable(parentSense, (const uint8_t)Y4000_CHLORO_VAR_NUM,
                 (uint8_t)Y4000_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                 "microgramPerLiter", varCode, uuid) {}
  YosemitechY4000_Chlorophyll()
      : Variable((const uint8_t)Y4000_CHLORO_VAR_NUM,
                 (uint8_t)Y4000_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                 "microgramPerLiter", "Y4000Chloro") {}
  ~YosemitechY4000_Chlorophyll() {}
};

// Defines the Blue Green Algae (BGA) Concentration
class YosemitechY4000_BGA : public Variable {
public:
  YosemitechY4000_BGA(Sensor *parentSense, const char *uuid = "",
                      const char *varCode = "Y4000BGA")
      : Variable(parentSense, (const uint8_t)Y4000_BGA_VAR_NUM,
                 (uint8_t)Y4000_BGA_RESOLUTION,
                 "blueGreenAlgaeCyanobacteriaPhycocyanin", "microgramPerLiter",
                 varCode, uuid) {}
  YosemitechY4000_BGA()
      : Variable((const uint8_t)Y4000_BGA_VAR_NUM,
                 (uint8_t)Y4000_BGA_RESOLUTION,
                 "blueGreenAlgaeCyanobacteriaPhycocyanin", "microgramPerLiter",
                 "Y4000BGA") {}
  ~YosemitechY4000_BGA() {}
};

#endif // Header Guard
