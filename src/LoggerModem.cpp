/**
 * @file LoggerModem.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the loggerModem class.
 */

#include "LoggerModem.h"

// Initialize the static members
int16_t loggerModem::_priorRSSI           = MS_INVALID_VALUE;
int16_t loggerModem::_priorSignalPercent  = MS_INVALID_VALUE;
float   loggerModem::_priorModemTemp      = MS_INVALID_VALUE;
float   loggerModem::_priorBatteryState   = MS_INVALID_VALUE;
float   loggerModem::_priorBatteryPercent = MS_INVALID_VALUE;
float   loggerModem::_priorBatteryVoltage = MS_INVALID_VALUE;


// Polling functions for modem metadata
void loggerModem::enableMetadataPolling(uint8_t pollingBitmask) {
    _pollModemMetaData |= pollingBitmask;
}
void loggerModem::disableMetadataPolling(uint8_t pollingBitmask) {
    _pollModemMetaData |= ~pollingBitmask;
}
void loggerModem::setMetadataPolling(uint8_t pollingBitmask) {
    _pollModemMetaData = pollingBitmask;
}


// standard implementation for updating modem metadata
bool loggerModem::updateModemMetadata() {
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI           = MS_INVALID_VALUE;
    loggerModem::_priorSignalPercent  = MS_INVALID_VALUE;
    loggerModem::_priorBatteryState   = MS_INVALID_VALUE;
    loggerModem::_priorBatteryPercent = MS_INVALID_VALUE;
    loggerModem::_priorBatteryVoltage = MS_INVALID_VALUE;
    loggerModem::_priorModemTemp      = MS_INVALID_VALUE;

    // Initialize variable
    int16_t rssi     = MS_INVALID_VALUE;
    int16_t percent  = MS_INVALID_VALUE;
    int8_t  state    = 99;
    int8_t  bpercent = -99;
    int16_t volt     = 9999;

    MS_DBG(F("Modem polling settings:"), String(_pollModemMetaData, BIN));

    if ((_pollModemMetaData & MODEM_RSSI_ENABLE_BITMASK) ==
            MODEM_RSSI_ENABLE_BITMASK ||
        (_pollModemMetaData & MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) ==
            MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) {
        // Try for up to 15 seconds to get a valid signal quality
        uint32_t startMillis = millis();
        do {
            success &= getModemSignalQuality(rssi, percent);
            loggerModem::_priorRSSI          = rssi;
            loggerModem::_priorSignalPercent = percent;
            if (rssi != 0 && rssi != MS_INVALID_VALUE) break;
            delay(250);
        } while ((rssi == 0 || rssi == MS_INVALID_VALUE) &&
                 millis() - startMillis < 15000L && success);
        MS_DBG(F("CURRENT RSSI:"), rssi);
        MS_DBG(F("CURRENT Percent signal strength:"), percent);
    } else {
        MS_DBG(F("Polling for both RSSI and signal strength is disabled"));
    }

    if ((_pollModemMetaData & MODEM_BATTERY_STATE_ENABLE_BITMASK) ==
            MODEM_BATTERY_STATE_ENABLE_BITMASK ||
        (_pollModemMetaData & MODEM_BATTERY_PERCENT_ENABLE_BITMASK) ==
            MODEM_BATTERY_PERCENT_ENABLE_BITMASK ||
        (_pollModemMetaData & MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) ==
            MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) {
        success &= getModemBatteryStats(state, bpercent, volt);
        MS_DBG(F("CURRENT Modem Battery Charge State:"), state);
        MS_DBG(F("CURRENT Modem Battery Charge Percentage:"), bpercent);
        MS_DBG(F("CURRENT Modem Battery Voltage:"), volt);
        if (state != 99)
            loggerModem::_priorBatteryState = static_cast<float>(state);
        else
            loggerModem::_priorBatteryState =
                static_cast<float>(MS_INVALID_VALUE);

        if (bpercent != -99)
            loggerModem::_priorBatteryPercent = static_cast<float>(bpercent);
        else
            loggerModem::_priorBatteryPercent =
                static_cast<float>(MS_INVALID_VALUE);

        if (volt != 9999)
            loggerModem::_priorBatteryVoltage = static_cast<float>(volt);
        else
            loggerModem::_priorBatteryVoltage =
                static_cast<float>(MS_INVALID_VALUE);
    } else {
        MS_DBG(F("Polling for all modem battery parameters is disabled"));
    }

    if ((_pollModemMetaData & MODEM_TEMPERATURE_ENABLE_BITMASK) ==
        MODEM_TEMPERATURE_ENABLE_BITMASK) {
        loggerModem::_priorModemTemp = getModemChipTemperature();
        MS_DBG(F("CURRENT Modem Chip Temperature:"),
               loggerModem::_priorModemTemp);
    } else {
        MS_DBG(F("Polling for modem chip temperature is disabled"));
    }

    return success;
}


// functions to get the current value of static member variables
float loggerModem::getModemRSSI() {
    float retVal = loggerModem::_priorRSSI;
    MS_DEEP_DBG(F("PRIOR RSSI:"), retVal);
    return retVal;
}
float loggerModem::getModemSignalPercent() {
    float retVal = loggerModem::_priorSignalPercent;
    MS_DEEP_DBG(F("PRIOR Percent signal strength:"), retVal);
    return retVal;
}
float loggerModem::getModemBatteryChargeState() {
    float retVal = loggerModem::_priorBatteryState;
    MS_DEEP_DBG(F("PRIOR Modem Battery Charge State:"), retVal);
    return retVal;
}
float loggerModem::getModemBatteryChargePercent() {
    float retVal = loggerModem::_priorBatteryPercent;
    MS_DEEP_DBG(F("PRIOR Modem Battery Charge Percentage:"), retVal);
    return retVal;
}
float loggerModem::getModemBatteryVoltage() {
    float retVal = loggerModem::_priorBatteryVoltage;
    MS_DEEP_DBG(F("PRIOR Modem Battery Voltage:"), retVal);
    return retVal;
}
float loggerModem::getModemTemperature() {
    float retVal = loggerModem::_priorModemTemp;
    MS_DEEP_DBG(F("PRIOR Modem Chip Temperature:"), retVal);
    return retVal;
}

// cSpell:ignore bpercent
