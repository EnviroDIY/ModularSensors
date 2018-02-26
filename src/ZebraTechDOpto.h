/*
 *ZebraTechDOpto.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the ZebraTech D-Opto digital dissolved oxygen sensor
 *This sensor communicates via SDI-12
 *
 *The manual for this sensor is available at:
 * www.zebra-tech.co.nz/wp-content/uploads/2014/04/D-Opto-Sensor-manual-A4-ver-2.pdf
 *
 * For Dissolved Oxygen:
 *     Accuracy is 1% of reading or 0.02PPM, whichever is greater
 *     Resolution is 0.01% / 0.001 PPM
 *
 * For Temperature:
 *     Accuracy is ± 0.1°C
 *     Resolution is 0.01°C
 *
 * Maximum warm-up time in SDI-12 mode: 200ms, assume stability at warm-up
 * Maximum measurement duration: 200ms
 *
*/

#ifndef ZebraTechDOpto_h
#define ZebraTechDOpto_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"
#include <SDI12_ExtInts.h>

#define DOPTO_NUM_VARIABLES 3
#define DOPTO_WARM_UP_TIME_MS 200
#define DOPTO_STABILIZATION_TIME_MS 5000
#define DOPTO_MEASUREMENT_TIME_MS 200

#define DOPTO_TEMP_RESOLUTION 2
#define DOPTO_TEMP_VAR_NUM 0

#define DOPTO_DOPCT_RESOLUTION 2
#define DOPTO_DOPCT_VAR_NUM 1

#define DOPTO_DOMGL_RESOLUTION 3
#define DOPTO_DOMGL_VAR_NUM 2

// The main class for the Decagon CTD
class ZebraTechDOpto : public Sensor
{
public:

    ZebraTechDOpto(char SDI12address, int powerPin, int dataPin, int measurementsToAverage = 1);
    ZebraTechDOpto(char *SDI12address, int powerPin, int dataPin, int measurementsToAverage = 1);
    ZebraTechDOpto(int SDI12address, int powerPin, int dataPin, int measurementsToAverage = 1);

    String getSensorVendor(void);
    String getSensorModel(void);
    String getSensorVersion(void);
    String getSensorSerialNumber(void);
    String getSensorLocation(void) override;

    virtual SENSOR_STATUS setup(void) override;
    virtual SENSOR_STATUS getStatus(void) override;

    virtual bool startSingleMeasurement(void);
    virtual bool addSingleMeasurementResult(void);

protected:
    bool getSensorInfo(void);
    SDI12 _SDI12Internal;

private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
    char _SDI12address;
};


// Defines the Temperature Variable
class ZebraTechDOptoTemp : public Variable
{
public:
    ZebraTechDOptoTemp(Sensor *parentSense,
                        String UUID = "", String customVarCode = "")
     : Variable(parentSense, DOPTO_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                DOPTO_TEMP_RESOLUTION,
                F("DOtempC"), UUID, customVarCode)
    {}
};


// Defines the Dissolved Oxygen Percent Saturation
class ZebraTechDOptoDOpct : public Variable
{
public:
    ZebraTechDOptoDOpct(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
     : Variable(parentSense, DOPTO_DOPCT_VAR_NUM,
                F("oxygenDissolvedPercentOfSaturation"), F("percent"),
                DOPTO_DOPCT_RESOLUTION,
                F("DOpercent"), UUID, customVarCode)
    {}
};


// Defines the Dissolved Oxygen Concentration
class ZebraTechDOptoDOmgL : public Variable
{
public:
    ZebraTechDOptoDOmgL(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
     : Variable(parentSense, DOPTO_DOMGL_VAR_NUM,
                F("oxygenDissolved"), F("milligramPerLiter"),
                DOPTO_DOMGL_RESOLUTION,
                F("DOppm"), UUID, customVarCode)
    {}
};

#endif
