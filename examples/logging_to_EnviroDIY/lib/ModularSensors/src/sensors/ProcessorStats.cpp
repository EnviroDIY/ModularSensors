/*
 *ProcessorStats.cpp
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
    : Sensor(BOARD, PROCESSOR_NUM_VARIABLES,
             PROCESSOR_WARM_UP_TIME_MS, PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS,
             -1, -1, 1)
{
    _version = version;
    sampNum = 0;

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
// Destructor
ProcessorStats::~ProcessorStats(){}


String ProcessorStats::getSensorLocation(void) {return BOARD;}


#if defined(ARDUINO_ARCH_SAMD)
    extern "C" char *sbrk(int i);

    int16_t FreeRam () {
      char stack_dummy = 0;
      return &stack_dummy - sbrk(0);
    }
#endif
//float *ProcessorStats::getBatteryVm1(float *batteryV) //sensorValue_battery

ps_Lbatt_status_t ProcessorStats::isBatteryStatusAbove(bool newBattReading, ps_pwr_req_t status_req) 
{
    ps_Lbatt_status_t lion_status;
    ps_Lbatt_status_t retValue;

    if (newBattReading) {
        getBatteryVm1(&LiIonBatt_V);
        MS_DBG(F(" isBatteryStatusAbove Vnew="),LiIonBatt_V);
    } else {
        MS_DBG(F(" isBatteryStatusAbove Vold="),LiIonBatt_V);
    }
    if      (LiIonBatt_V>=PS_LBATT_GOOD_V)   { lion_status=PS_LBATT_HEAVY_STATUS;
    }else if(LiIonBatt_V>=PS_LBATT_MEDIUM_V) { lion_status=PS_LBATT_MEDIUM_STATUS;
    }else if(LiIonBatt_V>=PS_LBATT_LOW_V)    { lion_status=PS_LBATT_LOW_STATUS;
    }else if(LiIonBatt_V>=PS_LBATT_USEABLE_V){ lion_status=PS_LBATT_BARELYUSEABLE_STATUS;
    }else                                    { lion_status=PS_LBATT_UNUSEABLE_STATUS;
    }
    retValue=lion_status;
    switch (status_req){
        case PS_PWR_LOW_REQ:    if (PS_LBATT_LOW_STATUS>lion_status)    {retValue=PS_LBATT_UNUSEABLE_STATUS;} break;
        case PS_PWR_MEDIUM_REQ: if (PS_LBATT_MEDIUM_STATUS>lion_status) {retValue=PS_LBATT_UNUSEABLE_STATUS;} break;
        case PS_PWR_HEAVY_REQ:  if (PS_LBATT_HEAVY_STATUS>lion_status)  {retValue=PS_LBATT_UNUSEABLE_STATUS;} break;
        //PS_LBATT_REQUEST_STATUS: //implicit
        //PS_PWR_USEABLE_REQ: if (PS_PWR_BARELYUSEABLE_STATUS>=lion_status) retValue=PS_PWR_FAILED_TEST_STATUS; break; Implicit
        default: 
           break; 
    }

    #if 0
    Serial.print(F("Req:"));
    Serial.print(status_req);
    Serial.print(F(" Status:"));
    Serial.print(lion_status);
    Serial.print(F(" retStatus:"));
    Serial.println(retValue);
    #endif
    MS_DBG(F(" isBatteryStatusAbove="),retValue,F(" req="),status_req);
    return retValue;
}
float ProcessorStats::getBatteryVm1(bool newBattReading) //sensorValue_battery
{ 
    if (newBattReading) {
        return getBatteryVm1(&LiIonBatt_V);
    } else {
        return LiIonBatt_V;
    }
}
float ProcessorStats::getBatteryVm1(float *BattV) //sensorValue_battery
{
    //float batteryV;// = (float) -999.0;
    #if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
        uint16_t rawBattery_adc=0;
        uint8_t adcLp;

        #define SAMPLE_BATTERY_PIN_NUM 4
        for (adcLp=0;adcLp<SAMPLE_BATTERY_PIN_NUM; adcLp++) 
        {
            rawBattery_adc += analogRead(_batteryPin);
        }

        if (strcmp(_version, "v0.5ba") == 0)
        {
            //For series 1M+270K mult raw_adc by ((3.3 / 1023) * 4.7037) 
            #define CONST_VBATT_0_5BA 0.0151732
            *BattV = CONST_VBATT_0_5BA * ((float)(rawBattery_adc/SAMPLE_BATTERY_PIN_NUM) );
        } else 
        if (strcmp(_version, "v0.5") == 0 or strcmp(_version, "v0.5b") == 0)
        {
            // Get the battery voltage for series 10M+2.7M
            *BattV = (3.3 / 1023.) * 4.7 * ((float)(rawBattery_adc/SAMPLE_BATTERY_PIN_NUM) );
        } else
        if (strcmp(_version, "v0.3") == 0 or strcmp(_version, "v0.4") == 0)
        {
            // Get the battery voltage
            *BattV = (3.3 / 1023.) * 1.47 * ((float)(rawBattery_adc/SAMPLE_BATTERY_PIN_NUM) );
        } else {
            MS_DBG(F("Unknown _version "), _version);
        }

    #elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
        float measuredvbat = analogRead(_batteryPin);
        measuredvbat *= 2;    // we divided by 2, so multiply back
        measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
        measuredvbat /= 1024; // convert to voltage
        *BattV = measuredvbat;

    #elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
        if (strcmp(_version, "v0.1") == 0)
        {
            // Get the battery voltage
            float rawBattery = analogRead(_batteryPin);
            *BattV = (3.3 / 1023.) * 2 * rawBattery;
        }
        if (strcmp(_version, "v0.2") == 0)
        {
            // Get the battery voltage
            float rawBattery = analogRead(_batteryPin);
            *BattV = (3.3 / 1023.) * 1.47 * rawBattery;
        }

    #elif defined(ARDUINO_AVR_SODAQ_NDOGO) || defined(ARDUINO_SODAQ_AUTONOMO) || defined(ARDUINO_AVR_SODAQ_MBILI)
        // Get the battery voltage
        float rawBattery = analogRead(_batteryPin);
        *BattV = (3.3 / 1023.) * 1.47 * rawBattery;

    #else
        *BattV = -9999;

    #endif    
    return *BattV;
}

bool ProcessorStats::addSingleMeasurementResult(void)
{
    // Get the battery voltage
    //MS_DBG(F("Getting battery voltage"));
    // assume early getBatteryVm1(&LiIonBatt_V);

    verifyAndAddMeasurementResult(PROCESSOR_BATTERY_VAR_NUM, LiIonBatt_V);
    //verifyAndAddMeasurementResult(PROCESSOR_BATTERY_VAR_NUM, sensorValue_battery);

    // Used only for debugging - can be removed
    MS_DBG(F("Getting Free RAM"));

    #if defined __AVR__
    extern int16_t __heap_start, *__brkval;
    int16_t v;
    float sensorValue_freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

    #elif defined(ARDUINO_ARCH_SAMD)
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
