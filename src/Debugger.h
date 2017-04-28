/**
 * @file       Debugger.h
 * @author     Volodymyr Shymanskyy (stolen from TinyGsmCommon.h)
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */

#ifdef TINY_GSM_DEBUG
namespace {
  template<typename T>
  static void DBG(T last) {
    TINY_GSM_DEBUG.print(last);
  }

  template<typename T, typename... Args>
  static void DBG(T head, Args... tail) {
    TINY_GSM_DEBUG.print(head);
    DBG(tail...);
  }
}
#else
  #define DBG(...)
#endif


#ifdef SENSORS_DBG
namespace {
 template<typename T>
 static void DBGS(T last) {
   SENSORS_DBG.print(last);
 }

 template<typename T, typename... Args>
 static void DBGS(T head, Args... tail) {
   SENSORS_DBG.print(head);
   DBGS(tail...);
 }
}
#else
 #define DBGS(...)
#endif


#ifdef VARIABLES_DBG
namespace {
 template<typename T>
 static void DBGV(T last) {
   VARIABLES_DBG.print(last);
 }

 template<typename T, typename... Args>
 static void DBGV(T head, Args... tail) {
   VARIABLES_DBG.print(head);
   DBGV(tail...);
 }
}
#else
 #define DBGV(...)
#endif


#ifdef VAR_ARRAY_DBG
namespace {
 template<typename T>
 static void DBGVA(T last) {
   VAR_ARRAY_DBG.print(last);
 }

 template<typename T, typename... Args>
 static void DBGVA(T head, Args... tail) {
   VAR_ARRAY_DBG.print(head);
   DBGVA(tail...);
 }
}
#else
 #define DBGVA(...)
#endif


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
