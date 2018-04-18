/*
 *Delta.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Bobby Schulz <schu3119@umn.edu>.
 *
 *This is software defined sensor which simply takes the differential between two other physical sensors and returns a variable of Var1 - Var2
*/

#include "Delta.h"


// The constructor - because this is I2C, only need the power pin
Delta::Delta(Variable *Var1, Variable *Var2, String Units, float ScaleVar1, float ScaleVar2)
     : Sensor(F("Delta"), DELTA_NUM_VARIABLES,
              DELTA_WARM_UP_TIME_MS, DELTA_STABILIZATION_TIME_MS, DELTA_MEASUREMENT_TIME_MS,
              -1, -1, 1)
{
    _Var1 = Var1;
    _ScaleVar1 = ScaleVar1;
    _Var2 = Var2;
    _ScaleVar2 = ScaleVar2;
    // VariableUnits = Units;
}


String Delta::getSensorLocation(void)
{
    //No physical location to report 
    return "N/A";
}


bool Delta::setup(void)
{
    //No setup required
    return Sensor::setup();  // this will set timestamp and status bit
}


bool Delta::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float diff = -9999;
    if (_millisMeasurementRequested > 0)
    {
        float Val1 = (*_Var1).getValue();
        float Val2 = (*_Var2).getValue();

        diff = Val1/_ScaleVar1 - Val2/_ScaleVar2;  //Find the differance between the readings, including scalers

        if (isnan(diff)) diff = -9999;

        if(Val1 == -9999 || Val2 == -9999) diff = -9999; //Make sure both input variables are validw

        MS_DBG(F("Differential: "), diff);
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    verifyAndAddMeasurementResult(DELTA_DIFF_VAR_NUM, diff);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return success;
}
