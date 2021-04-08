/**
 * @file STSTC3100_Sensor.cpp
 * @copyright 2020 Neil Hancock, assigned to the Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Neil Hancock.  Based on the AtlasParent.xx of the Stroud Water Research Center
 *
 * @brief Implements the STSTC3100_Sensor class.
 */

#include "STSTC3100_Sensor.h"
#include <Wire.h>

STSTC3100_Sensor::STSTC3100_Sensor(TwoWire* theI2C, 
                         uint8_t measurementsToAverage,
                         const char* sensorName, const uint8_t numReturnedVars,
                         uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms,
                         uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, -1, -1, measurementsToAverage) {
    _i2c           = theI2C;
}
STSTC3100_Sensor::STSTC3100_Sensor(
                         uint8_t measurementsToAverage, const char* sensorName,
                         const uint8_t numReturnedVars, uint32_t warmUpTime_ms,
                         uint32_t stabilizationTime_ms,
                         uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, -1, -1, measurementsToAverage) {
    _i2c           = &Wire;
}


STSTC3100_Sensor::~STSTC3100_Sensor() {}


String      STSTC3100_Sensor::getSensorLocation(void) {
    String address = F(" I2C_0x");
    address += String(_i2cAddressHex, HEX);
    // Could add the serial number at some point, but not much value here
    //String address = F("Sn ");
    //Must have done stc3100_device.start() to read string
    //address += stc3100_device.getSn();

    return address;
}


bool STSTC3100_Sensor::setup(void) {
    //njh _i2c->begin();  // Start the wire library (sensor power not required)
    MS_DBG(F("Setup"));
    stc3100_device.begin();
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    //njh _i2c->setTimeout(0);

    //Reads unique serial number
    if(!stc3100_device.start()){
        MS_DBG(F("Not detected!"));
        return false;
    }
    return Sensor::setup();  // this will set pin modes and the setup status bit
}

#if 0
// The Stc3100 sensors can be put into low power mode, but this changes the setup time
bool STSTC3100_Sensor::sleep(void) {
    if (!checkPowerOn()) { return true; }
    if (_millisSensorActivated == 0) {
        MS_DBG(getSensorNameAndLocation(), F("was not measuring!"));
        return true;
    }

    bool success = false;
    MS_DBG(F("Putting"), getSensorNameAndLocation(), F("to sleep"));
/*
    _i2c->beginTransmission(_i2cAddressHex);
    success &= _i2c->write((const uint8_t*)"Sleep",
                           5);  // Write "Sleep" to put it in low power mode
    success &= !_i2c->endTransmission();
    // NOTE: The return of 0 from endTransmission indicates success
*/
    if (success) {
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor activation (bits 3 & 4) and
        // measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Done"));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("did not accept sleep command"));
    }

    return success;
}
#endif //0


bool STSTC3100_Sensor::addSingleMeasurementResult(void) {
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {

        uint8_t code = stc3100_device.readValues(); // 0 == success 
        switch (code) {
            case 0:  // the command was successful.
                MS_DBG(getSensorNameAndLocation(),F("  Measurement successful"));
                success = true;
                break;

            default:  // the command has failed.
                MS_DBG(getSensorNameAndLocation(),F("  Measurement Failed="),code );
                break;
        }
        // If the response code is successful, parse the remaining results
        if (success) {
            float result;

            for (uint8_t snsrn = 0; snsrn < _numReturnedValues; snsrn++) {
                switch (snsrn) {
                case STC3100_BUS_VOLTAGE_VAR_NUM :
                    result=stc3100_device.v.voltage_V; 
                    if (result < -0.2) result = STC3100_SENSOR_INVALID;
                    break; 
                case STC3100_CURRENT_MA_VAR_NUM : 
                    result=stc3100_device.v.current_mA;
                    // This is based on the 30mOhms, and is unlikely to happen
                    if (result < -7000.0) {
                        MS_DBG(F("  Current_MA read invalid"),  result);
                        result = STC3100_SENSOR_INVALID;
                        } // Could probably test for +7000A if needed.
                    break;
                case STC3100_USED1_MAH_VAR_NUM: 
                    // For 30mOhms, the max could be +/-7000 mAhr and is unlikely to happen
                    result=stc3100_device.getEnergyUsed1_mAhr();
                    break;
                case STC3100_AVLBL_MAH_VAR_NUM: 
                    // For 30mOhms, the max could be +/-7000 mAhr and is unlikely to happen
                    result=stc3100_device.getEnergyAvlbl_mAhr();
                    break;
                default:
                    result = STC3100_SENSOR_INVALID; 
                }
                if (isnan(result)) result = STC3100_SENSOR_INVALID;

                MS_DBG(F("  Result #"), snsrn, ':', result);
                verifyAndAddMeasurementResult(snsrn, result);
            } 
        }
    } else {
        // If there's no measurement, need to make sure we send over all
        // of the "failed" result values
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
        for (uint8_t i = 0; i < _numReturnedValues; i++) {
            verifyAndAddMeasurementResult(i, static_cast<float>(-9999));
        }
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}

String STSTC3100_Sensor::getSensorDetails(void) {
    String sensorDetails(F("RS485 Wingboard STC3100 Sn "));
    sensorDetails += stc3100_device.getSn();
    return sensorDetails;
}