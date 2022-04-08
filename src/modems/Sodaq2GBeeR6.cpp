/**
 * @file Sodaq2GBeeR6.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Sodaq2GBeeR6 class.
 */

// Included Dependencies
#include "Sodaq2GBeeR6.h"


// Constructor
Sodaq2GBeeR6::Sodaq2GBeeR6(Stream* modemStream, int8_t powerPin,
                           int8_t statusPin, const char* apn)
    : SIMComSIM800(modemStream, powerPin, statusPin, -1, -1, apn) {
    setVRefPin(-1);
}


// Constructor
Sodaq2GBeeR6::Sodaq2GBeeR6(Stream* modemStream, int8_t vRefPin,
                           int8_t statusPin, int8_t powerPin, const char* apn)
    : SIMComSIM800(modemStream, powerPin, statusPin, -1, -1, apn) {
    setVRefPin(vRefPin);
}

// Destructor
Sodaq2GBeeR6::~Sodaq2GBeeR6() {}

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool Sodaq2GBeeR6::modemWakeFxn(void) {
    if (_vRefPin >= 0) {
        MS_DBG(F("Enabling voltage reference for GPRSBeeR6 on pin"), _vRefPin);
        digitalWrite(_vRefPin, HIGH);
    }
    return true;
}


bool Sodaq2GBeeR6::modemSleepFxn(void) {
    // Ask the SIM800 to shut down nicely
    MS_DBG(F("Asking SIM800 on GPRSBeeR6 to power down"));
    bool success = gsmModem.poweroff();
    if (_vRefPin >= 0) {
        MS_DBG(F("Disabling voltage reference for GPRSBeeR6 on pin"), _vRefPin);
        digitalWrite(_vRefPin, LOW);
    }
    return success;
}

bool Sodaq2GBeeR6::extraModemSetup(void) {
    bool success = gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    if (_vRefPin >= 0) pinMode(_vRefPin, OUTPUT);
    return success;
}

void Sodaq2GBeeR6::setVRefPin(int8_t vRefPin) {
    _vRefPin = vRefPin;
}
