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
template <typename Derived>
class loggerModemSensingMixin {
 protected:
    /**
     * @brief Helper function to cast this pointer to derived type.
     * @return Pointer to the derived class instance
     */
    Derived* derived() {
        return static_cast<Derived*>(this);
    }

    /**
     * @brief Const helper function to cast this pointer to derived type.
     * @return Const pointer to the derived class instance
     */
    const Derived* derivedConst() const {
        return static_cast<const Derived*>(this);
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
     * @brief Get the signal quality from the modem and write the values to the
     * supplied non-constant references.
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
        int16_t signalQual = derived()->gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);
        derived()->convertSignalQuality(
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
     * @brief Get the modem's battery stats
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
        return derived()->getModemBatteryStats(
            chargeState, percent, milliVolts,
            typename TinyGsmCapabilities::has_battery<
                typename Derived::GsmModemType>::type());
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
        return derived()->gsmModem.getBattStats(chargeState, percent,
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
     * @return The modem temperature in degrees Celsius
     */
    virtual float getModemChipTemperature() {
        return derived()->getModemChipTemperature(
            typename TinyGsmCapabilities::has_temperature<
                typename Derived::GsmModemType>::type());
    }

 protected:
    /**
     * @brief The modem temperature function for modems that are able to return
     * temperature
     * @return The modem temperature in degrees Celsius
     */
    float getModemChipTemperature(TinyGsmCapabilities::true_type) {
        MS_DBG(F("Getting temperature:"));
        float temp = derived()->gsmModem.getTemperature();
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

    /* ===================================================================== */
    /* Update Modem Metadata                                                 */
    /* ===================================================================== */
 public:
    /**
     * @brief Update all stored modem metadata.
     *
     * This polls the modem for signal quality, battery stats, and chip
     * temperature based on the configured polling bitmask. The results are
     * stored in static member variables for later retrieval by the static
     * getter functions.
     *
     * @return True if all requested metadata was successfully polled; false
     * otherwise
     */
    virtual bool updateModemMetadata() {
        bool success = true;

        // Unset whatever we had previously
        derived()->_priorRSSI           = MS_INVALID_VALUE;
        derived()->_priorSignalPercent  = MS_INVALID_VALUE;
        derived()->_priorBatteryState   = MS_INVALID_VALUE;
        derived()->_priorBatteryPercent = MS_INVALID_VALUE;
        derived()->_priorBatteryVoltage = MS_INVALID_VALUE;
        derived()->_priorModemTemp      = MS_INVALID_VALUE;

        // Initialize variables
        int16_t rssi     = MS_INVALID_VALUE;
        int16_t percent  = MS_INVALID_VALUE;
        int8_t  state    = 99;
        int8_t  bpercent = -99;
        int16_t volt     = 9999;

        MS_DBG(F("Modem polling settings:"),
               String(derived()->_pollModemMetaData, BIN));

        if ((derived()->_pollModemMetaData & MODEM_RSSI_ENABLE_BITMASK) ==
                MODEM_RSSI_ENABLE_BITMASK ||
            (derived()->_pollModemMetaData &
             MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) ==
                MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) {
            // Try for up to 15 seconds to get a valid signal quality
            uint32_t startMillis = millis();
            do {
                success &= derived()->getModemSignalQuality(rssi, percent);
                derived()->_priorRSSI          = rssi;
                derived()->_priorSignalPercent = percent;
                if (rssi != 0 && rssi != MS_INVALID_VALUE) break;
                delay(250);
            } while ((rssi == 0 || rssi == MS_INVALID_VALUE) &&
                     millis() - startMillis < 15000L && success);
            MS_DBG(F("CURRENT RSSI:"), rssi);
            MS_DBG(F("CURRENT Percent signal strength:"), percent);
        } else {
            MS_DBG(F("Polling for both RSSI and signal strength is disabled"));
        }

        if ((derived()->_pollModemMetaData &
             MODEM_BATTERY_STATE_ENABLE_BITMASK) ==
                MODEM_BATTERY_STATE_ENABLE_BITMASK ||
            (derived()->_pollModemMetaData &
             MODEM_BATTERY_PERCENT_ENABLE_BITMASK) ==
                MODEM_BATTERY_PERCENT_ENABLE_BITMASK ||
            (derived()->_pollModemMetaData &
             MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) ==
                MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) {
            success &= derived()->getModemBatteryStats(state, bpercent, volt);
            MS_DBG(F("CURRENT Modem Battery Charge State:"), state);
            MS_DBG(F("CURRENT Modem Battery Charge Percentage:"), bpercent);
            MS_DBG(F("CURRENT Modem Battery Voltage:"), volt);
            if (state != 99)
                derived()->_priorBatteryState = static_cast<float>(state);
            else
                derived()->_priorBatteryState =
                    static_cast<float>(MS_INVALID_VALUE);

            if (bpercent != -99)
                derived()->_priorBatteryPercent = static_cast<float>(bpercent);
            else
                derived()->_priorBatteryPercent =
                    static_cast<float>(MS_INVALID_VALUE);

            if (volt != 9999)
                derived()->_priorBatteryVoltage = static_cast<float>(volt);
            else
                derived()->_priorBatteryVoltage =
                    static_cast<float>(MS_INVALID_VALUE);
        } else {
            MS_DBG(F("Polling for all modem battery parameters is disabled"));
        }

        if ((derived()->_pollModemMetaData &
             MODEM_TEMPERATURE_ENABLE_BITMASK) ==
            MODEM_TEMPERATURE_ENABLE_BITMASK) {
            derived()->_priorModemTemp = derived()->getModemChipTemperature();
            MS_DBG(F("CURRENT Modem Chip Temperature:"),
                   derived()->_priorModemTemp);
        } else {
            MS_DBG(F("Polling for modem chip temperature is disabled"));
        }

        return success;
    }
    /**@}*/

    /* ===================================================================== */
    /* Static Functions                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_static_functions
     * @name Functions to return the current value of static member variables
     *
     * These functions do **NOT** query the modem for new values, they return
     * the stored value from the last poll.
     *
     * @note These must be static so that the modem variables can call
     * them.  (Non-static member functions cannot be called without an object.)
     */
    /**@{*/
 public:
    /**
     * @brief Get the stored Modem RSSI value.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored RSSI
     */
    static float getModemRSSI() {
        float retVal = Derived::_priorRSSI;
        MS_DEEP_DBG(F("PRIOR RSSI:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem signal strength as a percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored signal strength percent
     */
    static float getModemSignalPercent() {
        float retVal = Derived::_priorSignalPercent;
        MS_DEEP_DBG(F("PRIOR Percent signal strength:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem battery charge state.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery charge state
     */
    static float getModemBatteryChargeState() {
        float retVal = Derived::_priorBatteryState;
        MS_DEEP_DBG(F("PRIOR Modem Battery Charge State:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem battery charge percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery charge percent
     */
    static float getModemBatteryChargePercent() {
        float retVal = Derived::_priorBatteryPercent;
        MS_DEEP_DBG(F("PRIOR Modem Battery Charge Percentage:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem battery voltage.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery voltage in mV
     */
    static float getModemBatteryVoltage() {
        float retVal = Derived::_priorBatteryVoltage;
        MS_DEEP_DBG(F("PRIOR Modem Battery Voltage:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem temperature.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored temperature in degrees Celsius
     */
    static float getModemTemperature() {
        float retVal = Derived::_priorModemTemp;
        MS_DEEP_DBG(F("PRIOR Modem Chip Temperature:"), retVal);
        return retVal;
    }
    /**@}*/

    /* ===================================================================== */
    /* Polling Control                                                       */
    /* ===================================================================== */
 public:
    /**
     * @brief Enables metadata polling for one or more modem measured
     * variables. Setting this to 0b11111111 will enable polling for all modem
     * measured variables.
     *
     * @param pollingBitmask The bitmask indicating which parameters to poll.
     *
     * @see loggerModem::_pollModemMetaData
     *
     * @note This will **not** disable polling for any unset bits in the
     * provided bitmask.  It will only enable those bits that are set.
     */
    void enableMetadataPolling(uint8_t pollingBitmask) {
        derived()->_pollModemMetaData |= pollingBitmask;
    }

    /**
     * @brief Disables metadata polling for one or more modem measured
     * variables.  Setting this to 0b11111111 will disable polling for all
     * modem measured variables.
     *
     * @param pollingBitmask The bitmask indicating which parameters to disable.
     *
     * @see loggerModem::_pollModemMetaData
     *
     * @note This will **not** enable polling for any unset bits in the provided
     * bitmask.  It will only disable polling for those bits that are set.
     */
    void disableMetadataPolling(uint8_t pollingBitmask) {
        derived()->_pollModemMetaData &= ~pollingBitmask;
    }

    /**
     * @brief Sets the complete bitmask for modem metadata polling.
     *
     * This will enable polling for 1 bits and disable polling for 0 bits.
     * Setting this to 0 (0b00000000) will disable polling for all metadata
     * parameters.  Setting it to 255 (0b11111111) will enable polling for all
     * parameters.
     *
     * @param pollingBitmask The new polling bitmask.
     *
     * @see loggerModem::_pollModemMetaData
     */
    void setPollingMask(uint8_t pollingBitmask) {
        derived()->_pollModemMetaData = pollingBitmask;
    }

 protected:
    /**
     * @brief Polling mask for modem metadata
     *
     * An 8-bit code for the enabled modem polling variables
     *
     * Setting a bit to 0 will disable polling, to 1 will enable it.  By default
     * no polling is enabled to save time and power by not requesting
     * unnecessary information from the modem.  When modem measured variables
     * are attached to a modem, polling for those results is automatically
     * enabled.
     *
     * Bit | Variable Class | Relevent Define
     * ----|----------------|----------------
     *  0  | #Modem_RSSI | #MODEM_RSSI_ENABLE_BITMASK
     *  1  | #Modem_SignalPercent | #MODEM_PERCENT_SIGNAL_ENABLE_BITMASK
     *  2  | #Modem_BatteryState | #MODEM_BATTERY_STATE_ENABLE_BITMASK
     *  3  | #Modem_BatteryPercent | #MODEM_BATTERY_PERCENT_ENABLE_BITMASK
     *  4  | #Modem_BatteryVoltage | #MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK
     *  5  | #Modem_Temp | #MODEM_TEMPERATURE_ENABLE_BITMASK
     */
    uint8_t _pollModemMetaData = 0;
};

// cSpell:ignore bpercent

#endif  // SRC_LOGGERMODEMSENSINGMIXIN_H_
