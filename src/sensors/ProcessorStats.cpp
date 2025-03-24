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

// If the user only gave the version, we use info from known boards to fill in
// the gaps
ProcessorStats::ProcessorStats(const char* version,
                               uint8_t     measurementsToAverage)
    : Sensor(LOGGER_BOARD, PROCESSOR_NUM_VARIABLES, PROCESSOR_WARM_UP_TIME_MS,
             PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS, -1,
             -1, measurementsToAverage, PROCESSOR_INC_CALC_VARIABLES),
      _version(version),
      _boardName(LOGGER_BOARD) {
    // change the battery related settings for known boards
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
    _analogResolution = 10;
    _operatingVoltage = 3.3;
    _batteryPin       = A6;
    if (strcmp(_version, "v0.3") == 0 || strcmp(_version, "v0.4") == 0) {
        _batteryMultiplier = 1.47;
    } else if (strcmp(_version, "v0.5") == 0 || strcmp(_version, "v0.5b") ||
               strcmp(_version, "v1.0") || strcmp(_version, "v1.1") == 0) {
        _batteryMultiplier = 4.7;
    } else {
        _batteryPin        = -1;
        _batteryMultiplier = -1;
    }
#elif defined(ENVIRODIY_STONEFLY_M4)
    if (strcmp(_version, "v0.1") == 0) {
        _analogResolution  = 12;
        _operatingVoltage  = 3.3;
        _batteryPin        = A9;  // aka 75
        _batteryMultiplier = 4.7;
    } else {
        _batteryPin        = -1;
        _batteryMultiplier = -1;
    }
#elif defined(ARDUINO_AVR_FEATHER328P) || defined(ARDUINO_AVR_FEATHER32U4) ||  \
    defined(ARDUINO_SAMD_FEATHER_M0) || defined(SAMD_FEATHER_M0) ||            \
    defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) ||                                \
    defined(SAMD_FEATHER_M0_EXPRESS) || defined(ARDUINO_FEATHER_M4) ||         \
    defined(ADAFRUIT_FEATHER_M4_EXPRESS) || defined(ARDUINO_FEATHER_M4_CAN) || \
    defined(ADAFRUIT_FEATHER_M4_CAN) || defined(ADAFRUIT_FEATHER_M4_ADALOGGER)
    _analogResolution  = 10;
    _operatingVoltage  = 3.3;
    _batteryPin        = 9;
    _batteryMultiplier = 2;
#elif defined(ARDUINO_AVR_SODAQ_MBILI)
    _analogResolution  = 10;
    _operatingVoltage  = 3.3;
    _batteryPin        = A6;
    _batteryMultiplier = 1.47;
#elif defined(ARDUINO_AVR_SODAQ_NDOGO)
    _analogResolution  = 10;
    _operatingVoltage  = 3.3;
    _batteryPin        = 10;
    _batteryMultiplier = 1.47;
#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
    _analogResolution = 10;
    _operatingVoltage = 3.3;
    _batteryPin       = 10;
    if (strcmp(_version, "v0.1") == 0) {
        _batteryMultiplier = 2;
    } else if (strcmp(_version, "v0.2") == 0) {
        _batteryMultiplier = 1.47;
    } else {
        _batteryMultiplier = -1
    }
#elif defined(ARDUINO_SODAQ_AUTONOMO)
    _analogResolution  = 10;
    _operatingVoltage  = 3.3;
    _batteryMultiplier = 1.47;
    if (strcmp(_version, "v0.1") == 0)
        _batteryPin = 48;
    else
        _batteryPin = 33;
#else
    _batteryPin        = -1;
    _batteryMultiplier = -1;
#endif
}

// If the user specified all the processor info in the constructor, we don't
// need to do anything
ProcessorStats::ProcessorStats(const char* boardName, const char* version,
                               int8_t batteryPin, float batteryMultiplier,
                               int8_t analogResolution, float operatingVoltage,
                               uint8_t measurementsToAverage)
    : Sensor(LOGGER_BOARD, PROCESSOR_NUM_VARIABLES, PROCESSOR_WARM_UP_TIME_MS,
             PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS, -1,
             -1, measurementsToAverage, PROCESSOR_INC_CALC_VARIABLES),
      _version(version),
      _boardName(boardName),
      _batteryPin(batteryPin),
      _batteryMultiplier(batteryMultiplier),
      _analogResolution(analogResolution),
      _operatingVoltage(operatingVoltage) {}

// Destructor
ProcessorStats::~ProcessorStats() {}


String ProcessorStats::getSensorLocation(void) {
    return String(_boardName) + " " + String(_version);
}

float ProcessorStats::getBatteryVoltage(void) {
#if defined(ARDUINO_ARCH_SAMD)
    MS_DBG(F("Setting SAMD analog read resolution to"), _analogResolution,
           F("bits"));
    analogReadResolution(_analogResolution);
#endif

    float sensorValue_battery = -9999;
    if (_batteryPin >= 0 && _batteryMultiplier > 0) {
        // Get the battery voltage
        MS_DBG(F("Getting battery voltage from pin"), _batteryPin);
        pinMode(_batteryPin, INPUT);
        analogRead(_batteryPin);  // priming reading
        // The return value from analogRead() is IN BITS NOT IN VOLTS!!
        analogRead(_batteryPin);  // another priming reading
        float rawBattery = analogRead(_batteryPin);
        MS_DBG(F("Raw battery pin reading in bits:"), rawBattery);
        // convert bits to volts
        sensorValue_battery =
            (_operatingVoltage /
             static_cast<float>((1 << _analogResolution) - 1)) *
            _batteryMultiplier * rawBattery;
        MS_DBG(F("Battery in Volts:"), sensorValue_battery);
    } else {
        MS_DBG(F("No battery pin specified!"));
    }
    return sensorValue_battery;
}

