/*
 *ProcessorStats.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for metadata on the processor functionality.
 *
 *For battery voltage:
 *  Range of 0-5V with 10bit ADC - resolution of 0.005
 *
 * If the processor is awake, it's ready to take a reading.
*/

#include "ProcessorStats.h"

// EnviroDIY boards
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
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
#elif defined(ARDUINO_AVR_BT)    // Bluetooth
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
ProcessorStats::ProcessorStats(const char *version)
    : Sensor(F(BOARD), PROCESSOR_NUM_VARIABLES,
             PROCESSOR_WARM_UP_TIME_MS, PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS,
             -1, -1, 1)
{
    _version = version;

    #if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) || defined(ARDUINO_AVR_SODAQ_MBILI)
        _batteryPin = A6;
    #elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
        _batteryPin = 9;
    #elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA) || defined(ARDUINO_AVR_SODAQ_NDOGO)
        _batteryPin = 10;
    #elif defined(ARDUINO_SODAQ_AUTONOMO)
        if (strcmp(_version, "v0.1") == 0) _batteryPin = 48;
        else _batteryPin = 61;
    #else
        _batteryPin = -1;
    #endif
}


String ProcessorStats::getSensorLocation(void) {return BOARD;}


#if defined(ARDUINO_ARCH_SAMD)
    extern "C" char *sbrk(int i);

    int FreeRam () {
      char stack_dummy = 0;
      return &stack_dummy - sbrk(0);
    }
#endif


bool ProcessorStats::addSingleMeasurementResult(void)
{
    // Get the battery voltage
    MS_DBG(F("Getting battery voltage\n"));

    float sensorValue_battery = -9999;

    #if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
        if (strcmp(_version, "v0.3") == 0 or strcmp(_version, "v0.4") == 0)
        {
            // Get the battery voltage
            float rawBattery = analogRead(_batteryPin);
            sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
        }
        if (strcmp(_version, "v0.5") == 0 or strcmp(_version, "v0.5b") == 0)
        {
            // Get the battery voltage
            float rawBattery = analogRead(_batteryPin);
            sensorValue_battery = (3.3 / 1023.) * 4.7 * rawBattery;
        }

    #elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
        float measuredvbat = analogRead(_batteryPin);
        measuredvbat *= 2;    // we divided by 2, so multiply back
        measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
        measuredvbat /= 1024; // convert to voltage
        sensorValue_battery = measuredvbat;

    #elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
        if (strcmp(_version, "v0.1") == 0)
        {
            // Get the battery voltage
            float rawBattery = analogRead(_batteryPin);
            sensorValue_battery = (3.3 / 1023.) * 2 * rawBattery;
        }
        if (strcmp(_version, "v0.2") == 0)
        {
            // Get the battery voltage
            float rawBattery = analogRead(_batteryPin);
            sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
        }

    #elif defined(ARDUINO_AVR_SODAQ_NDOGO) || defined(ARDUINO_SODAQ_AUTONOMO) || defined(ARDUINO_AVR_SODAQ_MBILI)
        // Get the battery voltage
        float rawBattery = analogRead(_batteryPin);
        sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;

    #else
        sensorValue_battery = -9999;

    #endif

    verifyAndAddMeasurementResult(PROCESSOR_BATTERY_VAR_NUM, sensorValue_battery);

    // Used only for debugging - can be removed
    MS_DBG(F("Getting Free RAM\n"));

    #if defined __AVR__
    extern int __heap_start, *__brkval;
    int v;
    float sensorValue_freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

    #elif defined(ARDUINO_ARCH_SAMD)
    float sensorValue_freeRam = FreeRam();

    #else
    float sensorValue_freeRam = -9999;
    #endif

    verifyAndAddMeasurementResult(PROCESSOR_RAM_VAR_NUM, sensorValue_freeRam);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Make sure the status bits for measurement request (bit 5) and measurement
    // completion (bit 6) are no longer set
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
