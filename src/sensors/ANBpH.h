/**
 * @file ANBpH.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ANBpH sensor subclass for all ANB Sensors pH sensors and
 * the variable subclasses ANBpH_pH, ANBpH_Temp, ANBpH_Salinity, ANBpH_SpCond
 * and ANBpH_EC.
 *
 * Documentation for the ANB Sensors Modbus Protocol commands and responses,
 * along with information about the various variables, can be found in the
 * EnviroDIY ANB Sensors library at:
 * https://github.com/EnviroDIY/ANBSensorsModbus
 */
/**
 * @defgroup sensor_anb_ph ANB pH Sensors
 * The Sensor and Variable classes for all ANB pH sensors.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * This library currently supports all known [ANB
 * Sensors](https://www.anbsensors.com) pH sensors.
 *
 * The lower level details of the communication with the sensors is managed by
 * the [EnviroDIY ANBSensorsModbus
 * library](https://github.com/EnviroDIY/ANBSensorsModbus)
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_anb_ph_datasheet Sensor Datasheet
 * - [AQ5 and AQ50
 * Flyer](https://www.anbsensors.com/wp-content/uploads/2025/08/AQ-Flyer-Aug25.pdf)
 * - [User Guide](https://www.anbsensors.com/newdocs/docs/intro/)
 *
 * @section  sensor_anb_ph_power Power Requirements
 *
 * This sensor is a power hog. It draws ~90mA when measuring and ~60mA when
 * idle.  This doesn't seem so bad, until you consider that the sensor could
 * possibly take up to 4.25 **minutes** to take a sample after the first power
 * on.  For some ideas of how long your battery might last with this sensor, see
 * the description of the battery housing on the [product
 * page](https://www.anbsensors.com/products/), which includes a chart.
 *
 * Some recommendations:
 * - Get a beefy 12V battery and 18V solar panel - regardless of your
 * measurement interval.
 * - If you want measurements more often than every 5 minutes, leave the sensor
 * always powered
 *   - Size your battery and solar panel accordingly - it will be hungry!
 * - If you want measurements every 15 minutes or more, turn off power between
 * readings and be aware that the time of the measurement will be offset from
 * the time the logger woke or other sensors took measurements by the time it
 * takes the pH sensor to warm up and take a reading.
 *
 * @section sensor_anb_ph_ctor Sensor Constructor
 * {{ @ref ANBpH::ANBpH }}
 *
 * ___
 * @section sensor_anb_ph_examples Example Code
 * An ANB Sensors pH sensor is used in the @menulink{anb_ph} example.
 *
 * @menusnip{anb_ph}
 */

// Header Guards
#ifndef SRC_SENSORS_ANB_SENSORS_PH_H_
#define SRC_SENSORS_ANB_SENSORS_PH_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_ANB_SENSORS_PH_DEBUG
#define MS_DEBUGGING_STD "ANBpH"
#endif
#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "ANBpH"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "LoggerBase.h"
#include <ANBSensorsModbus.h>

/** @ingroup sensor_anb_ph */
/**@{*/

/**
 * @brief The minimum spacing between requesting responses from the sensor.
 *
 * This is used to prevent flooding the sensor with requests.
 */
#define ANB_PH_MINIMUM_REQUEST_SPACING 250

/**
 * @anchor sensor_anb_ph_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by an ANB Sensors pH sensor
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; an ANB Sensors pH sensor can report 8
/// values.
#define ANB_PH_NUM_VARIABLES 8
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ANB_PH_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_anb_ph_timing
 * @name Sensor Timing
 * The sensor timing for an ANB Sensors pH sensor
 *
 * The ANB pH sensor takes >3 minutes to complete the first sample after being
 * turned on, but ~6s to complete a subsequent measurement when in continuous
 * power and continuous sampling mode.  If the immersion sensor is enabled and a
 * measurement is requested while the sensor is not immersed, the failure
 * response can return in <4 seconds.
 *
 * More information on the timing can be found in the [sensor output section of
 * the ANB Sensors User
 * Guide](https://www.anbsensors.com/newdocs/docs/sensor-output)
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
 * roughly 5.3 - 5.6 seconds.
 *
 * This is the time for communication to begin.
 */
