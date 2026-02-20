/**
 * @file AOSongDHT.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the AOSongDHT class.
 */

#include "AOSongDHT.h"


// The constructor - need the power pin, data pin, and type of DHT
AOSongDHT::AOSongDHT(int8_t powerPin, int8_t dataPin, const uint8_t type,
                     uint8_t measurementsToAverage)
    : Sensor("AOSongDHT", DHT_NUM_VARIABLES, DHT_WARM_UP_TIME_MS,
             DHT_STABILIZATION_TIME_MS, DHT_MEASUREMENT_TIME_MS, powerPin,
             dataPin, measurementsToAverage, DHT_INC_CALC_VARIABLES),
      dht_internal(dataPin, type),
      _dhtType(type) {
    setAllowedMeasurementRetries(5);
}

// Destructor - does nothing.
AOSongDHT::~AOSongDHT() {}


bool AOSongDHT::setup(void) {
    dht_internal.begin();    // Start up the sensor (only sets pin modes, sensor
                             // power not required)
    return Sensor::setup();  // this will set pin modes and the setup status bit
}


String AOSongDHT::getSensorName(void) {
    switch (_dhtType) {
        case 11: return "AOSongDHT11";
        case 12: return "AOSongDHT12";
        case 21: return "AOSongDHT21";  // DHT 21 or AM2301
        default: return "AOSongDHT22";
    }
}


bool AOSongDHT::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success   = false;
    float humid_val = -9999;
    float temp_val  = -9999;
    float hi_val    = -9999;

    // Reading temperature or humidity takes about 250 milliseconds!
    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    // First read the humidity
    humid_val = dht_internal.readHumidity();
    // Read temperature as Celsius (the default)
    temp_val = dht_internal.readTemperature();
    // Check if any reads failed
    // If they are NaN (not a number) then something went wrong
    if (!isnan(humid_val) && !isnan(temp_val)) {
        // Compute heat index in Celsius (isFahrenheit = false)
        hi_val = dht_internal.computeHeatIndex(temp_val, humid_val, false);
        MS_DBG(F("  Temp:"), temp_val, F("°C"));
        MS_DBG(F("  Humidity:"), humid_val, '%');
        MS_DBG(F("  Calculated Heat Index:"), hi_val, F("°C"));
        verifyAndAddMeasurementResult(DHT_TEMP_VAR_NUM, temp_val);
        verifyAndAddMeasurementResult(DHT_HUMIDITY_VAR_NUM, humid_val);
        verifyAndAddMeasurementResult(DHT_HI_VAR_NUM, hi_val);
        success = true;
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
