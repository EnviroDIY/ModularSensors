/*
 *processorAdc.cpp
 *This file is part of the EnviroDIY modular sensors library
 *
 * Written By:  Neil Hanccok
 * Adapted from ExternalVoltage Bobby Schulz <schu3119@umn.edu>
 *
 
*/


#include "processorAdc.h"
#include "ms_cfg.h"
//#include <Adafruit_ADS1015.h>
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
    #define BOARD "EnviroDIY Mayfly"

// Sodaq boards
//#elif defined(ARDUINO_SODAQ_EXPLORER)
//    #define BOARD "SODAQ ExpLoRer"
#elif defined(ARDUINO_SODAQ_AUTONOMO)
    #define BOARD "SODAQ Autonomo"
/* #elif defined(ARDUINO_SODAQ_ONE_BETA)
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
*/
#elif defined(ARDUINO_SAMD_FEATHER_M0)
    #define BOARD "Feather M0"
#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
    #define BOARD "Feather M0 Express"
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
    #define BOARD "Feather M4 Express"
// Arduino boards
/* #elif defined(ARDUINO_AVR_ADK)
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
*/
#elif defined(ARDUINO_AVR_MEGA2560)
    #define BOARD "Mega 2560"
/*#elif defined(ARDUINO_AVR_MICRO)
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
*/
#elif defined(ARDUINO_SAMD_ZERO)
    #define BOARD "Zero"

#else
   #define BOARD "Unknown"
   #error BOARD Unknown
#endif
// The constructor - need the power pin the data pin, and gain if non standard
processorAdc::processorAdc(int8_t powerPin, uint8_t adcChannel, float gain,
                                 uint8_t measurementsToAverage)
    : Sensor("processorAdc", PROC_ADC_NUM_VARIABLES,
             PROC_ADC_WARM_UP_TIME_MS, PROC_ADC_STABILIZATION_TIME_MS, PROC_ADC_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage)
{
    _adcChannel = adcChannel;
    _gain = gain;
}
// Destructor
processorAdc::~processorAdc(){}


String processorAdc::getSensorLocation(void)
{
    String sensorLocation = F("procAdc");
    sensorLocation += String(_adcChannel);
    return sensorLocation;
}


bool processorAdc::addSingleMeasurementResult(void)
{
    // Variables to store the results in
    float adcVoltage = -9999;
    float calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        MS_DBG(getSensorNameAndLocation(), F(" is reporting:"));
        #if !defined ARDUINO_ARCH_AVR
        //Only for processors where ADC Resolution can be varied: not AVR
        analogReadResolution(ProcAdcDef_Resolution);
        #endif //ARDUINO_ARCH_AVR
        uint8_t useAdcChannel = _adcChannel; 
        #if defined ARD_ANALOLG_EXTENSION_PINS
        if ((thisVariantNumPins+ARD_DIGITAL_EXTENSION_PINS)< _adcChannel) {
            
            //Setup mutliplexer
            //digitalWrite(_adcChannel,1);
            //useAdcChannel = ARD_ANLAOG__MULTIPLEX_PIN;
            MS_DBG("  adc_Single Setup Multiplexer not supported ", _adcChannel,"-->",ARD_ANLAOG__MULTIPLEX_PIN); 

        } 

        #endif //ARD_ANALOLG_EXTENSION_PINS
        // Create an Auxillary ADD object
        // We create and set up the ADC object here so that each sensor using
        // the ADC may set the gain appropriately without effecting others.
        uint32_t  rawAdc = analogRead(useAdcChannel);
        adcVoltage = (3.3 /ProcAdc_Max) * (float) rawAdc;
        MS_DBG(F("  adc_SingleEnded_V("), _adcChannel, F("/"),ProcAdcDef_Resolution,F("):"), adcVoltage,F(" rawAdc:"),rawAdc, 
               F(" gain="),_gain);

        if (adcVoltage < 3.6 and adcVoltage > -0.3)  // Skip results out of range
        {
            // Apply the gain calculation, with a defualt gain of 10 V/V Gain
            calibResult = adcVoltage * _gain ;
            MS_DBG(F("  calibResult:"), calibResult);
        }
        else  // set invalid voltages back to -9999
        {
            adcVoltage = -9999;
        }
    }
    else MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));

    verifyAndAddMeasurementResult(PROC_ADC_VAR_NUM, calibResult);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if (adcVoltage < 3.6 and adcVoltage > -0.3) return true;
    else return false;
}
