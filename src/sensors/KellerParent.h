/**
 * @file KellerParent.h
 * @copyright 2017-2022 Stroud Water Research Center
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
/* clang-format off */
/**
 * @defgroup keller_group Keller Water Level Sensors
 * The Sensor and Variable objects for all Keller water level sensors.
 *
 * @ingroup the_sensors
 *
 *
 * This library currently supports the following Keller pressure and water level sensors:
 * - [Acculevel](https://www.kelleramerica.com/submersible-level-transmitters/acculevel-high-accuracy-submersible-level-transmitter.html)
 *     - [Manual](https://www.kelleramerica.com/manuals-and-software/manuals/Keller_America_Users_Guide.pdf)
 *     - [Datasheet](https://www.kelleramerica.com/pdf-library/High%20Accuracy%20Analog%20Digital%20Submersible%20Level%20Transmitters%20Acculevel.pdf)
 *     - [Class Documentation](@ref sensor_acculevel)
 * - [Nanolevel](https://www.kelleramerica.com/submersible-level-transmitters/nanolevel-ceramic-capacitive-level-transmitter.html)
 *     - [Manual](https://www.kelleramerica.com/manuals-and-software/manuals/Keller_America_Users_Guide.pdf)
 *     - [Datasheet](https://www.kelleramerica.com/pdf-library/Very%20Low%20Range%20Submersible%20Transmitter%20Nanolevel.pdf)
 *     - [Class Documentation](@ref sensor_nanolevel)
 * - Any Keller Series 30, Class 5, Group 20 sensors that are software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 *
 * These sensors communicate via Modbus RTU over RS485 at 9600 baud.
 * More documentation for our implementation of the Keller Modbus communication
 * commands and responses, along with information about the various variables,
 * can be found in the [EnviroDIY KellerModbus library](https://github.com/EnviroDIY/KellerModbus).
 *
 * Sensors ship with default slave addresses set to 0x01, which can be set by
 * the user.
 *
 * The Keller sensors expect an input voltage of 9-28 VDC, so they also require
 * a voltage booster and an RS485 to TTL Serial converter with logic level
 * shifting from the higher output voltage to the 3.3V or 5V of the Arduino data
 * logging board.
 *
 * Digital communication with Keller sensors configured for SDI12 communication
 * protocols are not supported by this library.
 *
 * The sensor constructors require as input: the sensor modbus address,  a
 * stream instance for data (ie, ```Serial```), and one or two power pins.  The
 * Arduino pin controlling the receive and data enable on your RS485-to-TTL
 * adapter and the number of readings to average are optional.  (Use -1 for the
 * second power pin and -1 for the enable pin if these don't apply and you want
 * to average more than one reading.) Please see the section
 * "[Notes on Arduino Streams and Software Serial](@ref page_arduino_streams)"
 * for more information about what streams can be used along with this library.
 * In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to
 * communicate with these sensors, because it isn't stable enough. AltSoftSerial
 * and HardwareSerial work fine.  Up to two power pins are provided so that the
 * RS485 adapter, the sensor and/or an external power relay can be controlled
 * separately.  If the power to everything is controlled by the same pin, use -1
 * for the second power pin or omit the argument.  If they are controlled by
 * different pins _and no other sensors are dependent on power from either pin_
 * then the order of the pins doesn't matter.  If the RS485 adapter, sensor, or
 * relay are controlled by different pins _and any other sensors are controlled
 * by the same pins_ you should put the shared pin first and the un-shared pin
 * second.  Both pins _cannot_ be shared pins.
 *
 * The lower level details of the communication with the sensors is managed by
 * the [EnviroDIY Keller library](https://github.com/EnviroDIY/KellerModbus)
 */
/* clang-format on */

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

/** @ingroup keller_group */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Keller level sensors can report 3
/// values.
#define KELLER_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define KELLER_INC_CALC_VARIABLES 0