#define ANB_PH_WARM_UP_TIME_MS 5400L
/// @brief The maximum time to wait for a modbus response.
#define ANB_PH_WARM_UP_TIME_MAX 10000L

/// @brief Sensor::_stabilizationTime_ms; the ANB pH sensor does not need to
/// stabilize, but we use this time as the check for ready time.
#define ANB_PH_STABILIZATION_TIME_MS 50
/// @brief The maximum time to wait for ready to measure.
#define ANB_PH_STABILIZATION_TIME_MAX 5000L

/// @brief The minimum time for the first value in high salinity (documented min
/// time of 129s).
/// @note If the immersion sensor is enabled and the sensor is not immersed, a
/// failure response may be returned sooner
#define ANB_PH_1ST_VALUE_HIGH_SALT 120000L
/// @brief The maximum time for the first value in high salinity (documented max
/// time of 238s for a long interval delay + 10s).
#define ANB_PH_1ST_VALUE_HIGH_SALT_MAX 248000L
/// @brief The minimum time for the first value in low salinity (documented min
/// time is 184s, but I got responses at 160s).
/// @note If the immersion sensor is enabled and the sensor is not immersed, a
/// failure response may be returned sooner
#define ANB_PH_1ST_VALUE_LOW_SALT 159000L
/// @brief The maximum time for the first value in low salinity (documented max
/// time of 255s for a long interval delay + 10s).
#define ANB_PH_1ST_VALUE_LOW_SALT_MAX 265000L

/// @brief The minimum time for the 2nd or subsequent values in high
/// salinity (documented new output time of 10.5s)
/// @warning After the first reading, the sensor will *always* say the sensor is
/// ready!  But there will not be a **new** value available before this time.
#define ANB_PH_2ND_VALUE_HIGH_SALT 10600L
/// @brief The minimum time for the 2nd or subsequent values in low
/// salinity (documented new output time of 14s).
/// @warning After the first reading, the sensor will *always* say the sensor is
/// ready!  But there will not be a **new** value available before this time.
#define ANB_PH_2ND_VALUE_LOW_SALT 14100L

/**
 * @anchor sensor_anb_ph_ph
 * @name pH
 * The pH variable from an ANB Sensors pH sensor
 * - Range is 0 to 14 pH
 * - Accuracy is ± 0.1 pH in low salinity water, ± 0.2 pH in high salinity water
 *
 * @note If the pH output is 99.99, check the transducer health code for
 * instruction.
 *
 * {{ @ref ANBpH_pH::ANBpH_pH }}
 */
/**@{*/
/// @brief Decimals places in string representation; soil moisture should have 1
/// - resolution is 0.01.
#define ANB_PH_PH_RESOLUTION 2
/// @brief Sensor variable number; pH is stored in sensorValues[0].
#define ANB_PH_PH_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
#define ANB_PH_PH_VAR_NAME "pH"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
#define ANB_PH_PH_UNIT_NAME "pH"
/// @brief Default variable short code; "ANBpHpH"
#define ANB_PH_PH_DEFAULT_CODE "ANBpHpH"
/**@}*/

/**
 * @anchor sensor_anb_ph_temp
 * @name Temperature
 * The temperature variable from an ANB Sensors pH sensor
 * - Range is -5°C to +40°C
 *
 * {{ @ref ANBpH_Temp::ANBpH_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.01°C.
#define ANB_PH_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define ANB_PH_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define ANB_PH_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define ANB_PH_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "ANBpHTemp"
#define ANB_PH_TEMP_DEFAULT_CODE "ANBpHTemp"
/**@}*/

/**
 * @anchor sensor_anb_ph_salinity
 * @name Salinity
 * The salinity variable from an ANB pH sensor
 *
 * @note If the salinity output is 99.99 but the pH output is
 * OK, the salinity is out of range.
 * - Try changing your salinity setting
 * - If expected salinity is > 7ppt no salinity output is given
 *
 * @note If both the pH and salinity output is 99.99, check the
 * transducer health code for instruction.
 *
 * {{ @ref ANBpH_Salinity::ANBpH_Salinity }}
 */
