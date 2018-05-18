/*
 *Decagon5TM.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the SDI12Sensors super class.
 *
 *Documentation for the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 * http://manuals.decagon.com/Integration%20Guides/5TM%20Integrators%20Guide.pdf
 *
 * For Ea and VWC:
 *     Resolution is 0.0008 m3/m3 (0.08% VWC) from 0 – 50% VWC
 *     Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 *     Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 *     Range is 0 – 1 m3/m3 (0 – 100% VWC)
 *
 * For Temperature:
 *     Resolution is 0.1°C
 *     Accuracy is ± 1°C
 *     Range is - 40°C to + 50°C
 *
 * Maximum warm-up time in SDI-12 mode: 200ms, assume stability at warm-up
 * Maximum measurement duration: 200ms
*/

#include "Decagon5TM.h"

bool Decagon5TM::addSingleMeasurementResult(void)
{
    if (_millisMeasurementRequested > 0)
    {
        MS_DBG(F("   Activating SDI-12 instance for "), getSensorName(),
               F(" at "), getSensorLocation(), '\n');
        // Make this the currently active SDI-12 Object
        // Use begin() instead of just setActive() to ensure timer is set correctly.
        _SDI12Internal.begin();;
        // Empty the buffer
        _SDI12Internal.clearBuffer();

        // Set up variables for receiving data
        float ea = -9999;
        float temp = -9999;
        float VWC = -9999;

        MS_DBG(F("   Requesting data from "), getSensorName(),
               F(" at "), getSensorLocation(), '\n');
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        getDataCommand += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        _SDI12Internal.sendCommand(getDataCommand);
        delay(30);  // It just needs this little delay
        MS_DBG(F("      >>> "), getDataCommand, F("\n"));

        uint32_t startTime = millis();
        while (_SDI12Internal.available() < 3 && (millis() - startTime) < 1500) {}
        MS_DBG(F("   Receiving results from "), getSensorName(),
               F(" at "), getSensorLocation(), '\n');
        _SDI12Internal.read();  // ignore the repeated SDI12 address
        // First variable returned is the Dialectric E
        ea = _SDI12Internal.parseFloat();
        if (ea < 0 || ea > 350) ea = -9999;
        // Second variable returned is the temperature in °C
        temp = _SDI12Internal.parseFloat();
        if (temp < -50 || temp > 60) temp = -9999;  // Range is - 40°C to + 50°C
        // the "third" variable of VWC is actually calculated, not returned by the sensor!
        if (ea != -9999)
        {
            VWC = (4.3e-6*(ea*ea*ea))
                        - (5.5e-4*(ea*ea))
                        + (2.92e-2 * ea)
                        - 5.3e-2 ;
            VWC *= 100;  // Convert to actual percent
        }

        // String sdiResponse = _SDI12Internal.readStringUntil('\n');
        // sdiResponse.trim();
        // _SDI12Internal.clearBuffer();
        // MS_DBG(F("      <<< "), sdiResponse, F("\n"));

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        // De-activate the SDI-12 Object
        // Use end() instead of just forceHold to un-set the timers
        _SDI12Internal.end();

        MS_DBG(F("Dialectric E: "), ea);
        MS_DBG(F(" Temperature: "), temp);
        MS_DBG(F(" Volumetric Water Content: "), VWC, F("\n"));

        verifyAndAddMeasurementResult(TM_EA_VAR_NUM, ea);
        verifyAndAddMeasurementResult(TM_TEMP_VAR_NUM, temp);
        verifyAndAddMeasurementResult(TM_VWC_VAR_NUM, VWC);

        // Unset the time stamp for the beginning of this measurement
        _millisMeasurementRequested = 0;
        // Unset the status bit for a measurement having been requested (bit 5)
        _sensorStatus &= 0b11011111;
        // Set the status bit for measurement completion (bit 6)
        _sensorStatus |= 0b01000000;

        // Return true when finished
        return true;
    }
    else
    {
        MS_DBG(F("   "), getSensorName(), F(" at "), getSensorLocation(),
               F(" is not currently measuring!\n"));
        return false;
    }
}