/**
 * @anchor keller_pressure
 * @name Pressure
 * The pressure variable from a Keller modbus sensor
 */
/**@{*/
/// Variable number; pressure is stored in sensorValues[0].
#define KELLER_PRESSURE_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "pressureGauge"
#define KELLER_PRESSURE_VAR_NAME "pressureGauge"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millibar"
#define KELLER_PRESSURE_UNIT_NAME "millibar"
/**@}*/

/**
 * @anchor keller_temp
 * @name Temperature
 * The temperature variable from a Keller modbus sensor
 */
/**@{*/
/// Variable number; temperature is stored in sensorValues[1].
#define KELLER_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define KELLER_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define KELLER_TEMP_UNIT_NAME "degreeCelsius"
/**@}*/

/**
 * @anchor keller_height
 * @name Height
 * The height variable from a Keller modbus sensor
 */
/**@{*/
/// Variable number; height is stored in sensorValues[2].
#define KELLER_HEIGHT_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "gaugeHeight"
#define KELLER_HEIGHT_VAR_NAME "gaugeHeight"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "meter"
#define KELLER_HEIGHT_UNIT_NAME "meter"
/**@}*/

/**
 * @brief The Sensor sub-class for all
 * [Keller water level sensors](@ref keller_group).
 *
 * @ingroup keller_group
 */
class KellerParent : public Sensor {
 public:
    /**
     * @brief Construct a new Keller Parent object.  This is only intended to be
     * used within this library.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Keller
     * sensor.  Use -1 if it is continuously powered.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.  An
     * RS485 adapter with integrated flow control is strongly recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param model The model of Keller sensor.
     * @param sensName The name of the sensor.  Defaults to "Keller-Sensor".
     * @param numVariables The number of variable results returned by the
     * sensor. Defaults to 3.
     * @param warmUpTime_ms The time in ms between when the sensor is powered on
     * and when it is ready to receive a wake command.  Defaults to 500.
     * @param stabilizationTime_ms The time in ms between when the sensor
     * receives a wake command and when it is able to return stable values.
     * Defaults to 5,000.
     * @param measurementTime_ms The time in ms between when a measurement is
     * started and when the result value is available.  Defaults to 1,500.
     */
    KellerParent(byte modbusAddress, Stream* stream, int8_t powerPin,
                 int8_t powerPin2, int8_t enablePin = -1,
                 uint8_t measurementsToAverage = 1, kellerModel model = OTHER,
                 const char* sensName = "Keller-Sensor",
                 uint8_t numVariables = 3, uint32_t warmUpTime_ms = 500,
                 uint32_t stabilizationTime_ms = 5000,
                 uint32_t measurementTime_ms   = 1500);
    /**
     * @copydoc KellerParent::KellerParent
     */
    KellerParent(byte modbusAddress, Stream& stream, int8_t powerPin,
                 int8_t powerPin2, int8_t enablePin = -1,
                 uint8_t measurementsToAverage = 1, kellerModel model = OTHER,
                 const char* sensName = "Keller-Sensor",
                 uint8_t numVariables = 3, uint32_t warmUpTime_ms = 500,
                 uint32_t stabilizationTime_ms = 5000,
                 uint32_t measurementTime_ms   = 1500);
    /**
     * @brief Destroy the Keller Parent object - no action taken
     */
    virtual ~KellerParent();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets pin modes on the #_powerPin, adapter power, and adapter
     * enable pins.  It also sets the expected stream timeout for modbus and
     * updates the #_sensorStatus.  No sensor power is required.  This will
     * always return true.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    // Override these to use two power pins
    void powerUp(void) override;
    void powerDown(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    keller      _ksensor;
    kellerModel _model;
    byte        _modbusAddress;
    Stream*     _stream;
    int8_t      _RS485EnablePin;
    int8_t      _powerPin2;
};
/**@}*/
#endif  // SRC_SENSORS_KELLERPARENT_H_
