/**
 * @file ModSensorDebugger.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Volodymyr Shymanskyy
 *
 * @brief Contains template functions used to print out debugging information.
 *
 * These classes are taken with minor modifications from the debugger in
 * TinyGsmCommon.h from the TinyGSM library authored by Volodymyr Shymanskyy.
 */


// Header Guards
#ifndef SRC_MODSENSORDEBUGGER_H_
#define SRC_MODSENSORDEBUGGER_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Included Dependencies
#include <Arduino.h>

// Helpers for strings stored in flash
// These are blatantly copied from the TinyGSM library
/**
 * @def TINY_GSM_PROGMEM
 * @brief A macro to use to put strings into flash memory for AVR boards but not
 * for other boards that don't support it.
 *
 * @typedef GsmConstStr
 * @brief A type for a string stored in flash memory for AVR boards but not for
 * boards that don't support it.
 *
 * @def GFP(x)
 * @brief A macro to convert a string into a __FlashStringHelper (aka
 * GsmConstStr) for AVR boards but not for other boards that don't support it.
 * @param x The string to convert
 *
 * @def GF(x)
 * @brief A macro to store string in flash memory for AVR boards but not for
 * other boards that don't support it.
 * @param x The string to store in flash memory
 */
#if (defined(__AVR__) || defined(ARDUINO_ARCH_AVR)) &&            \
    !defined(__AVR_ATmega4809__) && !defined(TINY_GSM_PROGMEM) && \
    !defined(GFP) && !defined(GF)
#define TINY_GSM_PROGMEM PROGMEM
typedef const __FlashStringHelper* GsmConstStr;
#define GFP(x) (reinterpret_cast<GsmConstStr>(x))
#define GF(x) F(x)
#elif !defined(TINY_GSM_PROGMEM) && !defined(GFP) && !defined(GF)
#define TINY_GSM_PROGMEM
typedef const char* GsmConstStr;
#define GFP(x) x
#define GF(x) x
#endif

/**
 * @def MS_DEFAULT_OUTPUT
 * @brief The "default" serial output based on defines within the board
 * framework.
 *
 * @def MS_OUTPUT
 * @brief The serial output for debugging
 *
 * @def MS_SERIAL_OUTPUT
 * @brief The serial output for debugging
 *
 */
#if defined(SERIAL_PORT_USBVIRTUAL)
#define MS_DEFAULT_OUTPUT SERIAL_PORT_USBVIRTUAL
#elif defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
#define MS_DEFAULT_OUTPUT Serial
#endif

#if !defined(MS_OUTPUT) && !defined(MS_SILENT)
#define MS_OUTPUT MS_DEFAULT_OUTPUT
#endif  // ifndef MS_OUTPUT

#if defined(MS_2ND_OUTPUT) && !defined(MS_SERIAL_OUTPUT)
#include <StreamDebugger.h>
/**
 * @brief A StreamDebugger instance to link the two outputs.
 */
static StreamDebugger MS_LINKED_OUTPUT(MS_OUTPUT, MS_2ND_OUTPUT);
#define MS_SERIAL_OUTPUT MS_LINKED_OUTPUT
#elif !defined(MS_SERIAL_OUTPUT)
#define MS_SERIAL_OUTPUT MS_OUTPUT
#endif


#if !defined(MS_SILENT)
/**
 * @brief Helper to print text to MS_SERIAL_OUTPUT that should *always* be
 * printed, even in field operation.
 *
 * @tparam T Any type that can be printed
 * @param last The last text to print out
 */
template <typename T>
static void PRINTOUT(T last) {
    MS_SERIAL_OUTPUT.println(last);
    MS_SERIAL_OUTPUT.flush();
}
/**
 * @brief Helper to print text to MS_SERIAL_OUTPUT that should *always* be
 * printed, even in field operation.
 *
 * @tparam T Any type that can be printed
 * @tparam Args Any type that can be printed
 * @param head The first text to print out
 * @param tail The last text to print out
 */
