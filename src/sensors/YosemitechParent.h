/**
 * @file YosemitechParent.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechParent sensor subclass, itself a parent class
 * for all Yosemitech sensors that communicate via Modbus and are supported by
 * the EnviroDIY Yosemitech library.
 *
 * Documentation for the Yosemitech Protocol commands and responses, along with
 * information about the various variables, can be found in the EnviroDIY
 * Yosemitech library at: https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup yosemitech_group Yosemitech Sensors
 * The Sensor and Variable objects for all Yosemitech sensors.
 *
 * @ingroup the_sensors
 *
 *
 * This library currently supports the following [Yosemitech](http://en.yosemitech.com/) sensors:
 * - [Y502-A or Y504-A Optical Dissolved Oxygen Sensors](http://en.yosemitech.com/aspcms/product/2021-3-1/161.html)
 *     - [Y504 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y504-DO_UserManual-v1.1.pdf)
 *     - [Y505 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y505-DO_UserManual-v1.2.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y504-DO-v6.2_ModbusInstructions.pdf)
 *     - [Class Documentation](@ref sensor_y504)
 * - [Y510-B Optical Turbidity Sensor](http://en.yosemitech.com/aspcms/product/2020-5-8/76.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y510-Turbidity_UserManual-v1.1.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y510-Turbidity_1.7-ModbusInstruction-en.pdf)
 *     - [Class Documentation](@ref sensor_y510)
 * - [Y511-A Optical Turbidity Sensor with Wiper](http://en.yosemitech.com/aspcms/product/2020-5-8/76.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y511-Turbidity+Wiper_UserManual-v1.1.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y511-Turbidity+Wiper-v1.7_ModbusInstructions.pdf)
 *     - [Class Documentation](@ref sensor_y511)
 * - [Y514-A Chlorophyll Sensor with Wiper](http://en.yosemitech.com/aspcms/product/2020-4-23/39.html)
 *     - [Y514 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y514-Chlorophyl+Wiper_UserManual-v1.0.pdf)
 *     - [Y515 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y515-Chlorophyll_UserManual-v1.0_en.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y514-Chlorophyl+Wiper-v1.6_ModbusInstructions.pdf)
 *     - [Class Documentation](@ref sensor_y514)
 * - [Y520-A or Y521-A 4-Electrode Conductivity Sensor](http://en.yosemitech.com/aspcms/product/2020-4-23/58.html)
 *     - [Y520 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y520-Conductivity_UserManual-v1.1.pdf)
 *     - [Y521 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y521-Conductivity_UserManual-v1.1.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y520-Conductivity-v1.8_ModbusInstructions.pdf)
 *     - [Class Documentation](@ref sensor_y520)
 * - [Y532-A Digital pH Sensor](http://en.yosemitech.com/aspcms/product/2020-6-15/154.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH_UserManual-v1.0.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH-ORP-v1.7_ModbusInstructions.pdf)
 *     - [Class Documentation](@ref sensor_y532)
 * - [Y533 ORP Sensor](http://en.yosemitech.com/aspcms/product/2020-5-8/91.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH_UserManual-v1.0.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH-ORP-v1.7_ModbusInstructions.pdf)
 *     - [Class Documentation](@ref sensor_y533)
 * - [Y551 COD/UV254 Sensor with Wiper](http://en.yosemitech.com/aspcms/product/2020-5-8/94.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y551-UV254-COD_UserManual_v1.0.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y551-UV254-COD_Modbus_v2020-05-11.pdf)
 *     - [Class Documentation](@ref sensor_y551)
 * - [Y560 Ammoinum Probe with Wiper](http://en.yosemitech.com/aspcms/product/2020-4-23/61.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y560-NH4_UserManual_v1.0.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y560-NH4_Modbus_v2020-05-11.pdf)
 *     - [Class Documentation](@ref sensor_y560)
 * - Y700 Pressure Sensor
 *     - [Class Documentation](@ref sensor_y700)
 * - [Y4000 Multiparameter Sonde](http://en.yosemitech.com/aspcms/product/2020-5-8/95.html)
 *     - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde_UserManual_v2.0.pdf)
 *     - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde-1.6-ModbusInstruction-en.pdf)
 *     - [Class Documentation](@ref sensor_y4000)
 *
 * Most of these sensors require a 9-12V power supply, but some can opperate as
 * low as 5V and sondes (Y560 & Y4000) require 12V. The power supply can be stopped between measurements for all.
 * (_Note that any user settings (such as brushing frequency) will be lost if the sensor loses power._)
 * They communicate via [Modbus RTU](https://en.wikipedia.org/wiki/Modbus) over [RS-485](https://en.wikipedia.org/wiki/RS-485).
 * To interface with them, you will need an RS485-to-TTL adapter.
 * The white wire of the Yosemitech sensor will connect to the "B" pin of the adapter and the green wire will connect to "A".
 * The red wire from the sensor should connect to the 5-12V power supply and the black to ground.
 * The Vcc pin on the adapter should be connected to another power supply (voltage depends on the specific adapter) and the ground to the same ground.
 * The red wire from the sensor _does not_ connect to the Vcc of the adapter.
 * The R/RO/RXD pin from the adapter connects to the TXD on the Arduino board and the D/DI/TXD pin from the adapter connects to the RXD.
 * If applicable, tie the RE and DE (receive/data enable) pins together and connect them to another pin on your board.
 * While this library supports an external enable pin, _we have had very bad luck with most of them and recommend against them_.
 * Adapters with automatic direction control tend to use very slightly more power, but have more stable communication.
 * There are a number of RS485-to-TTL adapters available.
 * When shopping for one, be mindful of the logic level of the TTL output by the adapter.
 * The MAX485, one of the most popular adapters, has a 5V logic level in the TTL signal.
 * This will _fry_ any board like the Mayfly that uses 3.3V logic.
 * You would need a voltage shifter in between the Mayfly and the MAX485 to make it work.
 *
 * The sensor constructor requires as input: the sensor modbus address, a stream instance for data (ie, ```Serial```), and one or two power pins.
 * The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.
 * (Use -1 for the second power pin and -1 for the enable pin if these don't apply and you want to average more than one reading.)
 * For all of these sensors except pH, Yosemitech strongly recommends averaging 10 readings for each measurement.
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
 * [EnviroDIY Yosemitech library](https://github.com/EnviroDIY/YosemitechModbus)
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHPARENT_H_
#define SRC_SENSORS_YOSEMITECHPARENT_H_

// Debugging Statement
// #define MS_YOSEMITECHPARENT_DEBUG
// #define MS_YOSEMITECHPARENT_DEBUG_DEEP

#ifdef MS_YOSEMITECHPARENT_DEBUG
#define MS_DEBUGGING_STD "YosemitechParent"
#endif

#ifdef MS_YOSEMITECHPARENT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "YosemitechParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"
#include "YosemitechModbus.h"

/* clang-format off */
/**
 * @brief The Sensor sub-class for all the [Yosemitech sensors](@ref yosemitech_group)
 *
 * @ingroup yosemitech_group
 */
