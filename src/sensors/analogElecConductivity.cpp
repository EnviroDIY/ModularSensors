/*
 *analogElecConductivity.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * see .h
*/

#include "analogElecConductivity.h"

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
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
    #define BOARD "Feather M4 Express"
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
   #error BOARD Unknown
#endif


// For Mayfly version because the battery resistor depends on it
analogElecConductivity::analogElecConductivity(const char *version)
    : Sensor(BOARD, ANALOGELECCONDUCTIVITY_NUM_VARIABLES,
             ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS, ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS, ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS,
             -1, -1, 1)
{
    _version = version;
    _EcPowerPin = -1;
    _EcAdcPin = -1;
    _ptrWaterTemperature_C = NULL;
   /* #if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) || defined(ARDUINO_AVR_SODAQ_MBILI)
        _EcAdcPin = A6;
    #elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
        _EcAdcPin = 9;
    #elif defined(ADAFRUIT_FEATHER_M4_EXPRESS) 
        _EcAdcPin = A6;//20;  //Dedicated PB01 V_DIV
    #elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA) || defined(ARDUINO_AVR_SODAQ_NDOGO)
        _EcAdcPin = 10;
    #elif defined(ARDUINO_SODAQ_AUTONOMO)
        if (strcmp(_version, "v0.1") == 0) _EcAdcPin = 48;
        else _EcAdcPin = 33;
    #else
    #error No board defined 
        _EcAdcPin = -1;
    #endif */
}
// Destructor
analogElecConductivity::~analogElecConductivity(){}


String analogElecConductivity::getSensorLocation(void) {return BOARD;}

bool analogElecConductivity::addSingleMeasurementResult(void)
{


        MS_DBG(F("Getting EC of Water"));

        float sensorValue_EC = SENSOR_UNINIT_VAL;
        #if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)

            // Just the voltage
            float rawBattery = analogRead(_EcAdcPin);
            sensorValue_EC = (3.3 / 1023.) * 4.7 * rawBattery;


        #elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
            #if defined analogElecConductivityDef_Resolution
            analogReadResolution(analogElecConductivityDef_Resolution);
            #endif

            uint32_t ElecCond_adc = analogRead(_EcAdcPin);
            sensorValue_EC = (3.3 / 1023.) * 4.7 * ElecCond_adc;
            sensorValue_EC = ElecCond_adc;


        #endif

        verifyAndAddMeasurementResult(ANALOGELECCONDUCTIVITY_EC_VAR_NUM, sensorValue_EC);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}

//void analogElecConductivity::setWaterTemperature(float  WaterTemperature_C) {
//    _WaterTemperature_C = WaterTemperature_C;
//}
void analogElecConductivity::setWaterTemperature(float  *ptrWaterTemperature_C) {
    _ptrWaterTemperature_C = ptrWaterTemperature_C;
}




