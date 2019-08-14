/**
 * @file       ModSensorDebugger.h
 * @author     Volodymyr Shymanskyy (stolen from TinyGsmCommon.h)
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */


// Header Guards
#ifndef ModSensorDebugger_h
#define ModSensorDebugger_h

// Included Dependencies
#include <Arduino.h>

// The current library version number
#define MODULAR_SENSORS_VERSION "0.21.3"

#ifndef STANDARD_SERIAL_OUTPUT
    // #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
    #if defined(SERIAL_PORT_USBVIRTUAL)
      // #define Serial SERIAL_PORT_USBVIRTUAL
      #define STANDARD_SERIAL_OUTPUT SERIAL_PORT_USBVIRTUAL
    #elif defined __AVR__
      #define STANDARD_SERIAL_OUTPUT Serial
    #endif
#endif  // ifndef STANDARD_SERIAL_OUTPUT

#ifdef STANDARD_SERIAL_OUTPUT
    namespace {
        template<typename T>
        static void PRINTOUT(T last) {
            STANDARD_SERIAL_OUTPUT.println(last);
        }

        template<typename T, typename... Args>
        static void PRINTOUT(T head, Args... tail) {
            STANDARD_SERIAL_OUTPUT.print(head);
            STANDARD_SERIAL_OUTPUT.print(' ');
            PRINTOUT(tail...);
        }
    }
#endif  // STANDARD_SERIAL_OUTPUT


#ifndef DEBUGGING_SERIAL_OUTPUT
    // #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
    #if defined(SERIAL_PORT_USBVIRTUAL)
      // #define Serial SERIAL_PORT_USBVIRTUAL
      #define DEBUGGING_SERIAL_OUTPUT SERIAL_PORT_USBVIRTUAL
    #elif defined __AVR__
      #define DEBUGGING_SERIAL_OUTPUT Serial
    #endif
#endif  // ifndef DEBUGGING_SERIAL_OUTPUT

#if defined DEBUGGING_SERIAL_OUTPUT  && defined MS_DEBUGGING_STD
    namespace {
        template<typename T>
        static void MS_DBG(T last) {
            DEBUGGING_SERIAL_OUTPUT.println(last);
        }

        template<typename T, typename... Args>
        static void MS_DBG(T head, Args... tail) {
            DEBUGGING_SERIAL_OUTPUT.print(head);
            DEBUGGING_SERIAL_OUTPUT.print(' ');
            MS_DBG(tail...);
        }
    }
#else
    #define MS_DBG(...)
#endif  // DEBUGGING_SERIAL_OUTPUT


#if defined DEBUGGING_SERIAL_OUTPUT && defined MS_DEBUGGING_DEEP
    namespace {
        template<typename T>
        static void MS_DEEP_DBG(T last) {
            DEEP_DEBUGGING_SERIAL_OUTPUT.println(last);
        }

        template<typename T, typename... Args>
        static void MS_DEEP_DBG(T head, Args... tail) {
            DEEP_DEBUGGING_SERIAL_OUTPUT.print(head);
            DEEP_DEBUGGING_SERIAL_OUTPUT.print(' ');
            MS_DEEP_DBG(tail...);
        }
    }
#else
    #define MS_DEEP_DBG(...)
#endif  // DEEP_DEBUGGING_SERIAL_OUTPUT


/***
#if defined(__AVR__)
  typedef const __FlashStringHelper* GsmConstStr;
  #define GFP(x) (reinterpret_cast<GsmConstStr>(x))
  #define GF(x)  F(x)
#else
  typedef const char* GsmConstStr;
  #define GFP(x) x
  #define GF(x)  x
#endif
***/


#endif  // ModSensorDebugger_h
