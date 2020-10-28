/**
 * @file YosemitechY4000.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY4000 sensor subclass and the variable
 * subclasses YosemitechY4000_DOmgL, YosemitechY4000_Turbidity,
 * YosemitechY4000_Cond, YosemitechY4000_pH, YosemitechY4000_Temp,
 * YosemitechY4000_ORP, YosemitechY4000_Chlorophyll, and YosemitechY4000_BGA.
 *
 * These are for the Yosemitech Y4000 multi-parameter sonde.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 *
 * More detailed infromation on each variable can be found in the documentation
 * for the individual sensor probes
 */
/* clang-format off */
/**
 * @defgroup y4000_group Yosemitech Y4000 Sonde
 * Classes for the Yosemitech Y4000 multi-parameter sonde.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section y4000_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde_UserManual_v2.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde-1.6-ModbusInstruction-en.pdf)
 *
 * @section y4000_sensor The Y4000 Sonde Sensor
 * @ctor_doc{YosemitechY4000, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y4000_timing Sensor Timing
 * - Time before sensor responds after power - 275-300ms (use 350 for safety)
 * - Time between "StartMeasurement" command and stable reading depends on the
 * indindividual sensor probes, with the Y520 conductivity sensor taking the
 * longest.
 *
 * @section y4000_domgl Dissolved Oxygen Concentration Output
 *   - Range is 0-20mg/L or 0-200% Air Saturation
 *   - Accuracy is ± 0.3 mg/L
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.01 mg/L
 *   - Reported as milligrams per liter (mg/L)
 *   - Default variable code is Y4000DOmgL
 *
 * @variabledoc{y4000_domgl,YosemitechY4000,DOmgL,Y4000DOmgL}
 *
 * @section y4000_turb Turbidity Output
 *   - Range is 0.1~1000 NTU
 *   - Accuracy is ＜5% or 0.3NTU
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.01 NTU
 *   - Reported as Nephelometric Turbidity Units (NTU)
 *   - Default variable code is Y4000Turbidity
 *
 * @variabledoc{y4000_turb,YosemitechY4000,Turbidity,Y4000Turbidity}
 *
 * @section y4000_cond Conductivity Output
 *   - Range is 1 µS/cm to 200 mS/cm
 *   - Accuracy is ± 1 % Full Scale
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.1 µS/cm
 *   - Reported as microsiemens per centimeter (µS/cm)
 *   - Default variable code is Y4000Cond
 *
 * @variabledoc{y4000_cond,YosemitechY4000,Cond,Y4000Cond}
 *
 * @section y4000_ph pH Output
 *   - Range is 2 to 12 pH units
 *   - Accuracy is ± 0.1 pH units
 *   - Result stored in sensorValues[3]
 *   - Resolution is 0.01 pH units
 *   - Reported as dimensionless pH units
 *   - Default variable code is Y4000pH
 *
 * @variabledoc{y4000_ph,YosemitechY4000,pH,Y4000pH}
 *
 * @section y4000_temp Temperature Output
 *   - Range is 0°C to + 50°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[4]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y4000Temp
 *
 * @variabledoc{y4000_temp,YosemitechY4000,Temp,Y4000Temp}
 *
 * @section y4000_orp ORP Output
 *   - Range is -999 ~ 999 mV
 *   - Accuracy is ± 20 mV
 *   - Result stored in sensorValues[5]
 *   - Resolution is 1 mV
 *   - Reported as millivolts (mV)
 *   - Default variable code is Y4000Potential
 *
 * @variabledoc{y4000_orp,YosemitechY4000,ORP,Y4000Potential}
 *
 * @section y4000_bga BGA Output
 *   - Range is 0 to 100 µg/L or 0 to 100 RFU
 *   - Accuracy is ±  0.04ug/L PC
 *   - Result stored in sensorValues[7]
 *   - Resolution is 0.01 µg/L / 0.01 RFU
 *   - Reported as micrograms per liter (µg/L)
 *   - Default variable code is Y4000BGA
 *
 * @variabledoc{y4000_bga,YosemitechY4000,BGA,Y4000BGA}
 *
 * @section y4000_chloro Chlorophyll Output
 *   - Range is 0 to 400 µg/L or 0 to 100 RFU
 *   - Accuracy is ± 1 %
 *   - Result stored in sensorValues[6]
 *   - Resolution is 0.1 µg/L / 0.1 RFU
 *   - Reported as micrograms per liter (µg/L)
 *   - Default variable code is Y4000Chloro
 *
 * @variabledoc{y4000_chloro,YosemitechY4000,Chlorophyll,Y4000Chloro}
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y4000_examples Example Code
 * The Y4000 Sonde is used in the @menulink{y4000} example.
 *
 * @menusnip{y4000}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY4000_H_
#define SRC_SENSORS_YOSEMITECHY4000_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Y4000 can report 8 values.
#define Y4000_NUM_VARIABLES 8
/**
 * @brief Sensor::_warmUpTime_ms; the Y4000 warms up in 1600ms.
 *
 * This is the time for communication to begin.
 */
