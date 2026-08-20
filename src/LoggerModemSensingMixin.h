/**
 * @file LoggerModemSensingMixin.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief CRTP mixin providing metadata polling functionality for modems.
 *
 * This mixin contains all functions for querying and storing modem metadata
 * such as signal quality, battery status, and chip temperature.
 */

// Header Guards
#ifndef SRC_LOGGERMODEMSENSINGMIXIN_H_
#define SRC_LOGGERMODEMSENSINGMIXIN_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEMSENSING_DEBUG
#define MS_DEBUGGING_STD "LoggerModemSensing"
#endif
#ifdef MS_LOGGERMODEMSENSING_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModemSensing"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

#include <Arduino.h>
#include <TinyGsmCapabilities.h>

/**
 * @brief CRTP mixin class providing metadata sensing for modems.
 *
 * This class implements the Curiously Recurring Template Pattern (CRTP) to
 * provide modem metadata polling functionality including signal quality,
 * battery stats, and chip temperature monitoring.
 *
 * @tparam Derived The derived class type that uses this mixin
 */
template <typename Derived, typename GsmModemType_T>
class loggerModemSensingMixin {
 public:
    // Type aliases to avoid incomplete type issues
    using GsmModemType = GsmModemType_T;

 protected:
    inline Derived& derived() {
        return static_cast<Derived&>(*this);
    }
    inline const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }

    /* ===================================================================== */
    /* Modem Metadata                                                        */
    /* ===================================================================== */
    /**
     * @anchor modem_metadata_functions
     * @name Modem metadata functions
     * Functions to get metadata about modem functionality - using the modem
     * like a sensor.
     *
     * These functions will query the modem to get new values.
     *
     * @note In order to use the modem metadata functions, they must be
     * called after the modem is connected to the internet.
     */
    /**@{*/

    /* ===================================================================== */
    /* Signal Quality                                                        */
    /* ===================================================================== */
 public:
    /**
     * @brief Get the signal quality from the modem
     *
     * @remark We can't distinguish between a bad modem response, no modem
     * response, and a real response from the modem of no service/signal.
     * The TinyGSM getSignalQuality function returns the same "no signal" value
     * (99 CSQ or 0 RSSI) in all 3 cases.
     *
     * @param rssi The reference to an int16_t which will be set with the
     * received signal strength indicator.
     * @param percent The reference to an int16_t which will be set with the
     * "percent" signal strength.
     * @return True if the communication with the modem was successful and the
     * values referenced by the pointers should be valid; false otherwise.
     */
    virtual bool getModemSignalQuality(int16_t& rssi, int16_t& percent) {
        MS_DBG(F("Getting signal quality:"));
        int16_t signalQual = derived().gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);
        derived().convertSignalQuality(
            signalQual, rssi, percent,
            typename Derived::SignalQualityIsRSSI::type());
        return true;
    }

 protected:
    /**
     * @anchor modem_signal_functions
     * @name Functions to convert between signal strength measurement types
     */
    /**@{*/
    /**
     * @brief The convert signal quality function for modems that return RSSI
     * directly - those with "signalQualityIsRSSI" set to a true type.
     *
     * @param signalQual The raw signal quality value from the modem.
     * @param rssi The reference to an int16_t which will be set with the
     * received signal strength indicator.
     * @param percent The reference to an int16_t which will be set with the
     * "percent" signal strength.
     */
    void convertSignalQuality(int16_t signalQual, int16_t& rssi,
                              int16_t& percent,
                              TinyGsmCapabilities::true_type) {
        rssi = signalQual;
        MS_DBG(F("Raw signal is already in units of RSSI:"), rssi);
        percent = getPctFromRSSI(signalQual);
        MS_DBG(F("Signal percent calculated from RSSI:"), percent);
    }

    /**
     * @brief The convert signal quality function for modems that return CSQ
     * instead of RSSI - those with "signalQualityIsRSSI" set to a false type.
     *
     * @param signalQual The raw signal quality value from the modem.
     * @param rssi The reference to an int16_t which will be set with the
     * received signal strength indicator.
     * @param percent The reference to an int16_t which will be set with the
     * "percent" signal strength.
     */
    void convertSignalQuality(int16_t signalQual, int16_t& rssi,
                              int16_t& percent,
                              TinyGsmCapabilities::false_type) {
        rssi = getRSSIFromCSQ(signalQual);
        MS_DBG(F("RSSI Estimated from CSQ:"), rssi);
        percent = getPctFromCSQ(signalQual);
        MS_DBG(F("Signal percent calculated from CSQ:"), percent);
    }

    /**
     * @brief Get approximate RSSI from CSQ (assuming no noise).
     *
     * @param csq A "CSQ" (0-31) signal quality
     * @return An RSSI in dBm, making assumptions about the conversion
     */
    static int16_t getRSSIFromCSQ(int16_t csq) {
        if ((csq < 0) || (csq > 31)) return 0;
        // equation matches previous table. not sure the original motivation.
        return ((csq * 2) - 113);
    }

    /**
     * @brief Get signal percent from CSQ.
     *
     * @param csq A "CSQ" (0-31) signal quality
     * @return The percent of maximum signal strength.
     */
    static int16_t getPctFromCSQ(int16_t csq) {
        if ((csq < 0) || (csq > 31)) return 0;
        // equation matches previous table. not sure the original motivation.
        return (csq * 827 + 127) >> 8;
    }

    /**
     * @brief Get signal percent from RSSI.
     *
     * @param rssi The RSSI in dBm.
     * @return The estimated percent of maximum signal strength.
     */
    static int16_t getPctFromRSSI(int16_t rssi) {
        auto pct =
            static_cast<int16_t>(1.6163 * static_cast<float>(rssi) + 182.61);
        if (rssi == 0) pct = 0;
        if (rssi == (255 - 93))
            pct = 0;  // This is a no-data-yet value from XBee
        return pct;
    }
    /**@}*/


    /* ===================================================================== */
    /* Battery Stats - battery voltage, charging state, and charge percent   */
    /* ===================================================================== */
 public:
    /**
     * @brief Get the modem's battery information  - this may or may not be a
     * valid values depending on the module and breakout.
     *
     * This populates the entered references with invalid values for modems
     * where such data is not available.
     *
     * @warning This function does **not** use #MS_INVALID_VALUE for the invalid
     * values! This is because of the size of the int variables and the
     * standards within TinyGSM.
     *
     * @remark The battery values for the modem will only be valid if the modem
     * is directly connected to a battery.  This is __not__ the way most modems
     * are wired.  If the modem is not directly wired to a battery, this will
     * return invalid data.
     *
     * @param chargeState The reference to an int8_t which will be set with the
     * battery charge state - exactly the meaning of this varies by module, but
     * it generally indicates whether the battery is currently charging or
     * discharging.
     * @param percent The reference to an int8_t which will be set with the
     * percent battery charge
     * @param milliVolts The reference to an int16_t which will be set with the
     * battery voltage in millivolts
     * @return True if the communication with the modem was successful and the
     * values referenced by the pointers should be valid; false otherwise.
     */
    virtual bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                                      int16_t& milliVolts) {
        return getModemBatteryStats(
            chargeState, percent, milliVolts,
            typename TinyGsmCapabilities::has_battery<GsmModemType>::type());
    }

 protected:
    /**
     * @brief The get battery stats function for modems that are capable of
     * returning battery information.
     * @copydetail getModemBatteryStats(int8_t&, int8_t&, int16_t&)
     */
    bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                              int16_t& milliVolts,
                              TinyGsmCapabilities::true_type) {
        MS_DBG(F("Getting modem battery data:"));
        return derived().gsmModem.getBattStats(chargeState, percent,
                                               milliVolts);
    }

    /**
     * @brief The get battery stats function for modems that cannot return
     * battery information.
     * @copydetail getModemBatteryStats(int8_t&, int8_t&, int16_t&)
     */
    bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                              int16_t& milliVolts,
                              TinyGsmCapabilities::false_type) {
        MS_DBG(F("This modem doesn't return battery information!"));
        chargeState = 99;
        percent     = -99;
        milliVolts  = 9999;
        return false;
    }


    /* ===================================================================== */
    /* Modem internal chip temperature                                       */
    /* ===================================================================== */
 public:
    /**
     * @brief Get the current temperature provided by the modem module.
     *
     * @remark This is **not** a measurement of the ambient temperature, it only
     * reflects the temperature of the modem chip itself.  This temperature is
     * expected to be above ambient temperature.
     *
     * @return The modem temperature in degrees Celsius
     */
    virtual float getModemChipTemperature() {
        return getModemChipTemperature(
            typename TinyGsmCapabilities::has_temperature<
                GsmModemType>::type());
    }

 protected:
    /**
     * @brief The modem temperature function for modems that are able to return
     * temperature
     * @return The modem temperature in degrees Celsius
     */
    float getModemChipTemperature(TinyGsmCapabilities::true_type) {
        MS_DBG(F("Getting temperature:"));
        float temp = derived().gsmModem.getTemperature();
        MS_DBG(F("Temperature:"), temp);
        return temp;
    }

    /**
     * @brief The modem temperature function for modems that cannot return
     * temperature
     * @return An invalid value
     */
    float getModemChipTemperature(TinyGsmCapabilities::false_type) {
        MS_DBG(F("This modem doesn't return temperature!"));
        return static_cast<float>(MS_INVALID_VALUE);
    }
};

// cSpell:ignore bpercent

#endif  // SRC_LOGGERMODEMSENSINGMIXIN_H_