template <typename T, typename... Args>
static void PRINTOUT(T head, Args... tail) {
    MS_SERIAL_OUTPUT.print(head);
    MS_SERIAL_OUTPUT.print(' ');
    PRINTOUT(tail...);
}

#else
/**
 * @brief Helper to print text that should *always* be printed, even in field
 * operation.
 */
#define PRINTOUT(...)
#endif


#if defined(MS_DEBUGGING_STD) && !defined(MS_SILENT)
/**
 * @brief Helper to print debugging text to MS_SERIAL_OUTPUT.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 *
 * @tparam T Any type that can be printed
 * @param last The last text to print out
 */
template <typename T>
static void MS_DBG(T last) {
    MS_SERIAL_OUTPUT.print(last);
    MS_SERIAL_OUTPUT.print(" <--");
    MS_SERIAL_OUTPUT.println(MS_DEBUGGING_STD);
    MS_SERIAL_OUTPUT.flush();
}
/**
 * @brief Helper to print debugging text to MS_SERIAL_OUTPUT.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 *
 * @tparam T Any type that can be printed
 * @tparam Args Any type that can be printed
 * @param head The first text to print out
 * @param tail The last text to print out
 */
template <typename T, typename... Args>
static void MS_DBG(T head, Args... tail) {
    MS_SERIAL_OUTPUT.print(head);
    MS_SERIAL_OUTPUT.print(' ');
    MS_DBG(tail...);
}
/**
 * @brief Initializes a variable called start with the current processor millis.
 *
 * Only to be used with debugging.
 */
#define MS_START_DEBUG_TIMER uint32_t start = millis();
/**
 * @brief Re-sets the variable called start to the current processor millis.
 *
 * Only to be used with debugging.
 */
#define MS_RESET_DEBUG_TIMER start = millis();
/**
 * @brief Calculates the difference between the current processor millis and the
 * value of the start variable.
 *
 * Only to be used with debugging.
 */
#define MS_PRINT_DEBUG_TIMER millis() - start

#else
/**
 * @brief Helper to print debugging text.
 */
#define MS_DBG(...)
/**
 * @brief Initializes a variable called start with the current processor millis.
 *
 * Only to be used with debugging.
 */
#define MS_START_DEBUG_TIMER
/**
 * @brief Re-sets the variable called start to the current processor millis.
 *
 * Only to be used with debugging.
 */
#define MS_RESET_DEBUG_TIMER
/**
 * @brief Calculates the difference between the current processor millis and the
 * value of the start variable.
 *
 * Only to be used with debugging.
 */
#define MS_PRINT_DEBUG_TIMER
#endif


#if defined(MS_DEBUGGING_DEEP) && !defined(MS_SILENT)
/**
 * @brief Helper to print debugging text to MS_SERIAL_OUTPUT.  This is intended
 * for printouts considered to be excessive during "normal" debugging.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 *
 * @tparam T Any type that can be printed
 * @param last The last text to print out
 */
template <typename T>
static void MS_DEEP_DBG(T last) {
    MS_SERIAL_OUTPUT.print(last);
    MS_SERIAL_OUTPUT.print(" <--");
    MS_SERIAL_OUTPUT.println(MS_DEBUGGING_DEEP);
    MS_SERIAL_OUTPUT.flush();
}
/**
 * @brief Helper to print debugging text to MS_SERIAL_OUTPUT.  This is intended
 * for printouts considered to be excessive during "normal" debugging.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 *
 * @tparam T Any type that can be printed
 * @tparam Args Any type that can be printed
 * @param head The first text to print out
 * @param tail The last text to print out
 */
template <typename T, typename... Args>
static void MS_DEEP_DBG(T head, Args... tail) {
    MS_SERIAL_OUTPUT.print(head);
    MS_SERIAL_OUTPUT.print(' ');
    MS_DEEP_DBG(tail...);
}

#else
/**
 * @brief Helper to print debugging text to MS_SERIAL_OUTPUT.  This is intended
 * for printouts considered to be excessive during "normal" debugging.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 */
#define MS_DEEP_DBG(...)
#endif


#endif  // SRC_MODSENSORDEBUGGER_H_
