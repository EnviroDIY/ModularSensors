/**
 * @file MeterTeros11.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MeterTeros11 class.
 */

#include "MeterAtmos14.h"


bool MeterAtmos14::getResults(void) {
    // Set up the float variables for receiving data
    float rh  = -9999;
    float airTemp = -9999;
    float airPress = -9999;
    float vPress = -9999;

    // MS_DEEP_DBG(F("   Activating SDI-12 instance for"),
    //        getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // if (wasActive) {
    //     MS_DEEP_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //                 F("was already active!"));
    // }
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set
    // correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    String getDataCommand = "";
    getDataCommand += _SDI12address;
    // SDI-12 command to get data [address][D][dataOption][!]
    getDataCommand += "D0!";
    _SDI12Internal.sendCommand(getDataCommand, _extraWakeTime);
    delay(30);  // It just needs this little delay
    MS_DEEP_DBG(F("    >>>"), getDataCommand);

    // Wait for the first few charaters to arrive.  The response from a data
    // request should always have more than three characters
    uint32_t start = millis();
    while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {}
    // read the returned address to remove it from the buffer
    char returnedAddress = _SDI12Internal.read();
    // print out a warning if the address doesn't match up
    if (returnedAddress != _SDI12address) {
        MS_DBG(F("Warning, expecting data from"), _SDI12address,
               F("but got data from"), returnedAddress);
    }
    // Start printing out the returned data
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(returnedAddress));

    
    // While there is any data left in the buffer
    while (_SDI12Internal.available()) {
        // First peek to see if the next character in the buffer is a number
        int c = _SDI12Internal.peek();
        // if there's a number, a decimal, or a negative sign next in the
        // buffer, start reading it as a float.
        if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
            // Now read the air temperature
            // Read the float without skipping any in-valid characters.
            // We don't want to skip anything because we want to be able to
            // debug and see exactly which characters the sensor sent over
            // if they weren't numbers.
            // Reading the numbers as a float will remove them from the
            // buffer.
            // Read the vapor pressure
            vPress = _SDI12Internal.parseFloat(SKIP_NONE);
            MS_DBG(F("    <<<"), String(vPress, 10));
            break;
        } else {
            // if we're debugging print out the non-numeric character
            #ifdef MS_SDI12SENSORS_DEBUG_DEEP
                MS_DEEP_DBG(F("    <<<"),static_cast<char>(_SDI12Internal.read()));
            #else
                // if we're not debugging, just read the character to make sure
                // it's removed from the buffer
                _SDI12Internal.read();
            #endif
        }
        delay(10);  // 1 character ~ 7.5ms
    }

    // While there is any data left in the buffer
    while (_SDI12Internal.available()) {
        // First peek to see if the next character in the buffer is a number
        int c = _SDI12Internal.peek();
        // if there's a number, a decimal, or a negative sign next in the
        // buffer, start reading it as a float.
        if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
            // Now read the air temperature
            // Read the float without skipping any in-valid characters.
            // We don't want to skip anything because we want to be able to
            // debug and see exactly which characters the sensor sent over
            // if they weren't numbers.
            // Reading the numbers as a float will remove them from the
            // buffer.
            airTemp = _SDI12Internal.parseFloat(SKIP_NONE);
            MS_DBG(F("    <<<"), String(airTemp, 10));
            break;
        } else {
            // if we're debugging print out the non-numeric character
            #ifdef MS_SDI12SENSORS_DEBUG_DEEP
                MS_DEEP_DBG(F("    <<<"),static_cast<char>(_SDI12Internal.read()));
            #else
                // if we're not debugging, just read the character to make sure
                // it's removed from the buffer
                _SDI12Internal.read();
            #endif
        }
        delay(10);  // 1 character ~ 7.5ms
    }

    // While there is any data left in the buffer
    while (_SDI12Internal.available()) {
        // First peek to see if the next character in the buffer is a number
        int c = _SDI12Internal.peek();
        Serial.println(c);
        // if there's a number, a decimal, or a negative sign next in the
        // buffer, start reading it as a float.
        if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
            // Now read the relative humidity
            // Read the float without skipping any in-valid characters.
            // We don't want to skip anything because we want to be able to
            // debug and see exactly which characters the sensor sent over
            // if they weren't numbers.
            // Reading the numbers as a float will remove them from the
            // buffer.
            rh = _SDI12Internal.parseFloat(SKIP_NONE);
            MS_DBG(F("    <<<"), String(rh, 10));
            break;
        } else {
            // if we're debugging print out the non-numeric character
            #ifdef MS_SDI12SENSORS_DEBUG_DEEP
                MS_DEEP_DBG(F("    <<<"),static_cast<char>(_SDI12Internal.read()));
            #else
                // if we're not debugging, just read the character to make sure
                // it's removed from the buffer
                _SDI12Internal.read();
            #endif
        }
        delay(10);  // 1 character ~ 7.5ms
    }

    // While there is any data left in the buffer
    while (_SDI12Internal.available()) {
        // First peek to see if the next character in the buffer is a number
        int c = _SDI12Internal.peek();
        Serial.println(c);
        // if there's a number, a decimal, or a negative sign next in the
        // buffer, start reading it as a float.
        if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
            // Now read the atmospheric pressure
            // Read the float without skipping any in-valid characters.
            // We don't want to skip anything because we want to be able to
            // debug and see exactly which characters the sensor sent over
            // if they weren't numbers.
            // Reading the numbers as a float will remove them from the
            // buffer.
            // Now read the atmospheric pressure
            airPress = _SDI12Internal.parseFloat(SKIP_NONE);
            MS_DBG(F("    <<<"), String(airPress, 10));
            break;
        } else {
            // if we're debugging print out the non-numeric character
            #ifdef MS_SDI12SENSORS_DEBUG_DEEP
                MS_DEEP_DBG(F("    <<<"),static_cast<char>(_SDI12Internal.read()));
            #else
                // if we're not debugging, just read the character to make sure
                // it's removed from the buffer
                _SDI12Internal.read();
            #endif
        }
        delay(10);  // 1 character ~ 7.5ms
    }

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    MS_DBG(F("Raw VWC Counts:"), vPress);
    MS_DBG(F("Raw Temperature Value:"), airTemp);
    MS_DBG(F("Raw Relative Humidity Value:"), rh);
    MS_DBG(F("Raw Atmospheric Pressure Value:"), airPress);

    // Set up the float variables for calculated variable
    float rh_calc  = -9999;
    float vPress_calc = -9999;
    float airPress_calc = -9999;

    if (vPress != -9999) {
        vPress_calc = vPress * 10;
        MS_DBG(F("Calculated vPress:"), vPress_calc);
    }

    // calculate
    if (rh != -9999) {
        rh_calc = rh * 100;

        MS_DBG(F("Calculated RH:"), rh_calc);
        if (rh_calc < 0) {
            rh_calc = 0;
            MS_DBG(F("Setting negative RH to 0"));
        }
        if (rh_calc > 100) {
            rh_calc = 100;
            MS_DBG(F("Setting RH >100 to 100"));
        }
    }

    if (airPress != -9999) {
        airPress_calc = airPress * 10;
        MS_DBG(F("Calculated airPress:"), airPress_calc);
    }

    verifyAndAddMeasurementResult(ATMOS14_VP_VAR_NUM, vPress_calc);
    verifyAndAddMeasurementResult(ATMOS14_ATEMP_VAR_NUM, airTemp);
    verifyAndAddMeasurementResult(ATMOS14_RH_VAR_NUM, rh_calc);
    verifyAndAddMeasurementResult(ATMOS14_APRESS_VAR_NUM, airPress_calc);

}