/**@{*/
/// @brief Decimals places in string representation; salinity should have 2.
#define ANB_PH_SALINITY_RESOLUTION 2
/// @brief Sensor variable number; salinity is stored in sensorValues[2].
#define ANB_PH_SALINITY_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "salinity"
#define ANB_PH_SALINITY_VAR_NAME "salinity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "partPerThousand" (ppt or ppth)
#define ANB_PH_SALINITY_UNIT_NAME "partPerThousand"
/// @brief Default variable short code; "ANBpHSalinity"
#define ANB_PH_SALINITY_DEFAULT_CODE "ANBpHSalinity"
/**@}*/

/**
 * @anchor sensor_anb_ph_spcond
 * @name Specific Conductivity
 * The specific conductance variable from an ANB pH sensor
 *
 * @note The ANB pH sensor reports specific conductance in mS/cm not µS/cm.
 *
 * @note If the specific conductance output is 99.99 but the pH output is
 * OK, the salinity is out of range.
 * - Try changing your salinity setting
 * - If expected salinity is > 7ppt no salinity output is given
 *
 * @note If both the pH and specific conductance output is 99.99, check the
 * transducer health code for instruction.
 *
 * {{ @ref ANBpH_SpCond::ANBpH_SpCond }}
 */
/**@{*/
/// @brief Decimals places in string representation; specific conductance
/// should have 2.
#define ANB_PH_SPCOND_RESOLUTION 2
/// @brief Sensor variable number; specific conductance is stored in
/// sensorValues[3].
#define ANB_PH_SPCOND_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificConductance"
#define ANB_PH_SPCOND_VAR_NAME "specificConductance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "millisiemenPerCentimeter" (mS/cm)
#define ANB_PH_SPCOND_UNIT_NAME "millisiemenPerCentimeter"
/// @brief Default variable short code; "ANBpHSpCond"
#define ANB_PH_SPCOND_DEFAULT_CODE "ANBpHSpCond"
/**@}*/

/**
 * @anchor sensor_anb_ph_raw_cond
 * @name Electrical Conductivity
 * The raw electrical conductivity variable from an ANB pH sensor
 *
 * @note In their documentation ANB Sensors refers to this as "actual
 * conductivity."
 *
 * @note If the raw conductivity output is 99.99 but the pH output is
 * OK, the salinity is out of range.
 * - Try changing your salinity setting
 * - If expected salinity is > 7ppt no salinity output is given
 *
 * @note If both the pH and raw conductivity output is 99.99, check the
 * transducer health code for instruction.
 *
 * {{ @ref ANBpH_EC::ANBpH_EC }}
 */
/**@{*/
/// @brief Decimals places in string representation; raw electrical conductivity
/// should have 2.
#define ANB_PH_EC_RESOLUTION 3
/// @brief Sensor variable number; conductivity is stored in sensorValues[4].
#define ANB_PH_EC_VAR_NUM 4
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricalConductivity"
#define ANB_PH_EC_VAR_NAME "electricalConductivity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "millisiemenPerCentimeter" (mS/cm)
#define ANB_PH_EC_UNIT_NAME "millisiemenPerCentimeter"
/// @brief Default variable short code; "ANBpHEC"
#define ANB_PH_EC_DEFAULT_CODE "ANBpHEC"
/**@}*/

