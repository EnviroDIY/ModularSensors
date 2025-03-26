/**
 * @file Decagon5TM.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Decagon5TM class.
 */

#include "Decagon5TM.h"

bool Decagon5TM::getResults(bool verify_crc) {
    // run the parent getResults method
    bool success = SDI12Sensors::getResults(verify_crc);

    // pull the ea and temperature values from the buffer that getResults filled
    float ea   = sensorValues[TM_EA_VAR_NUM];
    float temp = sensorValues[TM_TEMP_VAR_NUM];

    // Set up the float variables for calculated variable
    float VWC = -9999;

    // Calculate the VWC from EA using the Topp equation
    // range check
    if (ea < 0 || ea > 350) {
        MS_DBG(F("WARNING:  Ea results out of range (0-350)!  Cannot calculate "
                 "VWC"));
        ea = -9999;
    }
    // calculate
    if (ea != -9999) {
        VWC = (4.3e-6 * (ea * ea * ea)) - (5.5e-4 * (ea * ea)) +
            (2.92e-2 * ea) - 5.3e-2;
        VWC *= 100;  // Convert to actual percent
        MS_DBG(F("Calculated VWC:"), ea);
        if (VWC < 0) {
            VWC = 0;
            MS_DBG(F("Setting negative VWC to 0"));
        }
        if (VWC > 100) {
            VWC = 100;
            MS_DBG(F("Setting VWC >100 to 100"));
        }
    }

    // VWC = 3.879e-4*raw-0.6956;  // equation for mineral soils

    // range check on temp; range is - 40°C to + 50°C
    if (temp < -50 || temp > 60) {
        temp = -9999;
        MS_DBG(F("WARNING:  temperature results out of range (-50-60)!"));
    }

    // re-add to the buffer after calculation/validation
    verifyAndAddMeasurementResult(TM_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(TM_EA_VAR_NUM, ea);
    verifyAndAddMeasurementResult(TM_VWC_VAR_NUM, VWC);

    return success && temp != -9999;
}