#define Y4000_WARM_UP_TIME_MS 1600
/**
 * @brief  Sensor::_stabilizationTime_ms; the Y4000 is stable after 60000ms.
 *
 * Y4000 Modbus manual says 60s; we find Cond takes about that long to respond.
 */
#define Y4000_STABILIZATION_TIME_MS 60000
/**
 * @brief Sensor::_measurementTime_ms; the Y4000 takes 3000ms to complete a
 * measurement.
 */
#define Y4000_MEASUREMENT_TIME_MS 3000

/**
 * @brief Decimals places in string representation; dissolved oxygen
 * concentration should have 2.
 */
#define Y4000_DOMGL_RESOLUTION 2
/// Variable number; dissolved oxygen concentration is stored in sensorValues[0]
#define Y4000_DOMGL_VAR_NUM 0

/// Decimals places in string representation; turbidity should have 2.
#define Y4000_TURB_RESOLUTION 2
/// Variable number; turbidity is stored in sensorValues[1].
#define Y4000_TURB_VAR_NUM 1

/// Decimals places in string representation; conductivity should have 1.
#define Y4000_COND_RESOLUTION 1
/// Variable number; conductivity is stored in sensorValues[2].
#define Y4000_COND_VAR_NUM 2

/// Decimals places in string representation; pH should have 2.
#define Y4000_PH_RESOLUTION 2
/// Variable number; pH is stored in sensorValues[3].
#define Y4000_PH_VAR_NUM 3

/// Decimals places in string representation; temperature should have 1.
#define Y4000_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[4].
#define Y4000_TEMP_VAR_NUM 4

/// Decimals places in string representation; ORP should have 0.
#define Y4000_ORP_RESOLUTION 0
/// Variable number; ORP is stored in sensorValues[5].
#define Y4000_ORP_VAR_NUM 5

/**
 * @brief Decimals places in string representation; chlorophyll concentration
 * should have 1.
 */
#define Y4000_CHLORO_RESOLUTION 1
/// Variable number; chlorophyll concentration is stored in sensorValues[6].
#define Y4000_CHLORO_VAR_NUM 6