// clang-format off
/**
 * @anchor sensor_anb_ph_health
 * @name Transducer Health Code
 * The health code variable from a ANB Sensors pH sensor
 *
 * These codes indicate the health status of the sensor's transducer.
 *
 * | ANBHealthCode | Value | Explanation                            | Action |
 * | ------------- | ----- | -------------------------------------- |------- |
 * | OK            | 0     | Healthy Transducer                     | No action required |
 * | ABRADE_SOON   | 1     | Transducer will need abrading soon     | - Ensure salinity setting is correct<br>- Abrade the transducer based on scanning profile and access frequency;<br>if accessed daily, wait until the health number reaches 2 before abrading,<br>otherwise, abrade now for sensors not accessed for weeks/months |
 * | ABRADE_NOW    | 2     | Transducer needs abrading now          | - Ensure salinity setting is correct<br>- Abrade the transducer |
 * | REPLACE       | 3     | Transducer needs replacing             | - Ensure salinity setting is correct<br>- Replace the transducer (or transducer not immersed for sensors with serial numbers less than 300200) |
 * | NOT_IMMERSED  | 4     | Transducer is not immersed             | Immerse the sensor |
 * | NO_REFERENCE  | 5     | No valid reference tracker measurement | Please wait for the next measurement |
 * | NO_PH         | 6     | No valid pH measurement                | Please wait for the next measurement |
 * | UNKNOWN       | 255   | Unknown status                         | Reconnect with the sensor |
 *
 * [Transducer health details can be found here.](https://www.anbsensors.com/newdocs/docs/sensor-output#transducer-health)
 *
 * [Detailed maintenance guidelines including how to abrade the sensor can be found here.](https://www.anbsensors.com/newdocs/docs/transducer-maintenance/)
 *
 * {{ @ref ANBpH_HealthCode::ANBpH_HealthCode }}
 */
/**@{*/
// clang-format on
/// @brief Decimals places in string representation; the health code has 0.
#define ANB_PH_HEALTH_CODE_RESOLUTION 0
/// @brief Sensor variable number; health code is stored in sensorValues[4]
#define ANB_PH_HEALTH_CODE_VAR_NUM 5
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define ANB_PH_HEALTH_CODE_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define ANB_PH_HEALTH_CODE_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "ANBpHHealth"
#define ANB_PH_HEALTH_CODE_DEFAULT_CODE "ANBpHHealth"
/**@}*/

// clang-format off
/**
 * @anchor sensor_anb_ph_diagnostics
 * @name Diagnostics Code
 * The diagnostics code variable from a ANB Sensors pH sensor
 *
 * | ANBDiagnosticCode | Value | Explanation         | Action |
 * | ----------------- | ----- | ------------------- | ------ |
 * | OK                | 0     | Healthy Sensor      | None |
 * | BATTERY_ERROR     | 1     | Clock Battery Error | If there is no external power to the sensor the real time clock will not hold the programmed time<br>- If the sensor is powered, the time set and data streamed, this failure is not an issue<br>- If the sensor is in autonomous mode the clock will fail if disconnected from the power when it was programmed and placed on an external battery<br>-Users can record when the sensor was first switched on (where the sensor's time will be set to 0) and calculate the times externally, or, if this is not viable, contact support@anbsensors.com |
 * | SD_ERROR          | 2     | SD Card Error       | Either SD Card has been disabled or<br>A failing in the internal data save has occurred and no new data can be saved to the internal memory<br>- If the sensor is connected to an external communications system the sensor will continue to stream data, however no data will be saved in autonomous mode<br>- Please contact support@anbsensors.com |
 * | SYSTEM_ERROR      | 3     | System Error        | Contact support@anbsensors.com |
 * | UNKNOWN           | 255   | Unknown status      | Reconnect with the sensor |
 *
 * [Sensor diagnostic output details can be found here.](https://www.anbsensors.com/newdocs/docs/sensor-output#sensor-diagnostics)
 *
 * {{ @ref ANBpH_DiagnosticCode::ANBpH_DiagnosticCode }}
 */
/**@{*/
// clang-format on
/// @brief Decimals places in string representation; the diagnostic code has 0.
#define ANB_PH_DIAGNOSTIC_CODE_RESOLUTION 0
/// @brief Sensor variable number; diagnostic code is stored in sensorValues[4]
#define ANB_PH_DIAGNOSTIC_CODE_VAR_NUM 6
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define ANB_PH_DIAGNOSTIC_CODE_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define ANB_PH_DIAGNOSTIC_CODE_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "ANBpHDiagnostic"
#define ANB_PH_DIAGNOSTIC_CODE_DEFAULT_CODE "ANBpHDiagnostic"
/**@}*/

