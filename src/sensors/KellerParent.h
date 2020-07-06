/**
 * @file KellerParent.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the KellerParent sensor subclass, itself used as a parent
 * class for all Keller modbus sensors.
 *
 * This depends on the KellerModbus and SensorModbusMaster libraries.
 *
 * Documentation for the Keller Protocol commands and responses, along with
 * information about the various variables, can be found in the EnviroDIY
 * KellerModbus library at: https://github.com/EnviroDIY/KellerModbus
 */

// Header Guards
#ifndef SRC_SENSORS_KELLERPARENT_H_
#define SRC_SENSORS_KELLERPARENT_H_

// Debugging Statement
// #define MS_KELLERPARENT_DEBUG
// #define MS_KELLERPARENT_DEBUG_DEEP

#ifdef MS_KELLERPARENT_DEBUG
#define MS_DEBUGGING_STD "KellerParent"
#endif

#ifdef MS_KELLERPARENT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "KellerParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"
#include <KellerModbus.h>

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Keller level sensors can report 3 values.
#define KELLER_NUM_VARIABLES 3
/// Variable number; pressure is stored in sensorValues[0].
#define KELLER_PRESSURE_VAR_NUM 0
/// Variable number; temperature is stored in sensorValues[1].
#define KELLER_TEMP_VAR_NUM 1
/// Variable number; height is stored in sensorValues[2].
#define KELLER_HEIGHT_VAR_NUM 2

// The main class for the Keller Sensors
class KellerParent : public Sensor {
public:
  KellerParent(byte modbusAddress, Stream *stream, int8_t powerPin,
               int8_t powerPin2, int8_t enablePin = -1,
               uint8_t measurementsToAverage = 1, kellerModel model = OTHER,
                 const char* sensName = "Keller-Sensor",
                 uint8_t numVariables = 3, uint32_t warmUpTime_ms = 500,
               uint32_t stabilizationTime_ms = 5000,
               uint32_t measurementTime_ms = 1500);
  KellerParent(byte modbusAddress, Stream &stream, int8_t powerPin,
               int8_t powerPin2, int8_t enablePin = -1,
               uint8_t measurementsToAverage = 1, kellerModel model = OTHER,
                 const char* sensName = "Keller-Sensor",
                 uint8_t numVariables = 3, uint32_t warmUpTime_ms = 500,
               uint32_t stabilizationTime_ms = 5000,
               uint32_t measurementTime_ms = 1500);
  virtual ~KellerParent();

  String getSensorLocation(void) override;

    bool setup(void) override;

  // Override these to use two power pins
    void powerUp(void) override;
    void powerDown(void) override;

    bool addSingleMeasurementResult(void) override;
  void registerPinPowerMng(void (*fn)(bool));
  // Pins that need to be managed during power On and Off events
  void registerSerialPins(uint8_t txPin, uint8_t rxPin);
  // This sets a stream for debugging information to go to;
  void setDebugStream(Stream *stream) { _ksensor.setDebugStream(stream); }
  void stopDebugging(void) { _ksensor.stopDebugging(); }

private:
    keller      _ksensor;
  kellerModel _model;
  byte _modbusAddress;
  Stream *_stream;
  int8_t _RS485EnablePin;
  int8_t _powerPin2;
  void (*_pinPowerMngFn)(bool) = nullptr;
  uint8_t _txPin = -1;
  uint8_t _rxPin = -1;
};

#endif  // SRC_SENSORS_KELLERPARENT_H_