/// Decimals places in string representation; BGA should have 2.
#define Y4000_BGA_RESOLUTION 2
/// Variable number; BGA is stored in sensorValues[7].
#define Y4000_BGA_VAR_NUM 7


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y4000 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y4000.
     * Use -1 if it is continuously powered.
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
    YosemitechY4000(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y4000,
                           "YosemitechY4000", Y4000_NUM_VARIABLES,
                           Y4000_WARM_UP_TIME_MS, Y4000_STABILIZATION_TIME_MS,
                           Y4000_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY4000::YosemitechY4000
     */
    YosemitechY4000(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y4000,
                           "YosemitechY4000", Y4000_NUM_VARIABLES,
                           Y4000_WARM_UP_TIME_MS, Y4000_STABILIZATION_TIME_MS,
                           Y4000_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y4000 object
     */
    ~YosemitechY4000() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref y4000_domgl) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_DOmgL object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000DOmgL".
     */
    explicit YosemitechY4000_DOmgL(YosemitechY4000* parentSense,
                                   const char*      uuid    = "",
                                   const char*      varCode = "Y4000DOmgL")
        : Variable(parentSense, (const uint8_t)Y4000_DOMGL_VAR_NUM,
                   (uint8_t)Y4000_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_DOmgL object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_DOmgL()
        : Variable((const uint8_t)Y4000_DOMGL_VAR_NUM,
                   (uint8_t)Y4000_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", "Y4000DOmgL") {}
    /**
     * @brief Destroy the YosemitechY4000_DOmgL object - no action needed.
     */
    ~YosemitechY4000_DOmgL() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref y4000_turb) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Turbidity object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Turbidity".
     */
    explicit YosemitechY4000_Turbidity(YosemitechY4000* parentSense,
                                       const char*      uuid = "",
                                       const char* varCode   = "Y4000Turbidity")
        : Variable(parentSense, (const uint8_t)Y4000_TURB_VAR_NUM,
                   (uint8_t)Y4000_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Turbidity()
        : Variable((const uint8_t)Y4000_TURB_VAR_NUM,
                   (uint8_t)Y4000_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", "Y4000Turbidity") {}
    /**
     * @brief Destroy the YosemitechY4000_Turbidity object - no action needed.
     */
    ~YosemitechY4000_Turbidity() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref y4000_cond) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Cond : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Cond object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Cond".
     */
    explicit YosemitechY4000_Cond(YosemitechY4000* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "Y4000Cond")
        : Variable(parentSense, (const uint8_t)Y4000_COND_VAR_NUM,
                   (uint8_t)Y4000_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Cond object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Cond()
        : Variable((const uint8_t)Y4000_COND_VAR_NUM,
                   (uint8_t)Y4000_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", "Y4000Cond") {}
    /**
     * @brief Destroy the YosemitechY4000_Cond object - no action needed.
     */
    ~YosemitechY4000_Cond() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref y4000_ph)
 * from a [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_pH object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000pH".
     */
    explicit YosemitechY4000_pH(YosemitechY4000* parentSense,
                                const char*      uuid    = "",
                                const char*      varCode = "Y4000pH")
        : Variable(parentSense, (const uint8_t)Y4000_PH_VAR_NUM,
                   (uint8_t)Y4000_PH_RESOLUTION, "pH", "pH", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_pH object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_pH()
        : Variable((const uint8_t)Y4000_PH_VAR_NUM,
                   (uint8_t)Y4000_PH_RESOLUTION, "pH", "pH", "Y4000pH") {}
    /**
     * @brief Destroy the YosemitechY4000_pH object - no action needed.
     */
    ~YosemitechY4000_pH() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y4000_temp) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Temp object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Temp".
     */
    explicit YosemitechY4000_Temp(YosemitechY4000* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "Y4000Temp")
        : Variable(parentSense, (const uint8_t)Y4000_TEMP_VAR_NUM,
                   (uint8_t)Y4000_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Temp object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Temp()
        : Variable((const uint8_t)Y4000_TEMP_VAR_NUM,
                   (uint8_t)Y4000_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y4000Temp") {}
    /**
     * @brief Destroy the YosemitechY4000_Temp object - no action needed.
     */
    ~YosemitechY4000_Temp() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [electrode electrical potential output](@ref y4000_orp) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_ORP : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_ORP object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Potential".
     */
    explicit YosemitechY4000_ORP(YosemitechY4000* parentSense,
                                 const char*      uuid    = "",
                                 const char*      varCode = "Y4000Potential")
        : Variable(parentSense, (const uint8_t)Y4000_ORP_VAR_NUM,
                   (uint8_t)Y4000_ORP_RESOLUTION, "ORP", "millivolt", varCode,
                   uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_ORP object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_ORP()
        : Variable((const uint8_t)Y4000_ORP_VAR_NUM,
                   (uint8_t)Y4000_ORP_RESOLUTION, "ORP", "millivolt",
                   "Y4000Potential") {}
    /**
     * @brief Destroy the YosemitechY4000_ORP object - no action needed.
     */
    ~YosemitechY4000_ORP() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [chlorophyll concentration output](@ref y4000_chloro) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group)
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Chlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Chlorophyll object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Chloro".
     */
    explicit YosemitechY4000_Chlorophyll(YosemitechY4000* parentSense,
                                         const char*      uuid = "",
                                         const char* varCode   = "Y4000Chloro")
        : Variable(parentSense, (const uint8_t)Y4000_CHLORO_VAR_NUM,
                   (uint8_t)Y4000_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Chlorophyll object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Chlorophyll()
        : Variable((const uint8_t)Y4000_CHLORO_VAR_NUM,
                   (uint8_t)Y4000_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", "Y4000Chloro") {}
    /**
     * @brief Destroy the YosemitechY4000_Chlorophyll() object - no action
     * needed.
     */
    ~YosemitechY4000_Chlorophyll() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [blue green algae (BGA) concentration output](@ref y4000_bga) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_BGA : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_BGA object.
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of Y4000BGA
     */
    explicit YosemitechY4000_BGA(YosemitechY4000* parentSense,
                                 const char*      uuid    = "",
                                 const char*      varCode = "Y4000BGA")
        : Variable(parentSense, (const uint8_t)Y4000_BGA_VAR_NUM,
                   (uint8_t)Y4000_BGA_RESOLUTION,
                   "blueGreenAlgaeCyanobacteriaPhycocyanin",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_BGA object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_BGA()
        : Variable((const uint8_t)Y4000_BGA_VAR_NUM,
                   (uint8_t)Y4000_BGA_RESOLUTION,
                   "blueGreenAlgaeCyanobacteriaPhycocyanin",
                   "microgramPerLiter", "Y4000BGA") {}
    /**
     * @brief Destroy the YosemitechY4000_BGA object - no action needed.
     */
    ~YosemitechY4000_BGA() {}
};


#endif  // SRC_SENSORS_YOSEMITECHY4000_H_