/* clang-format on */
class YosemitechParent : public Sensor {
 public:
    /**
     * @brief Construct a new Yosemitech Parent object.  This is only intended
     * to be used within this library.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Yosemitech.
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
     * @param model The model of Yosemitech sensor.
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
    YosemitechParent(byte modbusAddress, Stream* stream, int8_t powerPin,
                     int8_t powerPin2, int8_t enablePin = -1,
                     uint8_t         measurementsToAverage = 1,
                     yosemitechModel model                 = UNKNOWN,
                     const char*     sensName = "Yosemitech-Sensor",
                     uint8_t numVariables = 2, uint32_t warmUpTime_ms = 1500,
                     uint32_t stabilizationTime_ms = 20000,
                     uint32_t measurementTime_ms   = 2000,
                     uint8_t  incCalcValues        = 0);
    /**
     * @copydoc YosemitechParent::YosemitechParent
     */
    YosemitechParent(byte modbusAddress, Stream& stream, int8_t powerPin,
                     int8_t powerPin2, int8_t enablePin = -1,
                     uint8_t         measurementsToAverage = 1,
                     yosemitechModel model                 = UNKNOWN,
                     const char*     sensName = "Yosemitech-Sensor",
                     uint8_t numVariables = 2, uint32_t warmUpTime_ms = 1500,
                     uint32_t stabilizationTime_ms = 20000,
                     uint32_t measurementTime_ms   = 2000,
                     uint8_t  incCalcValues        = 0);
    /**
     * @brief Destroy the Yosemitech Parent object - no action taken
     */
    virtual ~YosemitechParent();

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
    yosemitech      _ysensor;
    yosemitechModel _model;
    byte            _modbusAddress;
    Stream*         _stream;
    int8_t          _RS485EnablePin;
    int8_t          _powerPin2;
};

#endif  // SRC_SENSORS_YOSEMITECHPARENT_H_
