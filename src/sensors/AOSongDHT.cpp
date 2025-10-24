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


/**
 * @brief Construct an AOSongDHT sensor with specified power/data pins and DHT type.
 *
 * Initializes the sensor base with the AOSongDHT configuration, creates the internal
 * DHT instance bound to the given data pin and type, stores the sensor type, and
 * configures the allowed measurement retry count to 5.
 *
 * @param powerPin Digital pin used to power the sensor (can be -1 if sensor is always powered).
 * @param dataPin Digital data pin connected to the DHT sensor.
 * @param type DHT sensor type identifier (e.g., 11, 12, 21, 22).
 * @param measurementsToAverage Number of measurements to average for each reported value.
 */
AOSongDHT::AOSongDHT(int8_t powerPin, int8_t dataPin, const uint8_t type,
                     uint8_t measurementsToAverage)
    : Sensor("AOSongDHT", DHT_NUM_VARIABLES, DHT_WARM_UP_TIME_MS,
             DHT_STABILIZATION_TIME_MS, DHT_MEASUREMENT_TIME_MS, powerPin,
             dataPin, measurementsToAverage, DHT_INC_CALC_VARIABLES),
      dht_internal(dataPin, type),
      _dhtType(type) {
    setAllowedMeasurementRetries(5);
}

/**
 * @brief Destructor for AOSongDHT.
 *
 * No resources are released and no special cleanup is performed.
 */
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


/**
 * @brief Reads the DHT sensor and records temperature, relative humidity, and heat index when a measurement was started.
 *
 * If the sensor measurement was not started (MEASUREMENT_SUCCESSFUL is false), this function updates the measurement attempt
 * count and returns without reading the sensor. When reads succeed, temperature and heat index are in degrees Celsius and
 * humidity is in percent; the three values are validated and recorded for the sensor.
 *
 * @return `true` if the measurement values were validated and recorded, `false` otherwise.
 */
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