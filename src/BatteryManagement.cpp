/**
 * @file BatteryManagement.cpp
 * @copyright 2021 Neil Hancock Assigned Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library 
 * @author Neil Hancock
 *
 * @brief Implements the BatteryManagement class.
 */

#include "BatteryManagement.h"

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
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define BOARD "Feather M4 Express"
#elif defined(WIO_TERMINAL)
#define BOARD "WIO_TERMINAL"
#elif defined(adafruit_pygamer_advance_m4)
#define BOARD "adafruit_pygamer_advance_m4"

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
BatteryManagement::BatteryManagement(const char *version)
    //: Sensor(BOARD, PROCESSOR_NUM_VARIABLES, PROCESSOR_WARM_UP_TIME_MS,              PROCESSOR_STABILIZATION_TIME_MS, PROCESSOR_MEASUREMENT_TIME_MS, -1,             -1, 1) 
             {
    _version = version;
    //sampNum  = 0;

    _liion_type = PSLR_0500mA;

#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) || defined(ARDUINO_AVR_SODAQ_MBILI)
    _batteryPin = A6;
#elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || \
    defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
    _batteryPin        = 9;
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS) || defined(WIO_TERMINAL) || \
    defined(adafruit_pygamer_advance_m4)
#warning need to check WIO TERMINAL
    _batteryPin = A6;  // 20;  //Dedicated PB01 V_DIV
#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA) || \
    defined(ARDUINO_AVR_SODAQ_NDOGO)
    _batteryPin = 10;
#elif defined(ARDUINO_SODAQ_AUTONOMO)
    if (strcmp(_version, "v0.1") == 0)
        _batteryPin = 48;
    else
        _batteryPin = 33;
#else
#warning No board defined
    _batteryPin = -1;
#endif
}
// Destructor
BatteryManagement::~BatteryManagement() {}


