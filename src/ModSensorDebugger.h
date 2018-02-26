/**
 * @file       ModSensorDebugger.h
 * @author     Volodymyr Shymanskyy (stolen from TinyGsmCommon.h)
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */


#ifndef ModSensorDebugger_h
#define ModSensorDebugger_h

#include <Arduino.h>

#ifndef STANDARD_SERIAL_OUTPUT
    #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
      #define Serial SERIAL_PORT_USBVIRTUAL
      #define STANDARD_SERIAL_OUTPUT Serial
    #elif defined __AVR__
      #define STANDARD_SERIAL_OUTPUT Serial
    #endif
#endif  // ifndef STANDARD_SERIAL_OUTPUT

#ifdef STANDARD_SERIAL_OUTPUT
    namespace {
        template<typename T>
        static void PRINTOUT(T last) {
            STANDARD_SERIAL_OUTPUT.print(last);
        }

        template<typename T, typename... Args>
        static void PRINTOUT(T head, Args... tail) {
            STANDARD_SERIAL_OUTPUT.print(head);
            PRINTOUT(tail...);
        }
    }
#endif  // STANDARD_SERIAL_OUTPUT


#ifdef DEBUGGING_SERIAL_OUTPUT
    namespace {
        template<typename T>
        static void MS_DBG(T last) {
            DEBUGGING_SERIAL_OUTPUT.print(last);
        }

        template<typename T, typename... Args>
        static void MS_DBG(T head, Args... tail) {
            DEBUGGING_SERIAL_OUTPUT.print(head);
            MS_DBG(tail...);
        }
    }
#else
    #define MS_DBG(...)
#endif  // DEBUGGING_SERIAL_OUTPUT


#endif  // ModSensorDebugger_h
