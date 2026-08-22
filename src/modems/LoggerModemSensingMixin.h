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
    /// @copydoc loggerModemImpl::GsmModemType
    using GsmModemType = GsmModemType_T;

 protected:
    /// @copydoc loggerModemPowerMixin::derived()
    inline Derived& derived() {
        return static_cast<Derived&>(*this);
    }
    /// @copydoc loggerModemPowerMixin::derived() const
    inline const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }

    /* ===================================================================== */
    /* Modem Metadata                                                        */
    /* ===================================================================== */
    /**
     * @anchor modem_metadata_functions_impl
     * @name Modem metadata function implementations
     * Implementations of functions to get metadata about modem functionality -
     * using the modem like a sensor.
     */
    /**@{*/

    /* ===================================================================== */
    /* Signal Quality                                                        */
    /* ===================================================================== */
 protected:
    /// @copydoc loggerModem::getModemSignalQuality()
    virtual bool getModemSignalQualityImpl(int16_t& rssi, int16_t& percent) {
        MS_DBG(F("Getting signal quality:"));
        int16_t signalQual = derived().gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);
        derived().convertSignalQuality(
            signalQual, rssi, percent,
            typename Derived::SignalQualityIsRSSI::type());
        return true;
    }

    /**
     * @anchor modem_signal_functions
     * @name Functions to convert between signal strength measurement types
     */
    /**@{*/
 protected:
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
 protected:
    /// @copydoc loggerModem::getModemBatteryStats()
    virtual bool getModemBatteryStatsImpl(int8_t& chargeState, int8_t& percent,
                                          int16_t& milliVolts) {
        return getModemBatteryStatsImpl(
            chargeState, percent, milliVolts,
            typename TinyGsmCapabilities::has_battery<GsmModemType>::type());
    }

    /**
     * @brief The get battery stats function for modems that are capable of
     * returning battery information.
     * @copydetails loggerModem::getModemBatteryStats(int8_t&, int8_t&,
     * int16_t&)
     */
    bool getModemBatteryStatsImpl(int8_t& chargeState, int8_t& percent,
                                  int16_t& milliVolts,
                                  TinyGsmCapabilities::true_type) {
        MS_DBG(F("Getting modem battery data:"));
        return derived().gsmModem.getBattStats(chargeState, percent,
                                               milliVolts);
    }

    /**
     * @brief The get battery stats function for modems that cannot return
     * battery information.
     * @copydetails loggerModem::getModemBatteryStats(int8_t&, int8_t&,
     * int16_t&)
     */
    bool getModemBatteryStatsImpl(int8_t& chargeState, int8_t& percent,
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
 protected:
    /// @copydoc loggerModem::getModemChipTemperature()
    virtual float getModemChipTemperatureImpl() {
        return getModemChipTemperatureImpl(
            typename TinyGsmCapabilities::has_temperature<
                GsmModemType>::type());
    }

    /**
     * @brief The modem temperature function for modems that are able to return
     * temperature
     * @copydetails loggerModem::getModemChipTemperature()
     */
    float getModemChipTemperatureImpl(TinyGsmCapabilities::true_type) {
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
    float getModemChipTemperatureImpl(TinyGsmCapabilities::false_type) {
        MS_DBG(F("This modem doesn't return temperature!"));
        return static_cast<float>(MS_INVALID_VALUE);
    }
};

// cSpell:ignore bpercent

#endif  // SRC_LOGGERMODEMSENSINGMIXIN_H_
