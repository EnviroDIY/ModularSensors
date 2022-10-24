/**
 * @file       ModSensorDebugger.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org> *
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

// Included Dependencies
#include <Arduino.h>

#ifndef STANDARD_SERIAL_OUTPUT
// #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#if defined(SERIAL_PORT_USBVIRTUAL)
// #define Serial SERIAL_PORT_USBVIRTUAL
#define STANDARD_SERIAL_OUTPUT SERIAL_PORT_USBVIRTUAL
#elif defined __AVR__ || defined ARDUINO_ARCH_AVR
#define STANDARD_SERIAL_OUTPUT Serial
#endif
#endif  // ifndef STANDARD_SERIAL_OUTPUT

#ifdef STANDARD_SERIAL_OUTPUT
// namespace {
/**
 * @brief Prints text to the "debugging" serial port.  This is intended for text
 * that should *always* be printed, even in field operation.
 *
 * @tparam T Any type that can be printed
 * @param last The last text to print out
 */
template <typename T>
static void PRINTOUT(T last) {
    STANDARD_SERIAL_OUTPUT.println(last);
}

/**
 * @brief Prints text to the "debugging" serial port.  This is intended for text
 * that should *always* be printed, even in field operation.
 *
 * @tparam T Any type that can be printed
 * @tparam Args Any type that can be printed
 * @param head The first text to print out
 * @param tail The last text to print out
 */
template <typename T, typename... Args>
static void PRINTOUT(T head, Args... tail) {
    STANDARD_SERIAL_OUTPUT.print(head);
    STANDARD_SERIAL_OUTPUT.print(' ');
    PRINTOUT(tail...);
}
// }  // namespace
#endif  // STANDARD_SERIAL_OUTPUT


#ifndef DEBUGGING_SERIAL_OUTPUT
// #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#if defined(SERIAL_PORT_USBVIRTUAL)
// #define Serial SERIAL_PORT_USBVIRTUAL
#define DEBUGGING_SERIAL_OUTPUT SERIAL_PORT_USBVIRTUAL
#elif defined __AVR__ || defined ARDUINO_ARCH_AVR
#define DEBUGGING_SERIAL_OUTPUT Serial
#endif
#endif  // ifndef DEBUGGING_SERIAL_OUTPUT

#if defined(DEBUGGING_SERIAL_OUTPUT) && defined(MS_DEBUGGING_STD)
// namespace {
/**
 * @brief Prints text to the "debugging" serial port.  This is intended for
 * debugging the code of a specific module.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 *
 * @tparam T Any type that can be printed
 * @param last The last text to print out
 */
template <typename T>
static void MS_DBG(T last) {
    DEBUGGING_SERIAL_OUTPUT.print(last);
    DEBUGGING_SERIAL_OUTPUT.print(" <--");
    DEBUGGING_SERIAL_OUTPUT.println(MS_DEBUGGING_STD);
}

/**
 * @brief Prints text to the "debugging" serial port.  This is intended for
 * debugging the code of a specific module.
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
    DEBUGGING_SERIAL_OUTPUT.print(head);
    DEBUGGING_SERIAL_OUTPUT.print(' ');
    MS_DBG(tail...);
}
// }  // namespace
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
 * @brief Prints text to the "debugging" serial port.  This is intended for
 * debugging the code of a specific module.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
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
#endif  // DEBUGGING_SERIAL_OUTPUT


#ifndef DEEP_DEBUGGING_SERIAL_OUTPUT
// #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#if defined(SERIAL_PORT_USBVIRTUAL)
// #define Serial SERIAL_PORT_USBVIRTUAL
#define DEEP_DEBUGGING_SERIAL_OUTPUT SERIAL_PORT_USBVIRTUAL
#elif defined __AVR__ || defined ARDUINO_ARCH_AVR
#define DEEP_DEBUGGING_SERIAL_OUTPUT Serial
#endif
#endif  // ifndef DEEP_DEBUGGING_SERIAL_OUTPUT

#if defined(DEEP_DEBUGGING_SERIAL_OUTPUT) && defined(MS_DEBUGGING_DEEP)
// namespace {
/**
 * @brief Prints text to the "debugging" serial port.  This is intended for
 * printouts considered to be excessive during "normal" debugging.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 *
 * @tparam T Any type that can be printed
 * @param last The last text to print out
 */
template <typename T>
static void MS_DEEP_DBG(T last) {
    DEEP_DEBUGGING_SERIAL_OUTPUT.print(last);
    DEEP_DEBUGGING_SERIAL_OUTPUT.print(" <--");
    DEEP_DEBUGGING_SERIAL_OUTPUT.println(MS_DEBUGGING_DEEP);
}
/**
 * @brief Prints text to the "debugging" serial port.  This is intended for
 * printouts considered to be excessive during "normal" debugging.
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
    DEEP_DEBUGGING_SERIAL_OUTPUT.print(head);
    DEEP_DEBUGGING_SERIAL_OUTPUT.print(' ');
    MS_DEEP_DBG(tail...);
}
// }  // namespace
#else
/**
 * @brief Prints text to the "debugging" serial port.  This is intended for
 * printouts considered to be excessive during "normal" debugging.
 *
 * The name of the header file calling the print command is appended to the end
 * of the text.
 */
#define MS_DEEP_DBG(...)
#endif  // DEEP_DEBUGGING_SERIAL_OUTPUT


/***
#if defined (__AVR__) || defined (ARDUINO_ARCH_AVR)
  typedef const __FlashStringHelper* GsmConstStr;
  #define GFP(x) (reinterpret_cast<GsmConstStr>(x))
  #define GF(x)  F(x)
#else
  typedef const char* GsmConstStr;
  #define GFP(x) x
  #define GF(x)  x
#endif
***/


#endif  // SRC_MODSENSORDEBUGGER_H_
