/**
 * @file Decagon5TM.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Decagon5TM class.
 */

#include "Decagon5TM.h"

bool Decagon5TM::getResults(void) {
    // Set up the float variables for receiving data
    float ea   = -9999;
    float temp = -9999;

    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
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
    while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {
        // wait
    }
    // read the returned address to remove it from the buffer
    auto returnedAddress = static_cast<char>(_SDI12Internal.read());
    // print out a warning if the address doesn't match up
    if (returnedAddress != _SDI12address) {
        MS_DBG(F("Warning, expecting data from"), _SDI12address,
               F("but got data from"), returnedAddress);
    }
    // Start printing out the returned data
    MS_DEEP_DBG(F("    <<<"), returnedAddress);

    // read the '+' out of the buffer, and print it if we're debugging
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    // if we're not debugging, just read the character to make sure
    // it's removed from the buffer
    _SDI12Internal.read();
#endif

    // First variable returned is the Dialectric E
    ea = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<<"), String(ea, 10));

    // read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Now read the temperature
    temp = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<<"), String(temp, 10));

    // read and dump anything else
    while (_SDI12Internal.available()) {
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
        MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
        _SDI12Internal.read();
#endif
    }

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    MS_DBG(F("Raw dielectric permittivity:"), ea);
    MS_DBG(F("Raw Temperature Value:"), temp);


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

    verifyAndAddMeasurementResult(TM_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(TM_EA_VAR_NUM, ea);
    verifyAndAddMeasurementResult(TM_VWC_VAR_NUM, VWC);

    return temp != -9999;
}
