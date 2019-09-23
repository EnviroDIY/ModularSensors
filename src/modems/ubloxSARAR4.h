/*
 *ubloxSARAR4.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all u-blox SARA R4 and N4 LTE modules.  This is an extra
 *level on top of the DigiXBee3GBypass and the Sodaq UBee U201 needed so that
 *TinyGSMublox.h is only included once.  Because that h file contains the
 *definitions of functions instead of mearly their declarations, including
 *it more than once causes a violation of the one-definition rule and
 *possible undefined behavior (ie, bad things).
*/

// Header Guards
#ifndef ubloxSARAR4_h
#define ubloxSARAR4_h

#define TINY_GSM_MODEM_UBLOX
#include "TinyGsmClient.h"

#endif  // Header Guard
