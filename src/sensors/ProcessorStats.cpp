/**
 * @file ProcessorStats.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ProcessorStats class.
 */

#include "ProcessorStats.h"

// Need to know the Mayfly version because the battery resistor depends on it
ProcessorStats::ProcessorStats(const char* version)
    : Sensor(LOGGER_BOARD, PROCESSOR_NUM_VARIABLES, PROCESSOR_WARM_UP_TIME_MS,
             PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS, -1,
             -1, 1, PROCESSOR_INC_CALC_VARIABLES),
      _version(version) {
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) || defined(ARDUINO_AVR_SODAQ_MBILI)
    _batteryPin = A6;
#elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || \
    defined(SAMD_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) ||   \
    defined(SAMD_FEATHER_M0_EXPRESS)
    _batteryPin = 9;
#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA) || \
    defined(ARDUINO_AVR_SODAQ_NDOGO)
    _batteryPin = 10;
#elif defined(ARDUINO_SODAQ_AUTONOMO)
    if (strcmp(_version, "v0.1") == 0)
        _batteryPin = 48;
    else
        _batteryPin = 33;
#else
    _batteryPin         = -1;
#endif
}
// Destructor
ProcessorStats::~ProcessorStats() {}


String ProcessorStats::getSensorLocation(void) {
    return LOGGER_BOARD;
}


#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)
extern "C" char* sbrk(int i);

int16_t FreeRam() {
    char stack_dummy = 0;
    return &stack_dummy - sbrk(0);
}
#endif


bool ProcessorStats::addSingleMeasurementResult(void) {
    // Get the battery voltage
    MS_DBG(F("Getting battery voltage from pin"), _batteryPin);

    float sensorValue_battery = -9999;
    analogRead(_batteryPin);  // priming reading

#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
    if (strcmp(_version, "v0.3") == 0 || strcmp(_version, "v0.4") == 0) {
        // Get the battery voltage
        // The return value from analogRead() is IN BITS NOT IN VOLTS!!
        analogRead(_batteryPin);  // priming reading
        float rawBattery = analogRead(_batteryPin);
        MS_DBG(F("Raw battery pin reading in bits:"), rawBattery);
        // convert bits to volts
        sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
        MS_DBG(F("Battery in Volts:"), sensorValue_battery);
    }
    if (strcmp(_version, "v0.5") == 0 || strcmp(_version, "v0.5b") ||
        strcmp(_version, "v1.0") || strcmp(_version, "v1.1") == 0) {
        // Get the battery voltage
        // The return value from analogRead() is IN BITS NOT IN VOLTS!!
        analogRead(_batteryPin);  // priming reading
        float rawBattery = analogRead(_batteryPin);
        MS_DBG(F("Raw battery pin reading in bits:"), rawBattery);
        // convert bits to volts
        sensorValue_battery = (3.3 / 1023.) * 4.7 * rawBattery;
        MS_DBG(F("Battery in Volts:"), sensorValue_battery);
    }

#elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || \
    defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
    analogRead(_batteryPin);  // priming reading
    float measuredvbat = analogRead(_batteryPin);
    measuredvbat *= 2;     // we divided by 2, so multiply back
    measuredvbat *= 3.3;   // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024;  // convert to voltage
    sensorValue_battery       = measuredvbat;

#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
    if (strcmp(_version, "v0.1") == 0) {
        // Get the battery voltage
        analogRead(_batteryPin);  // priming reading
        float rawBattery = analogRead(_batteryPin);
        MS_DBG(F("Raw battery pin reading in bits:"), rawBattery);
        sensorValue_battery = (3.3 / 1023.) * 2 * rawBattery;
        MS_DBG(F("Battery in Volts:"), sensorValue_battery);
    }
    if (strcmp(_version, "v0.2") == 0)
        analogRead(_batteryPin);  // priming reading{
    // Get the battery voltage
    float rawBattery = analogRead(_batteryPin);
    MS_DBG(F("Raw battery pin reading in bits:"), rawBattery);
    sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
    MS_DBG(F("Battery in Volts:"), sensorValue_battery);
}

#elif defined(ARDUINO_AVR_SODAQ_NDOGO) || defined(ARDUINO_SODAQ_AUTONOMO) || \
    defined(ARDUINO_AVR_SODAQ_MBILI)
    // Get the battery voltage
    analogRead(_batteryPin);  // priming reading
    float rawBattery = analogRead(_batteryPin);
    MS_DBG(F("Raw battery pin reading in bits:"), rawBattery);
    sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
    MS_DBG(F("Battery in Volts:"), sensorValue_battery);

#else
    sensorValue_battery = -9999;

#endif

    verifyAndAddMeasurementResult(PROCESSOR_BATTERY_VAR_NUM,
                                  sensorValue_battery);

    // Used only for debugging - can be removed
    MS_DBG(F("Getting Free RAM"));

#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
    extern int16_t __heap_start, *__brkval;
    int16_t        v;
    float          sensorValue_freeRam = (int)&v -
        (__brkval == 0 ? (int)&__heap_start : (int)__brkval);

#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)
    float sensorValue_freeRam = FreeRam();

#else
float sensorValue_freeRam = -9999;
#endif

    verifyAndAddMeasurementResult(PROCESSOR_RAM_VAR_NUM, sensorValue_freeRam);

    // bump up the sample number
    sampNum += 1;

    verifyAndAddMeasurementResult(PROCESSOR_SAMPNUM_VAR_NUM, sampNum);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
