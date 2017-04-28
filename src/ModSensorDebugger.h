/**
 * @file       ModSensorDebugger.h
 * @author     Volodymyr Shymanskyy (stolen from TinyGsmCommon.h)
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */


 #ifndef ModSensorDebugger_h
 #define ModSensorDebugger_h

#ifdef MODULES_DBG
namespace {
 template<typename T>
 static void DBGM(T last) {
   MODULES_DBG.print(last);
 }

 template<typename T, typename... Args>
 static void DBGM(T head, Args... tail) {
   MODULES_DBG.print(head);
   DBGM(tail...);
 }
}
#else
 #define DBGM(...)
#endif

#endif