// clang-format off
/**
 * @anchor sensor_anb_ph_status
 * @name Status Code
 * The status code variable from a ANB Sensors pH sensor
 *
 * These codes indicate the current operational status of the sensor.
 *
 * | ANBStatusCode       | Value | Explanation                                                     |
 * | ------------------- | ----- | --------------------------------------------------------------- |
 * | SLEEPING            | 0     | Sensor is idle and/or following its pre-set interval routine    |
 * | INTERVAL_SCANNING   | 1     | pH will be updated when measurement completes                   |
 * | CONTINUOUS_SCANNING | 2     | pH will be automatically updated as per our continuous sequence |
 * | UNKNOWN             | 255   | Unknown status; no response from the sensor                     |
 *
 * [Sensor status code documentation is available here.](https://www.anbsensors.com/newdocs/docs/modbus#sensor-diagnostics)
 *
 * {{ @ref ANBpH_StatusCode::ANBpH_StatusCode }}
 */
/**@{*/
// clang-format on
/// @brief Decimals places in string representation; the error code has 0.
#define ANB_PH_STATUS_CODE_RESOLUTION 0
/// @brief Sensor variable number; error code is stored in sensorValues[4]
#define ANB_PH_STATUS_CODE_VAR_NUM 7
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define ANB_PH_STATUS_CODE_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define ANB_PH_STATUS_CODE_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "ANBpHStatus"
#define ANB_PH_STATUS_CODE_DEFAULT_CODE "ANBpHStatus"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the [ANB pH sensors](@ref sensor_anb_ph)
 *
 * @note For the ANB pH sensor, the sensor::_measurementTime_ms is the time of the 2nd or subsequent reading.
 * The time for the first reading after power on is variable and much longer.
 */
/* clang-format on */
class ANBpH : public Sensor {
 public:
    /**
     * @brief Construct a new ANB pH object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the ANB pH
     * sensor. Use -1 if it is continuously powered.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.
     * @note An RS485 adapter with integrated flow control is strongly
     * recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    ANBpH(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2,
          int8_t enablePin = -1, uint8_t measurementsToAverage = 1);
    /// @copydoc ANBpH::ANBpH(byte, Stream*, int8_t, int8_t, int8_t, uint8_t)
    ANBpH(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2,
          int8_t enablePin = -1, uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ANB pH object - no action taken
     */
    virtual ~ANBpH();

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
     * @return True if the setup was successful.
     */
    bool setup(void) override;
    bool wake(void) override;
    bool sleep(void) override;

    bool addSingleMeasurementResult(void) override;

    /**
     * @copydoc Sensor::isWarmedUp(bool debug)
     *
     * For the ANB pH sensor, this waits for both the power-on warm up and for a
     * valid response from the sensor to a Modbus command.
     *
     * @note The timing here is probably not very variable.
     */
    bool isWarmedUp(bool debug = false) override;

    /**
     * @brief Check whether or not enough time has passed between the sensor
     * responding to any modbus command to giving a valid status code - which
     * indicates that it's ready to take a measurement.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that enough time has passed that the sensor is
     * ready to take a measurement.
     *
     * @note The timing here is probably not very variable.
     */
    bool isStable(bool debug = false) override;

    /**
     * @brief Check whether or not the pH sensor has completed a measurement.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that the pH sensor has completed a measurement.
     *
     * @note We override the default function because the amount of time
     * required depends on the salinity, power "style" and the immersion sensor.
     */
    bool isMeasurementComplete(bool debug = false) override;

    /**
     * @brief Set the sensor salinity mode
     *
     * Change this value to any of the following valid values:
     * ANBSalinityMode::LOW_SALINITY, ANBSalinityMode::HIGH_SALINITY
     *
     * @note Before scanning set the expected salinity.
     *
     * @remark The salinity mode is set to low salinity by default.
     *
     * @param newSalinityMode The new salinity mode to use
     * @return True if the salinity mode was successfully set, false if not.
     */
    bool setSalinityMode(ANBSalinityMode newSalinityMode);
    /**
     * @brief Enable or disable the immersion sensor
     *
     * @note The new immersion sensor status (immersion rule) is effective
     * immediately.  When power cycled, the immersion sensor defaults to enabled
     * and the sensor goes into a low power mode.
     *
     * @remark The immersion sensor is enabled by default.
     *
     * @param enable True to enable the immersion sensor, false to disable
     * @return True if the immersion sensor status was successfully set, false
     * if not.
     */
    bool enableImmersionSensor(bool enable = true);

