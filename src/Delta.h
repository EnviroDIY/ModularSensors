/*
 *Delta.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Bobby Schulz <schu3119@umn.edu>.
 *
 *This is software defined sensor which simply takes the differential between two other physical sensors and returns a variable of Var1 - Var2
*/

#ifndef Delta_h
#define Delta_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"


#include "SensorBase.h"
#include "VariableBase.h"

#define DELTA_NUM_VARIABLES 2
#define DELTA_WARM_UP_TIME_MS 6
#define DELTA_STABILIZATION_TIME_MS 0
#define DELTA_MEASUREMENT_TIME_MS 4

#define DELTA_DIFF_RESOLUTION 2
#define DELTA_DIFF_VAR_NUM 0

// String VariableUnits = "";

// The main class for the Measurement Specialties MS5803
class Delta : public Sensor
{
public:
    Delta(Variable *Var1, Variable *Var2, String Units, float ScaleVar1 = 1, float ScaleVar2 = 1);

    bool setup(void) override;
    String getSensorLocation(void) override;
    bool addSingleMeasurementResult(void) override;
protected:
    Variable *_Var1;
    Variable *_Var2;
    float _ScaleVar1;
    float _ScaleVar2;
    // String _Units;
};


// Defines the Differential Variable
class Delta_Diff : public Variable
{
public:
    Delta_Diff(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, DELTA_DIFF_VAR_NUM,
               F("differential"), F(" units"),  //Fix to make units variable! 
               DELTA_DIFF_RESOLUTION,
               F("Delta_Diff"), UUID, customVarCode)
    {}
};


#endif
