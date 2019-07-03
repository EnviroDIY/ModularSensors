/*
 * AtlasScientificCO2.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc CO2 is the temperature in degrees C.
 *     Accuracy is ± 3% or ± 30 ppm
 *     Range is 0 − 10000 ppm
 *     Resolution is 1 ppm
 */

// Header Guards
#ifndef AtlasScientificCO2_h
#define AtlasScientificCO2_h

// Debugging Statement
// #define MS_ATLASSCIENTIFICCO2_DEBUG

#ifdef MS_ATLASSCIENTIFICCO2_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificCO2"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_CO2_I2C_ADDR 0x69  // 105

// Sensor Specific Defines
#define ATLAS_CO2_NUM_VARIABLES 2
// TODO:  Test timing with sensor
#define ATLAS_CO2_WARM_UP_TIME_MS 850
// NOTE:  This has a long stabilization time!
#define ATLAS_CO2_STABILIZATION_TIME_MS 10000
#define ATLAS_CO2_MEASUREMENT_TIME_MS 900

#define ATLAS_CO2_RESOLUTION 1
#define ATLAS_CO2_VAR_NUM 0

#define ATLAS_CO2TEMP_RESOLUTION 0
#define ATLAS_CO2TEMP_VAR_NUM 1

// The main class for the Atlas Scientific CO2 temperature sensor
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class AtlasScientificCO2 : public AtlasParent<THEWIRE>
{
public:
    // Constructors
    AtlasScientificCO2(THEWIRE theI2C, int8_t powerPin,
                       uint8_t i2cAddressHex = ATLAS_CO2_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
      : AtlasParent<THEWIRE>(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificCO2", ATLAS_CO2_NUM_VARIABLES,
                    ATLAS_CO2_WARM_UP_TIME_MS, ATLAS_CO2_STABILIZATION_TIME_MS,
                    ATLAS_CO2_MEASUREMENT_TIME_MS)
    {}
    AtlasScientificCO2(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_CO2_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
      : AtlasParent<THEWIRE>(powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificCO2", ATLAS_CO2_NUM_VARIABLES,
                    ATLAS_CO2_WARM_UP_TIME_MS, ATLAS_CO2_STABILIZATION_TIME_MS,
                    ATLAS_CO2_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~AtlasScientificCO2(){}

    // We must be very explicity about which functions/internal variables we're using
    using AtlasParent<THEWIRE>::getSensorLocation;
    using Sensor::getSensorName;
    using Sensor::getSensorNameAndLocation;
    using Sensor::getPowerPin;
    using Sensor::setNumberMeasurementsToAverage;
    using Sensor::getNumberMeasurementsToAverage;
    using Sensor::getStatus;
    using Sensor::update;
    using Sensor::powerUp;
    using Sensor::powerDown;
    using Sensor::wake;
    using AtlasParent<THEWIRE>::sleep;
    using AtlasParent<THEWIRE>::startSingleMeasurement;
    using AtlasParent<THEWIRE>::addSingleMeasurementResult;
    using Sensor::clearValues;
    using Sensor::verifyAndAddMeasurementResult;
    using Sensor::averageMeasurements;
    using Sensor::registerVariable;
    using Sensor::notifyVariables;
    using Sensor::checkPowerOn;
    using Sensor::isWarmedUp;
    using Sensor::waitForWarmUp;
    using Sensor::isStable;
    using Sensor::waitForStability;
    using Sensor::isMeasurementComplete;
    using Sensor::waitForMeasurementCompletion;
    using AtlasParent<THEWIRE>::waitForProcessing;

    using AtlasParent<THEWIRE>::_i2c;
    using AtlasParent<THEWIRE>::_i2cAddressHex;

    using Sensor::_sensorStatus;
    using Sensor::sensorValues;
    using Sensor::_dataPin;
    using Sensor::_powerPin;
    using Sensor::_sensorName;
    using Sensor::_numReturnedVars;
    using Sensor::_measurementsToAverage;
    using Sensor::numberGoodMeasurementsMade;
    using Sensor::_warmUpTime_ms;
    using Sensor::_millisPowerOn;
    using Sensor::_stabilizationTime_ms;
    using Sensor::_millisSensorActivated;
    using Sensor::variables;


    // Setup
    virtual bool setup(void) override
    {
        bool success = Sensor::setup();  // this will set pin modes and the setup status bit

        // This sensor needs power for setup!
        // We want to turn on all possible measurement parameters
        bool wasOn = checkPowerOn();
        if (!wasOn) {powerUp();}
        waitForWarmUp();

        MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report temperature with CO2"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,t,1", 6);  // Enable temperature
        success &= !_i2c.endTransmission();
        // NOTE: The return of 0 from endTransmission indicates success
        success &= waitForProcessing();

        if (!success)
        {
            // Set the status error bit (bit 7)
            _sensorStatus |= 0b10000000;
            // UN-set the set-up bit (bit 0) since setup failed!
            _sensorStatus &= 0b11111110;
        }

        // Turn the power back off it it had been turned on
        if (!wasOn) {powerDown();}

        return success;
    }
};

// The class for the CO2 Concentration Variable
class AtlasScientificCO2_CO2 : public Variable
{
public:
    AtlasScientificCO2_CO2(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "AtlasCO2ppm")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_CO2_VAR_NUM,
                 (uint8_t)ATLAS_CO2_RESOLUTION,
                 "carbonDioxide", "partPerMillion",
                 varCode, uuid)
    {}
    AtlasScientificCO2_CO2()
      : Variable((const uint8_t)ATLAS_CO2_VAR_NUM,
                 (uint8_t)ATLAS_CO2_RESOLUTION,
                 "carbonDioxide", "partPerMillion", "AtlasCO2ppm")
    {}
    ~AtlasScientificCO2_CO2(){}
};

// The class for the Temp Variable
class AtlasScientificCO2_Temp : public Variable
{
public:
    AtlasScientificCO2_Temp(Sensor *parentSense,
                            const char *uuid = "",
                            const char *varCode = "AtlasCO2Temp")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_CO2TEMP_VAR_NUM,
                 (uint8_t)ATLAS_CO2TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    AtlasScientificCO2_Temp()
      : Variable((const uint8_t)ATLAS_CO2TEMP_VAR_NUM,
                 (uint8_t)ATLAS_CO2TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "AtlasCO2Temp")
    {}
    ~AtlasScientificCO2_Temp(){}
};

#endif  // Header Guard
