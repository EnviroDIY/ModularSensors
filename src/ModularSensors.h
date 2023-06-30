/**
 * @file ModularSensors.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief A simple include file for the Arduino command line interface (CLI).s
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
#define MODULAR_SENSORS_VERSION "0.34.1"


// To get all of the base classes for ModularSensors, include LoggerBase.
// NOTE:  Individual sensor definitions must be included separately.
#include "LoggerBase.h"

#endif  // SRC_MODULARSENSORS_H_