 private:
    /**
     * @brief Private reference to the ANB Sensors class for communication with
     * the ANB pH sensor.
     */
    anbSensor _anb_sensor;
    /**
     * @brief Private reference to the ANB pH sensor's modbus address
     */
    byte _modbusAddress;
    /**
     * @brief Private reference to the stream for communication with the
     * ANB pH sensor.
     */
    Stream* _stream;
    /**
     * @brief Private reference to the RS-485 adapter's flow direction control
     * pin.
     */
    int8_t _RS485EnablePin;
    /**
     * @brief Private reference to the salinity mode for the ANB pH sensor.
     * @remark The salinity mode is set to low salinity by default.
     */
    ANBSalinityMode _salinityMode = ANBSalinityMode::LOW_SALINITY;
    /**
     * @brief Private reference to whether or not the immersion sensor is
     * enabled.
     * @remark The immersion sensor is enabled by default.
     */
    bool _immersionSensorEnabled = true;
    /**
     * @brief The last time a Modbus command was sent.
     *
     * This is used to prevent flooding the sensor with requests.
     */
    uint32_t _lastModbusCommandTime = 0;
    /**
     * @brief Check whether or not enough time has passed between modbus
     * commands to the sensor to send another command.
     * @param checkReadyFxn A pointer to a function that checks whether or not
     * the sensor is ready to for whatever you need it to do.
     * @param spacing The minimum time spacing between commands; defaults to
     * #ANB_PH_MINIMUM_REQUEST_SPACING.
     * @param startTime The time to use as the start time to calculate the total
     * elapsed time in waiting; defaults to 0, which uses the last modbus
     * command time.  This is only used for debugging output.
     * @return True indicates that enough time has passed that another command
     * can be sent.
     */
    bool isSensorReady(bool (anbSensor::*checkReadyFxn)(),
                       uint32_t spacing   = ANB_PH_MINIMUM_REQUEST_SPACING,
                       uint32_t startTime = 0);

