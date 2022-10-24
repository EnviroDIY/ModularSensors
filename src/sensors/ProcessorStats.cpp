/**
 * @file ProcessorStats.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ProcessorStats class.
 */

#include "ProcessorStats.h"

// EnviroDIY boards
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
/// @brief Pretty text for the board name derived from the board's compiler
/// define.
#define BOARD "EnviroDIY Mayfly"

// Sodaq boards
#elif defined(ARDUINO_SODAQ_EXPLORER)
#define BOARD "SODAQ ExpLoRer"
#elif defined(ARDUINO_SODAQ_AUTONOMO)
#define BOARD "SODAQ Autonomo"
#elif defined(ARDUINO_SODAQ_ONE_BETA)
#define BOARD "SODAQ ONE Beta"
#elif defined(ARDUINO_SODAQ_ONE)
#define BOARD "SODAQ ONE"
#elif defined(ARDUINO_AVR_SODAQ_MBILI)
#define BOARD "SODAQ Mbili"
#elif defined(ARDUINO_AVR_SODAQ_NDOGO)
#define BOARD "SODAQ Ndogo"
#elif defined(ARDUINO_AVR_SODAQ_TATU)
#define BOARD "SODAQ Tatu"
#elif defined(ARDUINO_AVR_SODAQ_MOJA)
#define BOARD "SODAQ Moja"

// Adafruit boards
#elif defined(ARDUINO_AVR_FEATHER32U4)
#define BOARD "Feather 32u4"
#elif defined(ARDUINO_SAMD_FEATHER_M0)
#define BOARD "Feather M0"
#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
#define BOARD "Feather M0 Express"

// Arduino boards
#elif defined(ARDUINO_AVR_ADK)
#define BOARD "Mega Adk"
#elif defined(ARDUINO_AVR_BT)  // Bluetooth
#define BOARD "Bt"
#elif defined(ARDUINO_AVR_DUEMILANOVE)
#define BOARD "Duemilanove"
#elif defined(ARDUINO_AVR_ESPLORA)
#define BOARD "Esplora"
#elif defined(ARDUINO_AVR_ETHERNET)
#define BOARD "Ethernet"
#elif defined(ARDUINO_AVR_FIO)
#define BOARD "Fio"
#elif defined(ARDUINO_AVR_GEMMA)
#define BOARD "Gemma"
#elif defined(ARDUINO_AVR_LEONARDO)
#define BOARD "Leonardo"
#elif defined(ARDUINO_AVR_LILYPAD)
#define BOARD "Lilypad"
#elif defined(ARDUINO_AVR_LILYPAD_USB)
#define BOARD "Lilypad Usb"
#elif defined(ARDUINO_AVR_MEGA)
#define BOARD "Mega"
#elif defined(ARDUINO_AVR_MEGA2560)
#define BOARD "Mega 2560"
#elif defined(ARDUINO_AVR_MICRO)
#define BOARD "Micro"
#elif defined(ARDUINO_AVR_MINI)
#define BOARD "Mini"
#elif defined(ARDUINO_AVR_NANO)
#define BOARD "Nano"
#elif defined(ARDUINO_AVR_NG)
#define BOARD "NG"
#elif defined(ARDUINO_AVR_PRO)
#define BOARD "Pro"
#elif defined(ARDUINO_AVR_ROBOT_CONTROL)
#define BOARD "Robot Ctrl"
#elif defined(ARDUINO_AVR_ROBOT_MOTOR)
#define BOARD "Robot Motor"
#elif defined(ARDUINO_AVR_UNO)
#define BOARD "Uno"
#elif defined(ARDUINO_AVR_YUN)
#define BOARD "Yun"
#elif defined(ARDUINO_SAMD_ZERO)
#define BOARD "Zero"

#else
#define BOARD "Unknown"
#endif


// Need to know the Mayfly version because the battery resistor depends on it
ProcessorStats::ProcessorStats(const char* version)
    : Sensor(BOARD, PROCESSOR_NUM_VARIABLES, PROCESSOR_WARM_UP_TIME_MS,
             PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS, -1,
             -1, 1, PROCESSOR_INC_CALC_VARIABLES),
      _version(version) {
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) || defined(ARDUINO_AVR_SODAQ_MBILI)
    _batteryPin = A6;
#elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || \
    defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
    _batteryPin        = 9;
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
    return BOARD;
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
    MS_DBG(F("Getting battery voltage"));

    float sensorValue_battery = -9999;

#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
    if (strcmp(_version, "v0.3") == 0 || strcmp(_version, "v0.4") == 0) {
        // Get the battery voltage
        // The return value from analogRead() is IN BITS NOT IN VOLTS!!
        analogRead(_batteryPin);  // priming reading
        float rawBattery = analogRead(_batteryPin);
        // convert bits to volts
        sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
    }
    if (strcmp(_version, "v0.5") == 0 || strcmp(_version, "v0.5b") ||
        strcmp(_version, "v1.0") || strcmp(_version, "v1.1") == 0) {
        // Get the battery voltage
        // The return value from analogRead() is IN BITS NOT IN VOLTS!!
        analogRead(_batteryPin);  // priming reading
        float rawBattery = analogRead(_batteryPin);
        // convert bits to volts
        sensorValue_battery = (3.3 / 1023.) * 4.7 * rawBattery;
    }

#elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || \
    defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
    float measuredvbat = analogRead(_batteryPin);
    measuredvbat *= 2;     // we divided by 2, so multiply back
    measuredvbat *= 3.3;   // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024;  // convert to voltage
    sensorValue_battery       = measuredvbat;

#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
    if (strcmp(_version, "v0.1") == 0) {
        // Get the battery voltage
        float rawBattery    = analogRead(_batteryPin);
        sensorValue_battery = (3.3 / 1023.) * 2 * rawBattery;
    }
    if (strcmp(_version, "v0.2") == 0) {
        // Get the battery voltage
        float rawBattery    = analogRead(_batteryPin);
        sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
    }

#elif defined(ARDUINO_AVR_SODAQ_NDOGO) || defined(ARDUINO_SODAQ_AUTONOMO) || \
    defined(ARDUINO_AVR_SODAQ_MBILI)
    // Get the battery voltage
    float rawBattery    = analogRead(_batteryPin);
    sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;

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
