/**
 * @file GroPointParent.cpp
 * @copyright 2017-2023 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Contains the GroPointParent sensor subclass, itself a parent
 * class for all GroPoint Soil Moisture and Temperature sensors
 * that communicate via SDI-12 or Modbus.
 * NOTE: Presently this library only supports Modbus communication and GroPoint
 * Profile Multi Segment Soil Moisture & Temperature Profiling Probes (GPLP-X)
 * via the EnviroDIY GroPointModbus library.
 *
 * Documentation for the GroPointModbus Modbus Protocol commands and responses,
 * along with information about the various variables, can be found in the
 * EnviroDIY GroPointModbus library at:
 * https://github.com/EnviroDIY/GroPointModbus
 */
/* clang-format off */
/**
 * @defgroup gropoint_group GroPoint Sensors
 * The Sensor and Variable objects for all GroPoint sensors.
 *
 * @ingroup the_sensors
 *
 *
 * This library currently supports the following [GroPoint](https://www.gropoint.com) sensors:
 * - [GroPoint Profile GPLP-8](https://www.gropoint.com/products/soil-sensors/gropoint-profile), 8-Segment Soil Moisture & Temperature Profiling Probe
 *     - [GroPoint Profile User Manual](https://www.gropoint.com/s/2625-N-T-GroPoint-Profile-User-Manual-V113.pdf), including Modbus Instructions.
 *     - [GroPoint Profile Technical Info](https://www.gropoint.com/s/GroPoint-Profile-Technical-Info.pdf)
 *     - [Class Documentation](@ref sensor_gplp8)
 *
 * These sensors operate a 7.5 to 14.0 VDC power supply (Max 18.0 VDC). The power supply can be stopped between measurements for all.
 * They communicate via [Modbus RTU](https://en.wikipedia.org/wiki/Modbus) over [RS-485](https://en.wikipedia.org/wiki/RS-485).
 * To interface with them, you will need an RS485-to-TTL adapter.
 *
 * The sensor constructor requires as input: the sensor modbus address, a stream instance for data (ie, ```Serial```), and one or two power pins.
 * The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.
 * (Use -1 for the second power pin and -1 for the enable pin if these don't apply and you want to average more than one reading.)
 * Please see the section "[Notes on Arduino Streams and Software Serial](@ref page_arduino_streams)"
 * for more information about what streams can be used along with this library.
 * In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to communicate with these sensors, because it isn't stable enough.
 * AltSoftSerial and HardwareSerial work fine.
 * Up to two power pins are provided so that the RS485 adapter, the sensor and/or an external power relay can be controlled separately.
 * If the power to everything is controlled by the same pin, use -1 for the second power pin or omit the argument.
 * If they are controlled by different pins _and no other sensors are dependent on power from either pin_ then the order of the pins doesn't matter.
 * If the RS485 adapter, sensor, or relay are controlled by different pins _and any other sensors are controlled by the same pins_ you should put the shared pin first and the un-shared pin second.
 * Both pins _cannot_ be shared pins.
 *
 * By default, this library cuts power to the sensors between readings, causing them to lose track of their brushing interval.
 * The library manually activates the brushes as part of the "wake" command.
 * There are currently no other ways to set the brushing interval in this library.
 *
 * The lower level details of the communication with the sensors is managed by the
 * [EnviroDIY GroPointModbus library](https://github.com/EnviroDIY/GroPointModbus)
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_GROPOINTPARENT_H_
#define SRC_SENSORS_GROPOINTPARENT_H_

// Debugging Statement
// #define MS_GROPOINTPARENT_DEBUG
// #define MS_GROPOINTPARENT_DEBUG_DEEP

#ifdef MS_GROPOINTPARENT_DEBUG
#define MS_DEBUGGING_STD "GroPointParent"
#endif

#ifdef MS_GROPOINTPARENT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "GroPointParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"
#include "GroPointModbus.h"

/* clang-format off */
/**
 * @brief The Sensor sub-class for all the [GroPoint sensors](@ref gropoint_group)
 *
 * @ingroup gropoint_group
 */
/* clang-format on */
class GroPointParent : public Sensor {
 public:
    /**
     * @brief Construct a new GroPoint Parent object.  This is only intended
     * to be used within this library.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the GroPoint.
     * Use -1 if it is continuously powered.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.  An
     * RS485 adapter with integrated flow control is strongly recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param model The model of GroPoint sensor.
     * @param sensName The name of the sensor.  Defaults to "SDI12-Sensor".
     * @param numVariables The number of variable results returned by the
     * sensor. Defaults to 2.
     * @param warmUpTime_ms The time in ms between when the sensor is powered on
     * and when it is ready to receive a wake command.  Defaults to 1500.
     * @param stabilizationTime_ms The time in ms between when the sensor
     * receives a wake command and when it is able to return stable values.
     * Defaults to 20,000 (20s).
     * @param measurementTime_ms The time in ms between when a measurement is
     * started and when the result value is available.  Defaults to 2000.
     * @param incCalcValues The number of included calculated variables from the
     * sensor, if any.  These are used for values that we would always calculate
     * for a sensor and depend only on the raw results of that single sensor;
     * optional with a default value of 0.
     */
    GroPointParent(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2, int8_t enablePin = -1,
                   uint8_t       measurementsToAverage = 1,
                   gropointModel model                 = GPLPX,
                   const char*   sensName              = "GroPoint-Sensor",
                   uint8_t numVariables = 2, uint32_t warmUpTime_ms = 350,
                   uint32_t stabilizationTime_ms = 100,
                   uint32_t measurementTime_ms   = 200,
                   uint8_t  incCalcValues        = 0);
    /**
     * @copydoc GroPointParent::GroPointParent
     */
    GroPointParent(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2, int8_t enablePin = -1,
                   uint8_t       measurementsToAverage = 1,
                   gropointModel model                 = GPLPX,
                   const char*   sensName              = "GroPoint-Sensor",
                   uint8_t numVariables = 2, uint32_t warmUpTime_ms = 350,
                   uint32_t stabilizationTime_ms = 100,
                   uint32_t measurementTime_ms   = 200,
                   uint8_t  incCalcValues        = 0);
    /**
     * @brief Destroy the GroPoint Parent object - no action taken
     */
    virtual ~GroPointParent();

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
     * updates the #_sensorStatus.   No sensor power is required.  This will
     * always return true.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
    /**
     * @brief Wake the sensor up, if necessary.  Do whatever it takes to get a
     * sensor in the proper state to begin a measurement.
     *
     * Verifies that the power is on and updates the #_sensorStatus.  This also
     * sets the #_millisSensorActivated timestamp.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return **bool** True if the wake function completed successfully.
     */
    bool wake(void) override;
    /**
     * @brief Puts the sensor to sleep, if necessary.
     *
     * This also un-sets the #_millisSensorActivated timestamp (sets it to 0).
     * This does NOT power down the sensor!
     *
     * @return **bool** True if the sleep function completed successfully.
     */
    bool sleep(void) override;

    // Override these to use two power pins
    void powerUp(void) override;
    void powerDown(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    gropoint      _gsensor;
    gropointModel _model;
    byte          _modbusAddress;
    Stream*       _stream;
    int8_t        _RS485EnablePin;
    int8_t        _powerPin2;
};

#endif  // SRC_SENSORS_GROPOINTPARENT_H_