void BatteryManagement::setBatteryV(float newReading) {
    _batteryExt_V = newReading;
    _usebatExt    = true;
}
void BatteryManagement::setBatteryType(ps_liion_rating_t LiionType) {
    _liion_type = LiionType;
}
void BatteryManagement::printBatteryThresholds() {
    Serial.print(F("Battery Type="));
    Serial.println(_liion_type);
    Serial.print(F(" Thresholds USEABLE="));
    Serial.print(PS_LBATT_USEABLE_V);
    Serial.print(F("V LOW="));
    Serial.print(PS_LBATT_LOW_V);
    Serial.print(F("V MEDIUM="));
    Serial.print(PS_LBATT_MEDIUM_V);
    Serial.print(F("V GOOD="));
    Serial.print(PS_LBATT_HEAVY_V);
    Serial.println(F("V"));
}
ps_Lbatt_status_t
BatteryManagement::isBatteryStatusAbove(bool         newBattReading,
                                     ps_pwr_req_t status_req) {
    ps_Lbatt_status_t lion_status;
    ps_Lbatt_status_t retValue;
#if defined BatteryManagement_DBG || defined DEBUGGING_SERIAL_OUTPUT
    __attribute__((unused)) float threshold_V;
#define threshold_store(p1) threshold_V = p1
#else
#define threshold_store(p1)
#endif
    if (_usebatExt) {
        LiIonBatt_V = _batteryExt_V;
    } else if (newBattReading) {
        getBatteryVm1(&LiIonBatt_V);
    }

    // determine expected status from thresholds
    if (LiIonBatt_V < 0) {
        //Sanity Check - if less than zero, allow any action
        lion_status = PS_LBATT_HEAVY_STATUS;
    } else if (LiIonBatt_V >= PS_LBATT_HEAVY_V) {
        lion_status = PS_LBATT_HEAVY_STATUS;
    } else if (LiIonBatt_V >= PS_LBATT_MEDIUM_V) {
        lion_status = PS_LBATT_MEDIUM_STATUS;
    } else if (LiIonBatt_V >= PS_LBATT_LOW_V) {
        lion_status = PS_LBATT_LOW_STATUS;
    } else if (LiIonBatt_V >= PS_LBATT_USEABLE_V) {
        lion_status = PS_LBATT_BARELYUSEABLE_STATUS;
    } else {
        lion_status = PS_LBATT_UNUSEABLE_STATUS;
    }
    retValue = lion_status;

    // Check if meets the requested threshold/status
    switch (status_req) {
        case PS_PWR_LOW_REQ:
            if (PS_LBATT_LOW_STATUS > lion_status) {
                retValue = PS_LBATT_UNUSEABLE_STATUS;
            }
            threshold_store(PS_LBATT_LOW_V);
            break;
        case PS_PWR_MEDIUM_REQ:
            if (PS_LBATT_MEDIUM_STATUS > lion_status) {
                retValue = PS_LBATT_UNUSEABLE_STATUS;
            }
            threshold_store(PS_LBATT_MEDIUM_V);
            break;
        case PS_PWR_HEAVY_REQ:
            if (PS_LBATT_HEAVY_STATUS > lion_status) {
                retValue = PS_LBATT_UNUSEABLE_STATUS;
            }
            threshold_store(PS_LBATT_HEAVY_V);
            break;
        // PS_LBATT_REQUEST_STATUS: //implicit
        // PS_PWR_USEABLE_REQ: if (PS_PWR_BARELYUSEABLE_STATUS>=lion_status)
        // retValue=PS_PWR_FAILED_TEST_STATUS; break; Implicit
        default: threshold_store(PS_LBATT_USEABLE_V); break;
    }

    if (newBattReading) {
        MS_DBG(F(" isBatteryStatusAbove rsp:"), retValue, F(" (Vnew="),
               LiIonBatt_V, F("V) req:"), status_req, F(" (above "),
               threshold_V, F("V)"));
    } else {
        MS_DBG(F(" isBatteryStatusAbove rsp:"), retValue, F(" (Vold="),
               LiIonBatt_V, F("V) req:"), status_req, F(" (above "),
               threshold_V, F("V)"));
    }
    return retValue;
}
float BatteryManagement::getBatteryVm1(bool newBattReading)  // sensorValue_battery
{
    if (newBattReading) {
        return getBatteryVm1(&LiIonBatt_V);
    } else {
        return LiIonBatt_V;
    }
}
float BatteryManagement::getBatteryVm1(
    float* sensorValue_battery)  // sensorValue_battery
{
// float batteryV;// = (float) -999.0;
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
    uint16_t rawBattery_adc = 0;
    uint8_t  adcLp;
    if (_usebatExt) {  // defined MS_VBAT_EXT
        MS_DBG(F("Get battery voltage ext"));
        *sensorValue_battery = _batteryExt_V;
    } else {
#define SAMPLE_BATTERY_PIN_NUM 4
        for (adcLp = 0; adcLp < SAMPLE_BATTERY_PIN_NUM; adcLp++) {
            rawBattery_adc += analogRead(_batteryPin);
        }

        if (strcmp(_version, "v0.5ba") == 0) {
// For series 1M+270K mult raw_adc by ((3.3 / 1023) * 4.7037)
#define CONST_VBATT_0_5BA 0.0151732
            *sensorValue_battery = CONST_VBATT_0_5BA *
                ((float)(rawBattery_adc / SAMPLE_BATTERY_PIN_NUM));
        } else if (strcmp(_version, "v0.5") == 0 or
                   strcmp(_version, "v0.5b") == 0) {
            // Get the battery voltage for series 10M+2.7M
            *sensorValue_battery = (3.3 / 1023.) * 4.7 *
                ((float)(rawBattery_adc / SAMPLE_BATTERY_PIN_NUM));
        } else if (strcmp(_version, "v0.3") == 0 or
                   strcmp(_version, "v0.4") == 0) {
            // Get the battery voltage
            *sensorValue_battery = (3.3 / 1023.) * 1.47 *
                ((float)(rawBattery_adc / SAMPLE_BATTERY_PIN_NUM));
        } else {
            MS_DBG(F("Unknown _version "), _version);
        }
    }

#elif defined(ARDUINO_AVR_FEATHER32U4) || defined(ARDUINO_SAMD_FEATHER_M0) || \
    defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) ||                               \
    defined(ADAFRUIT_FEATHER_M4_EXPRESS)
    float measuredvbat        = analogRead(_batteryPin);
    measuredvbat *= 2;     // we divided by 2, so multiply back
    measuredvbat *= 3.3;   // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024;  // convert to voltage
    *sensorValue_battery = measuredvbat;

#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
    if (strcmp(_version, "v0.1") == 0) {
        // Get the battery voltage
        float rawBattery     = analogRead(_batteryPin);
        *sensorValue_battery = (3.3 / 1023.) * 2 * rawBattery;
    }
    if (strcmp(_version, "v0.2") == 0) {
        // Get the battery voltage
        float rawBattery     = analogRead(_batteryPin);
        *sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
    }

#elif defined(ARDUINO_AVR_SODAQ_NDOGO) || defined(ARDUINO_SODAQ_AUTONOMO) || \
    defined(ARDUINO_AVR_SODAQ_MBILI)
    // Get the battery voltage
    float rawBattery     = analogRead(_batteryPin);
    *sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;

#else
    *sensorValue_battery = -9999;
    MS_DBG(F("Unknown _version "), _version);

#endif
    return *sensorValue_battery;
}
