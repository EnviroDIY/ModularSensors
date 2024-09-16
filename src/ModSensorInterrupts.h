/**
 * @file ModSensorInterrupts.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief A work-around for the AVR interrupt library not supporting SAMD
 * boards. That libary isn't necessary for them.
 */


// Header Guards
#ifndef SRC_MODSENSORINTERRUPTS_H_
#define SRC_MODSENSORINTERRUPTS_H_

#if defined(__AVR__)
// #define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts
#else
#define enableInterrupt(pin, userFunc, mode) \
    attachInterrupt(pin, userFunc, mode)
#define disableInterrupt(pin) detachInterrupt(pin)
#endif


#endif  // SRC_MODSENSORINTERRUPTS_H_