    /**
     * @brief Get the start of the estimated time window for a measurement to
     * complete based on the sensor's current configuration.
     * @return The start of the estimated time window for a measurement to
     * complete.
     */
    uint32_t getStartMeasurementWindow(void);
    /**
     * @brief Get the end of the estimated time window for a measurement to
     * complete based on the sensor's current configuration.
     * @return The end of the estimated time window for a measurement to
     * complete.
     */
    uint32_t getEndMeasurementWindow(void);
    /**
     * @brief Set the sensor's real time clock (RTC) to the current time.
     *
     * @note This requires that the logger has a valid time and that the
     * ANB pH sensor is powered and communicating.
     *
     * @warning The ANB pH sensor's RTC **does not** have an independent battery
     * backup - the clock will be reset every time the sensor loses power.
     *
     * @return True if the RTC was successfully set, false if not.
     */
    bool setSensorRTC(void);
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the [pH](@ref sensor_anb_ph_ph)
 * from an [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_pH : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_pH object.
     *
     * @param parentSense The parent ANBpH object providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHpH".
     */
    explicit ANBpH_pH(ANBpH* parentSense, const char* uuid = "",
                      const char* varCode = ANB_PH_PH_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_PH_VAR_NUM,
                   (uint8_t)ANB_PH_PH_RESOLUTION, ANB_PH_PH_VAR_NAME,
                   ANB_PH_PH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_pH object.
     *
     * @note This must be tied with a parent ANBpH before it can be
     * used.
     */
    ANBpH_pH()
        : Variable((uint8_t)ANB_PH_PH_VAR_NUM, (uint8_t)ANB_PH_PH_RESOLUTION,
                   ANB_PH_PH_VAR_NAME, ANB_PH_PH_UNIT_NAME,
                   ANB_PH_PH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_pH object - no action needed.
     */
    ~ANBpH_pH() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_anb_ph_temp) from a
 * [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_Temp : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_Temp object.
     *
     * @param parentSense The parent ANBpH object providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHTemp".
     */
    explicit ANBpH_Temp(ANBpH* parentSense, const char* uuid = "",
                        const char* varCode = ANB_PH_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_TEMP_VAR_NUM,
                   (uint8_t)ANB_PH_TEMP_RESOLUTION, ANB_PH_TEMP_VAR_NAME,
                   ANB_PH_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_Temp object.
     *
     * @note This must be tied with a parent ANBpH before it can be
     * used.
     */
    ANBpH_Temp()
        : Variable((uint8_t)ANB_PH_TEMP_VAR_NUM,
                   (uint8_t)ANB_PH_TEMP_RESOLUTION, ANB_PH_TEMP_VAR_NAME,
                   ANB_PH_TEMP_UNIT_NAME, ANB_PH_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_Temp object - no action needed.
     */
    ~ANBpH_Temp() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [salinity output](@ref sensor_anb_ph_salinity) from an
 * [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_Salinity : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_Salinity object.
     *
     * @param parentSense The parent ANBpH object providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHSalinity".
     */
    explicit ANBpH_Salinity(ANBpH* parentSense, const char* uuid = "",
                            const char* varCode = ANB_PH_SALINITY_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_SALINITY_VAR_NUM,
                   (uint8_t)ANB_PH_SALINITY_RESOLUTION,
                   ANB_PH_SALINITY_VAR_NAME, ANB_PH_SALINITY_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new ANBpH_Salinity object.
     *
     * @note This must be tied with a parent ANBpH object before it can be
     * used.
     */
    ANBpH_Salinity()
        : Variable((uint8_t)ANB_PH_SALINITY_VAR_NUM,
                   (uint8_t)ANB_PH_SALINITY_RESOLUTION,
                   ANB_PH_SALINITY_VAR_NAME, ANB_PH_SALINITY_UNIT_NAME,
                   ANB_PH_SALINITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_Salinity() object - no action
     * needed.
     */
    ~ANBpH_Salinity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [specific conductance output](@ref sensor_anb_ph_spcond) from a
 * [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_SpCond : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_SpCond object.
     *
     * @param parentSense The parent ANBpH object providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHSpCond".
     */
    explicit ANBpH_SpCond(ANBpH* parentSense, const char* uuid = "",
                          const char* varCode = ANB_PH_SPCOND_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_SPCOND_VAR_NUM,
                   (uint8_t)ANB_PH_SPCOND_RESOLUTION, ANB_PH_SPCOND_VAR_NAME,
                   ANB_PH_SPCOND_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_SpCond object.
     *
     * @note This must be tied with a parent ANBpH object before it can be used.
     */
    ANBpH_SpCond()
        : Variable((uint8_t)ANB_PH_SPCOND_VAR_NUM,
                   (uint8_t)ANB_PH_SPCOND_RESOLUTION, ANB_PH_SPCOND_VAR_NAME,
                   ANB_PH_SPCOND_UNIT_NAME, ANB_PH_SPCOND_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_SpCond object - no action needed.
     */
    ~ANBpH_SpCond() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw electrical conductivity output](@ref sensor_anb_ph_raw_cond)
 * from an [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_EC : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_EC object.
     *
     * @param parentSense The parent ANBpH object providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHEC".
     */
    explicit ANBpH_EC(ANBpH* parentSense, const char* uuid = "",
                      const char* varCode = ANB_PH_EC_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_EC_VAR_NUM,
                   (uint8_t)ANB_PH_EC_RESOLUTION, ANB_PH_EC_VAR_NAME,
                   ANB_PH_EC_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_EC object.
     *
     * @note This must be tied with a parent ANBpH object before it can be
     * used.
     */
    ANBpH_EC()
        : Variable((uint8_t)ANB_PH_EC_VAR_NUM, (uint8_t)ANB_PH_EC_RESOLUTION,
                   ANB_PH_EC_VAR_NAME, ANB_PH_EC_UNIT_NAME,
                   ANB_PH_EC_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_EC object - no action needed.
     */
    ~ANBpH_EC() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [health code output](@ref sensor_anb_ph_health) from a
 * an [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_HealthCode : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_HealthCode object.
     *
     * @param parentSense The parent ANBpH object providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHHealth".
     */
    explicit ANBpH_HealthCode(
        ANBpH* parentSense, const char* uuid = "",
        const char* varCode = ANB_PH_HEALTH_CODE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_HEALTH_CODE_VAR_NUM,
                   (uint8_t)ANB_PH_HEALTH_CODE_RESOLUTION,
                   ANB_PH_HEALTH_CODE_VAR_NAME, ANB_PH_HEALTH_CODE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_HealthCode object.
     *
     * @note This must be tied with a parent ANBpH object before it can be
     * used.
     */
    ANBpH_HealthCode()
        : Variable((uint8_t)ANB_PH_HEALTH_CODE_VAR_NUM,
                   (uint8_t)ANB_PH_HEALTH_CODE_RESOLUTION,
                   ANB_PH_HEALTH_CODE_VAR_NAME, ANB_PH_HEALTH_CODE_UNIT_NAME,
                   ANB_PH_HEALTH_CODE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_HealthCode object - no action
     * needed.
     */
    ~ANBpH_HealthCode() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [diagnostic code output](@ref sensor_anb_ph_diagnostics) from a
 * an [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_DiagnosticCode : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_DiagnosticCode object.
     *
     * @param parentSense The parent ANBpH object providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHDiagnostic".
     */
    explicit ANBpH_DiagnosticCode(
        ANBpH* parentSense, const char* uuid = "",
        const char* varCode = ANB_PH_DIAGNOSTIC_CODE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_DIAGNOSTIC_CODE_VAR_NUM,
                   (uint8_t)ANB_PH_DIAGNOSTIC_CODE_RESOLUTION,
                   ANB_PH_DIAGNOSTIC_CODE_VAR_NAME,
                   ANB_PH_DIAGNOSTIC_CODE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_DiagnosticCode object.
     *
     * @note This must be tied with a parent ANBpH object before it can be
     * used.
     */
    ANBpH_DiagnosticCode()
        : Variable((uint8_t)ANB_PH_DIAGNOSTIC_CODE_VAR_NUM,
                   (uint8_t)ANB_PH_DIAGNOSTIC_CODE_RESOLUTION,
                   ANB_PH_DIAGNOSTIC_CODE_VAR_NAME,
                   ANB_PH_DIAGNOSTIC_CODE_UNIT_NAME,
                   ANB_PH_DIAGNOSTIC_CODE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_DiagnosticCode object - no action
     * needed.
     */
    ~ANBpH_DiagnosticCode() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [status code output](@ref sensor_anb_ph_status) from a
 * an [ANB Sensors pH sensor](@ref sensor_anb_ph).
 */
/* clang-format on */
class ANBpH_StatusCode : public Variable {
 public:
    /**
     * @brief Construct a new ANBpH_StatusCode object.
     *
     * @param parentSense The parent ANBpH object providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ANBpHStatus".
     */
    explicit ANBpH_StatusCode(
        ANBpH* parentSense, const char* uuid = "",
        const char* varCode = ANB_PH_STATUS_CODE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANB_PH_STATUS_CODE_VAR_NUM,
                   (uint8_t)ANB_PH_STATUS_CODE_RESOLUTION,
                   ANB_PH_STATUS_CODE_VAR_NAME, ANB_PH_STATUS_CODE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new ANBpH_StatusCode object.
     *
     * @note This must be tied with a parent ANBpH object before it can be
     * used.
     */
    ANBpH_StatusCode()
        : Variable((uint8_t)ANB_PH_STATUS_CODE_VAR_NUM,
                   (uint8_t)ANB_PH_STATUS_CODE_RESOLUTION,
                   ANB_PH_STATUS_CODE_VAR_NAME, ANB_PH_STATUS_CODE_UNIT_NAME,
                   ANB_PH_STATUS_CODE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ANBpH_StatusCode object - no action
     * needed.
     */
    ~ANBpH_StatusCode() {}
};
/**@}*/
#endif  // SRC_SENSORS_ANB_SENSORS_PH_H_

// cSpell:ignore millisiemenPerCentimeter ppth SPCOND
