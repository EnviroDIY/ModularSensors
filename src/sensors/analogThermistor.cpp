/*
 *analogThermistor.cpp
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

#include "analogThermistor.h"

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
analogThermistor::analogThermistor(int8_t powerPin, int8_t adcPin, uint8_t measurementsToAverage)
    : Sensor(BOARD, ANALOGTHERMISTOR_NUM_VARIABLES,
             ANALOGTHERMISTOR_WARM_UP_TIME_MS, ANALOGTHERMISTOR_STABILIZATION_TIME_MS, ANALOGTHERMISTOR_MEASUREMENT_TIME_MS,
             powerPin, adcPin, measurementsToAverage)
{
    //_version = version;
    //_TemperaturePowerPin= -1;
    _TemperatureAdcPin = adcPin, 
    _thermistorType = APTT_NCP15XH193F03RC;
    _thermistorSieresResistance_ohms = AP_THERMISTOR_SERIES_R_OHMS ;    

    _ptrWaterTemperature_C =NULL;

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
analogThermistor::~analogThermistor(){}


String analogThermistor::getSensorLocation(void) {return BOARD;}


#if defined(ARDUINO_ARCH_SAMD)
    /*extern "C" char *sbrk(int i);

    int16_t FreeRam () {
      char stack_dummy = 0;
      return &stack_dummy - sbrk(0);
    }*/
#endif

void analogThermistor::setTemperature_k( uint8_t thermistorType,float  sieresResistance_ohms){
   // _TemperaturePowerPin = powerPin;
    //_TemperatureAdcPin = adcPin, 
    _thermistorType = thermistorType;
    _thermistorSieresResistance_ohms = sieresResistance_ohms ;
}

bool analogThermistor::addSingleMeasurementResult(void){
    //MS_DBG(F("Getting Temperature"));

    // Calculate R relative to R1 - 75000  ~ ThermistorR is series with R= 75K, and same Vcc
    // Vref could be Vcc/2   - Vthermistor+Vr = Vcc
#warning - should the followinb be used bitRead(_sensorStatus, 6)
    //Rthermistor = Vth/Vref*75000  - where for raw ADC  ADCth/(ADCrange-ADCth)*75000 (ohms)  
    //T(c)+273.15		1 / (0.0008746904041902967 + 0.0002532755006290475 * logR + 1.877479431169023e-7 * logR * logR * logR)		
    #define THERMISTOR_ADC_RANGE 1024
    #define THERMISTOR_SERIES_R_OHMS 75000
    if (APTT_NUM < _thermistorType) return false;
    
    uint32_t adcThermistor = analogRead(_TemperatureAdcPin);
    uint32_t adcSeriesR = THERMISTOR_ADC_RANGE - adcThermistor;
    float Rthermistor = ((float)adcThermistor/(float)adcSeriesR)* THERMISTOR_SERIES_R_OHMS ;
    float lnResistor =log(Rthermistor );
    float lnResCube = lnResistor*lnResistor*lnResistor;
        //T(c)+273.15		1 / (0.0008746904041902967 + 0.0002532755006290475 * logR + 1.877479431169023e-7 * logR * logR * logR)		
    float sensorTemperature_C =( 1/(0.0008746904041902967 + 0.0002532755006290475 * lnResistor + 1.877479431169023e-7 * lnResCube)	) - APTT_KELVIN_OFFSET;  
    MS_DBG(F("Temperature ="),sensorTemperature_C);

    verifyAndAddMeasurementResult(ANALOGTHERMISTOR_TEMPERATURE_VAR_NUM, sensorTemperature_C);


    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}





