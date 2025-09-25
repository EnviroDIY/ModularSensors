/**
 * @file MeterTeros11.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MeterTeros11 class.
 */

#include "MeterTeros11.h"


bool MeterTeros11::getResults(bool verify_crc) {
    // run the parent getResults method
    bool success = SDI12Sensors::getResults(verify_crc);

    // pull the raw count and temperature values from the buffer that getResults
    // filled
    float temp = sensorValues[TEROS11_TEMP_VAR_NUM];
    float raw  = sensorValues[TEROS11_COUNT_VAR_NUM];

    // Set up the float variables for calculated variable
    float ea  = -9999;
    float VWC = -9999;

    // Calculate the dielectric EA from the raw count value.
    // Equation 8 from the Teros 11 user manual:
    // http://publications.metergroup.com/Manuals/20587_TEROS11-12_Manual_Web.pdf
    if (raw < 0 || raw > 5000) {
        MS_DBG(
            F("WARNING:  raw results out of range (0-5000)!  Cannot calculate "
              "Ea or VWC"));
        raw = -9999;
    }
    if (raw != -9999) {
        ea = ((2.887e-9 * (raw * raw * raw)) - (2.08e-5 * (raw * raw)) +
              (5.276e-2 * raw) - 43.39) *
            ((2.887e-9 * (raw * raw * raw)) - (2.08e-5 * (raw * raw)) +
             (5.276e-2 * raw) - 43.39);
        MS_DBG(F("Calculated Ea:"), ea);
    }

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
    verifyAndAddMeasurementResult(TEROS11_COUNT_VAR_NUM, raw);
    verifyAndAddMeasurementResult(TEROS11_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(TEROS11_EA_VAR_NUM, ea);
    verifyAndAddMeasurementResult(TEROS11_VWC_VAR_NUM, VWC);

    return success && temp != -9999;
}
