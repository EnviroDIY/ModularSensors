/**
 * @file NISTTime.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the NISTTime helper class for requesting and parsing time
 * from a NIST TIME protocol server.
 */

// Header Guards
#ifndef SRC_NISTTIME_H_
#define SRC_NISTTIME_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label for the debugger
#ifdef MS_LOGGERBASE_DEBUG
#define MS_DEBUGGING_STD "NISTTime"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugging label
#undef MS_DEBUGGING_STD

// Include other external dependencies
#include <Arduino.h>
#include <Client.h>
#include <EpochTime.h>

/**
 * @brief A helper class for using the NIST TIME protocol over an Arduino
 * Client.
 *
 * @note This is effectively a static class, with all static functions and a
 * deleted constructor.
 */
class NISTTime {
 public:
    /// This class contains only static functions and cannot be instantiated.
    NISTTime() = delete;

    /**
     * @brief Request the current time from a NIST TIME protocol server.
     *
     * The network connection used by the client must already be active. This
     * function opens and closes the TCP socket but does not connect or
     * disconnect the underlying network interface.
     *
     * @param nistClient An idle Arduino Client to use for the request.
     * @return Unix timestamp (seconds since January 1, 1970 UTC), or zero if
     * the request fails.
     */
    static uint32_t query(Client& nistClient);

    /**
     * @brief Wait until another NIST request is permitted and record the new
     * request time.
     *
     * This enforces the minimum spacing required by the NIST servers across
     * all clients using this helper.
     */
    static void waitForNextRequest();

    /**
     * @brief Convert a NIST TIME protocol response to a Unix timestamp.
     *
     * @param nistBytes Four network-order bytes containing the number of
     * seconds since January 1, 1900 UTC.
     * @return Unix timestamp (seconds since January 1, 1970 UTC), or zero if
     * the parsed timestamp is outside the configured sane range.
     */
    static uint32_t parseResponse(
        const byte nistBytes[NIST_TIME_PROTOCOL_BYTES]);

 private:
    /** @brief Time of the most recent NIST request, in milliseconds. */
    static uint32_t _lastRequest;
};

#endif  // SRC_NISTTIME_H_
