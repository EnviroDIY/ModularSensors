/**
 * @file EnviroDIYPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the EnviroDIYPublisher subclass of dataPublisher for
 * publishing data to the Monitor My Watershed/EnviroDIY data portal at
 * http://data.enviroDIY.org
 */

// Header Guards
#ifndef SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_
#define SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

// Include the Monitor My Watershed Publisher since the EnviroDIYPublisher is
// only a typedef reference to it for backward compatibility
#include "MonitorMyWatershedPublisher.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
/**
 * @brief typedef for backwards compatibility; use the
 * MonitorMyWatershedPublisher class in new code
 *
 * @ingroup the_publishers
 *
 * @m_deprecated_since{0,38,0}
 */
typedef MonitorMyWatershedPublisher EnviroDIYPublisher;

#endif  // SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_