#if defined(__SAMD51__)

uint8_t ProcessorStats::getLastResetCode() {
    return RSTC->RCAUSE.reg;
}
String ProcessorStats::getLastResetCause() {
    uint8_t lastResetCause = getLastResetCode();
    switch (lastResetCause) {
        case RSTC_RCAUSE_POR: return "Power On Reset";
        case RSTC_RCAUSE_BODCORE: return "Brown Out CORE Detector Reset";
        case RSTC_RCAUSE_BODVDD: return "Brown Out VDD Detector Reset";
        case RSTC_RCAUSE_NVM: return "NVM Reset";
        case RSTC_RCAUSE_EXT: return "External Reset";
        case RSTC_RCAUSE_WDT: return "Watchdog Reset";
        case RSTC_RCAUSE_SYST: return "System Reset Request";
        case RSTC_RCAUSE_BACKUP: return "Backup Reset";
        default: return "unknown";
    }
}

// SAMD21
#elif defined(ARDUINO_ARCH_SAMD)

extern "C" char* sbrk(int i);

/**
 * @brief Function to get the available free RAM on a SAMD board
 *
 * @return The available RAM
 */
int16_t FreeRam() {
    char stack_dummy = 0;
    return &stack_dummy - sbrk(0);
}

uint8_t ProcessorStats::getLastResetCode(void) {
    return PM->RCAUSE.reg;
}
String ProcessorStats::getLastResetCause() {
    uint8_t lastResetCause = getLastResetCode();
    switch (lastResetCause) {
        case PM_RCAUSE_POR: return "Power On Reset";
        case PM_RCAUSE_BOD12: return "Brown Out 12 Detector Reset";
        case PM_RCAUSE_BOD33: return "Brown Out 33 Detector Reset";
        case PM_RCAUSE_EXT: return "External Reset";
        case PM_RCAUSE_WDT: return "Watchdog Reset";
        case PM_RCAUSE_SYST: return "System Reset Request";
        default: return "unknown";
    }
}

#elif defined(__AVR__) || defined(ARDUINO_ARCH_AVR)

/**
 * @brief Function to get the available free RAM on an AVR board
 *
 * @return The available RAM
 */
int16_t FreeRam() {
    extern int16_t __heap_start, *__brkval;
    int16_t        v;
    float          sensorValue_freeRam = (int)&v -
        (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
    return sensorValue_freeRam;
}

uint8_t ProcessorStats::getLastResetCode(void) {
    return MCUSR;
}
String ProcessorStats::getLastResetCause() {
    uint8_t lastResetCause = getLastResetCode();
    switch (lastResetCause) {
        case PORF: return "Power On Reset";
        case EXTRF: return "External Reset";
        case BORF: return "Brown Out Detector Reset";
        case WDRF: return "Watchdog Reset";
        case JTRF: return "JTAG Reset Request";
        default: return "unknown";
    }
}
#endif


bool ProcessorStats::addSingleMeasurementResult(void) {
    float sensorValue_battery = getBatteryVoltage();
    verifyAndAddMeasurementResult(PROCESSOR_BATTERY_VAR_NUM,
                                  sensorValue_battery);

    // NOTE: Only running this section if there are no measurements already for
    // the RAM!
    if (numberGoodMeasurementsMade[PROCESSOR_RAM_VAR_NUM] == 0) {
        // Used only for debugging - can be removed
        MS_DBG(F("Getting Free RAM"));

#if !defined(__SAMD51__)
        float sensorValue_freeRam = FreeRam();
#else
        float sensorValue_freeRam = -9999;
#endif

        verifyAndAddMeasurementResult(PROCESSOR_RAM_VAR_NUM,
                                      sensorValue_freeRam);
    } else {
        MS_DBG(F("Skipping RAM measurement on reps"));
    }

    // NOTE: Only running this section if there are no measurements already for
    // the sample number! We do not want to bump up the sample number for each
    // averageable measurment, only for new measurements. This is a workaround
    // in case someone wants to average more than one mesurement of the battery
    // voltage.
    if (numberGoodMeasurementsMade[PROCESSOR_SAMPNUM_VAR_NUM] == 0) {
        // bump up the sample number
        sampNum += 1;

        verifyAndAddMeasurementResult(PROCESSOR_SAMPNUM_VAR_NUM, sampNum);
    } else {
        MS_DBG(F("Not bumping sample number on reps"));
    }

    // NOTE: Only running this section if there are no measurements already for
    // the reset cause!
    if (numberGoodMeasurementsMade[PROCESSOR_RESET_VAR_NUM] == 0) {
        // Used only for debugging - can be removed
        MS_DBG(F("Getting last reset cause"));

        float sensorValue_resetCode = getLastResetCode();
        MS_DBG(F("The most recent reset cause was"), sensorValue_resetCode, '(',
               getLastResetCause(), ')');
        verifyAndAddMeasurementResult(PROCESSOR_RESET_VAR_NUM,
                                      sensorValue_resetCode);
    } else {
        MS_DBG(F("Skipping reset cause check on reps"));
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
