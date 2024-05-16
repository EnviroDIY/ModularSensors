/**
 * @file ModularSensors.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief A simple include file for the Arduino command line interface (CLI).
 */

// Header Guards
#ifndef SRC_MODULARSENSORS_H_
#define SRC_MODULARSENSORS_H_

/**
 * @brief The current library version number
 * https://semver.org/
 * Add hypen '-' and alpha number for a branches unique tracking number
 * A pre-release version will always be indicated as slightly ahead of the
 * EnviroDIY branch that it is based on.
 */
#define MODULAR_SENSORS_VERSION "0.35.0"

// To support interrupts
#include "ModSensorInterrupts.h"

// To get all of the base classes for ModularSensors, include LoggerBase.
// NOTE:  Individual sensor definitions must be included separately.
#include "LoggerBase.h"

#endif  // SRC_MODULARSENSORS_H_
