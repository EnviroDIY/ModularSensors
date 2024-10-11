/**
 * @file ClockSupport.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the epochStart class, which is a helper to convert between
 * various time types used by different processors.
 */
#include "ClockSupport.h"

#ifdef MS_CLOCKSUPPORT_DEBUG
// helper functions to convert between epoch starts
/**
 * @brief Gets a string name for the epoch
 *
 * @param epoch The epoch to get the name of
 * @return The name for the epoch
 */
String epochStart::printEpochName() {
    switch (_unixOffset) {
        case epochStart::unix_epoch: return "Unix";
        case epochStart::y2k_epoch: return "Y2K";
        case epochStart::gps_epoch: return "GPS";
        case epochStart::nist_epoch: return "NIST";
        default: return "???";
    }
}
/**
 * @brief Gets a string for the start date of the epoch
 *
 * @param epoch The epoch to get the starting date of
 * @return The starting date, in ISO8601
 */
String epochStart::printEpochStart() {
    switch (_unixOffset) {
        case epochStart::unix_epoch: return "1970-01-01T00:00:00Z";
        case epochStart::y2k_epoch: return "2000-01-01T00:00:00Z";
        case epochStart::gps_epoch: return "1970-01-01T00:00:00Z";
        case epochStart::nist_epoch: return "1900-01-01T00:00:00Z";
        default: return "???";
    }
}
#endif


// ===================================================================== //
// Public functions to access the clock in proper format and time zone
// ===================================================================== //


// Initialize the processor epoch
epochStart loggerClock::_core_epoch = epochStart::y2k_epoch;
// Initialize the static timezone
int8_t loggerClock::_rtcTimeZone = 0;


// Initialize the RTC
// Needed for static instances
// NOTE: The Sodaq DS3231 library externs the clock instance, so it's not needed
// here.
#if defined(MS_USE_RV8803)
RV8803 Logger::rtc;
#elif defined(MS_USE_RTC_ZERO)
RTCZero  Logger::zero_sleep_rtc;
#endif


// Constructor
loggerClock::loggerClock() {
    _core_epoch = getProcessorEpochStart();
}
// Destructor
loggerClock::~loggerClock() {}

// Sets the static timezone that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void loggerClock::setRTCTimeZone(int8_t timeZone) {
    _rtcTimeZone = timeZone;
// Some helpful prints for debugging
#ifdef STANDARD_SERIAL_OUTPUT
    const char* prtout1 = "RTC timezone is set to UTC";
    if (_rtcTimeZone == 0) {
        PRINTOUT(prtout1);
    } else if (_rtcTimeZone > 0) {
        PRINTOUT(prtout1, '+', _rtcTimeZone);
    } else {
        PRINTOUT(prtout1, _rtcTimeZone);
    }
#endif
}
int8_t loggerClock::getRTCTimeZone(void) {
    return loggerClock::_rtcTimeZone;
}

#if defined(MS_USE_RV8803)
uint32_t loggerClock::getNowUTCEpoch(epochStart epoch) {
    // uint32_t getEpoch(bool use1970sEpoch = false);
    // The use1970sEpoch works properly ONLY on AVR/8-bit boards!!
    //   - Setting use1970sEpoch to false returns the seconds from Jan 1, 2000.
    //   - Setting use1970sEpoch to true returns the seconds from Jan 1, 1970.
    // For 32-bit boards, you must set use1970sEpoch to FALSE to get the 1970's
    // epoch. See:
    // https://github.com/sparkfun/SparkFun_RV-8803_Arduino_Library/issues/29

    // Get the epoch - with the time zone subtracted (i.e. return UTC epoch)
    rtc.updateTime();
    MS_DEEP_DBG(F("Set use1970sEpoch to"), _core_epoch == epochStart::y2k_epoch,
                F("because the processor epoch is"),
                static_cast<uint32_t>(epoch));
    MS_DEEP_DBG(F("Raw returned timestamp:"),
                rtc.getEpoch(_core_epoch == epochStart::y2k_epoch));
    MS_DEEP_DBG(F("Returned timestamp adjusted by"),
                static_cast<uint32_t>(epoch), (":"),
                rtc.getEpoch(_core_epoch == epochStart::y2k_epoch) +
                    (epoch - epochStart::unix_epoch));
    return rtc.getEpoch(_core_epoch == epochStart::y2k_epoch) +
        (epoch - epochStart::unix_epoch);
}
void loggerClock::setNowUTCEpoch(uint32_t ts, epochStart epoch) {
    // bool setEpoch(uint32_t value, bool use1970sEpoch = false, int8_t
    // timeZoneQuarterHours = 0);
    // If timeZoneQuarterHours is non-zero, update RV8803_RAM. Add the zone to
    // the epoch before setting
    MS_DEEP_DBG(F("Raw input timestamp:"), ts);
    MS_DEEP_DBG(F("Input timestamp adjusted by"), static_cast<uint32_t>(epoch),
                (":"), ts + (epoch - epochStart::unix_epoch));
    rtc.setEpoch(ts + (epoch - epochStart::unix_epoch),
                 _core_epoch == epochStart::y2k_epoch);
}

#elif defined(MS_USE_DS3231)
uint32_t loggerClock::getNowUTCEpoch(epochStart epoch) {
    MS_DEEP_DBG(F("Raw returned timestamp:"), rtc.now().getEpoch());
    MS_DEEP_DBG(F("Returned timestamp adjusted by"),
                static_cast<uint32_t>(epoch), (":"),
                rtc.now().getEpoch() + (epoch - epochStart::unix_epoch));
    return rtc.now().getEpoch() + (epoch - epochStart::unix_epoch);
}
void loggerClock::setNowUTCEpoch(uint32_t ts, epochStart epoch) {
    MS_DEEP_DBG(F("Raw input timestamp:"), ts);
    MS_DEEP_DBG(F("Input timestamp adjusted by"), static_cast<uint32_t>(epoch),
                (":"), ts + (epoch - epochStart::unix_epoch));
    rtc.setEpoch(ts + (epoch - epochStart::unix_epoch));
}

#elif defined(MS_USE_RTC_ZERO)

uint32_t loggerClock::getNowUTCEpoch(epochStart epoch) {
    MS_DEEP_DBG(F("Raw returned timestamp:"), rtc.getEpoch());
    MS_DEEP_DBG(F("Returned timestamp adjusted by"),
                static_cast<uint32_t>(epoch), (":"),
                rtc.getEpoch() + (epoch - epochStart::unix_epoch));
    return zero_sleep_rtc.getEpoch() + (epoch - epochStart::unix_epoch);
}
void loggerClock::setNowUTCEpoch(uint32_t ts, epochStart epoch) {
    MS_DEEP_DBG(F("Raw input timestamp:"), ts);
    MS_DEEP_DBG(F("Input timestamp adjusted by"), static_cast<uint32_t>(epoch),
                (":"), ts + (epoch - epochStart::unix_epoch));
    zero_sleep_rtc.setEpoch(ts + (epoch - epochStart::unix_epoch));
}

#endif

// This converts an epoch time (unix time) into a ISO8601 formatted string.
// It assumes the supplied date/time is in the LOGGER's timezone and adds the
// LOGGER's offset as the time zone offset in the string.
// code modified from parts of the SparkFun RV-8803 library
String loggerClock::formatDateTime_ISO8601(uint32_t   epochTime,
                                           int8_t     loggerTimeZone,
                                           epochStart epoch) {
    MS_DEEP_DBG(F("Input epoch time:"), epochTime, F("; input epoch:"),
                (uint32_t)epoch);
    // Create a temporary variable for the epoch time
    // NOTE: for AVR boards time_t is a typedef for uint32_t, defined in time.h
    // For SAMD time_t is a typedef for __int_least64_t _timeval.h
    if (sizeof(time_t) != sizeof(uint32_t)) {
        MS_DEEP_DBG(F("THE TIME FORMAT IS NOT THE SAME SIZE AS A uint32_t!"),
                    sizeof(time_t), sizeof(uint32_t));
    }
    bool is_signed = (((time_t)(-1)) < 0);
    MS_DEEP_DBG(F("THE TIME FORMAT IS"),
                is_signed ? F("SIGNED") : F("UNSIGNED"));

    // implicit cast to time_t
    time_t t = epochTime;


    time_t     epoch_zero    = 0;
    struct tm* epoch_zero_tm = gmtime(&epoch_zero);
    // create a temporary buffer to put the timestamp into
    char epoch_zero_year[5];  // Max of yyyy with \0 terminator
    // use strftime (from time.h) to format the time
    strftime(epoch_zero_year, 5, "%Y", epoch_zero_tm);
    MS_DEEP_DBG(F("epoch_zero_year:"), epoch_zero_year);
    int zero_year = atoi(epoch_zero_year);
    MS_DEEP_DBG(F("zero_year:"), zero_year);

    epochStart ret_val;
    switch (zero_year) {
        default:
        case 1970: ret_val = epochStart::unix_epoch; break;
        case 2000: ret_val = epochStart::y2k_epoch; break;
        case 1980: ret_val = epochStart::gps_epoch; break;
        case 1900: ret_val = epochStart::nist_epoch; break;
    }
    MS_DEEP_DBG(F("ret_val:"), (uint32_t)ret_val);


    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp2 = gmtime(&t);
    MS_DEEP_DBG(F("Time components without adjustment: year:"), tmp2->tm_year,
                F("month:"), tmp2->tm_mon + 1, F("day:"), tmp2->tm_mday,
                F("hour:"), tmp2->tm_hour, F("minute:"), tmp2->tm_min,
                F("second:"), tmp2->tm_sec);
    // create a temporary buffer to put the timestamp into
    static char
        time8601tz2[20];  // Max of yyyy-mm-ddThh:mm:ss with \0 terminator
    // use strftime (from time.h) to format the time
    strftime(time8601tz2, 20, "%Y-%m-%dT%H:%M:%S", tmp2);
    MS_DEEP_DBG(F("Formatted time string:"), time8601tz2);


    MS_DEEP_DBG(F("Converted epoch time:"), t);
    if (epoch != _core_epoch) {
        t += epoch;        // convert to to unix
        t -= _core_epoch;  // convert to processor epoch (used by gmtime)
    }

    MS_DEEP_DBG(F("Adjusted epoch by"), static_cast<uint32_t>(epoch), (":"), t);
    MS_DEEP_DBG(F(" time:"), t);
    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&t);
    MS_DEEP_DBG(F("Time components: year:"), tmp->tm_year, F("month:"),
                tmp->tm_mon + 1, F("day:"), tmp->tm_mday, F("hour:"),
                tmp->tm_hour, F("minute:"), tmp->tm_min, F("second:"),
                tmp->tm_sec);

    // create a temporary buffer to put the timestamp into
    static char
        time8601tz[20];  // Max of yyyy-mm-ddThh:mm:ss with \0 terminator
    // use strftime (from time.h) to format the time
    strftime(time8601tz, 20, "%Y-%m-%dT%H:%M:%S", tmp);
    MS_DEEP_DBG(F("Formatted time string:"), time8601tz);

    // get the corrected timezone format
    // NOTE: the %z format from strftime formats the timezone as +hhmm, but we
    // need +hh:mm
    char   isotz[8];
    int8_t quarterHours = loggerTimeZone * 4;
    char   plusMinus    = '+';
    if (quarterHours < 0) {
        plusMinus = '-';
        quarterHours *= -1;
    }
    uint16_t tz_mins = quarterHours * 15;
    uint8_t  tzh     = tz_mins / 60;
    uint8_t  tzm     = tz_mins % 60;
    MS_DEEP_DBG(F("Time zone: plusMinus:"), plusMinus, F("tzh:"), tzh,
                F("tzm:"), tzm);
    snprintf(isotz, sizeof(isotz), "%c%02d:%02d", plusMinus, tzh, tzm);
    MS_DEEP_DBG(F("Formatted time zone string:"), isotz);

    return String(time8601tz) + String(isotz);
}


// This sets the real time clock to the given time
bool loggerClock::setRTClock(uint32_t UTCEpochSeconds, epochStart epoch) {
    // If the timestamp is zero, just exit
    if (UTCEpochSeconds == 0) {
        PRINTOUT(F("Bad timestamp, not setting clock."));
        return false;
    }

    // The "setTime" is the number of seconds since the start of the input epoch
    // We're interested in the setTime in the logger's and RTC's timezone
    // The RTC's timezone is equal to the logger's timezone minus the offset
    // between the logger and the RTC.
    uint32_t new_rtc_value = UTCEpochSeconds;

    // Check the current RTC time
    uint32_t prev_rtc_value = getNowUTCEpoch(epoch);
    MS_DBG(F("    Current Time on RTC:"), prev_rtc_value, F("->"),
           formatDateTime_ISO8601(prev_rtc_value, epoch));
    MS_DBG(F("    Offset between input and RTC:"),
           abs(new_rtc_value - prev_rtc_value));

    // NOTE:  Because we take the time to do some UTC/Local conversions and
    // print stuff out, the clock might end up being set up to a few
    // milliseconds behind the input time.  Given the clock is only accurate to
    // seconds (not milliseconds or less), I don't think this is a problem.

    // If the RTC and NIST disagree by more than 5 seconds, set the clock
    if (abs(new_rtc_value - prev_rtc_value) > 5) {
        setNowUTCEpoch(new_rtc_value, epoch);
        PRINTOUT(F("Clock set!"));
        return true;
    } else {
        PRINTOUT(F("Clock already within 5 seconds of time."));
        return false;
    }
}

// This checks that the logger time is within a "sane" range
bool loggerClock::isRTCSane(void) {
    uint32_t curRTC = getNowUTCEpoch(epochStart::unix_epoch);
    return isRTCSane(curRTC, epochStart::unix_epoch);
}
bool loggerClock::isRTCSane(uint32_t epochTime, epochStart epoch) {
    uint32_t epochTime2 = epochTime - (epoch - epochStart::unix_epoch);
    if (epochTime2 < EARLIEST_SANE_UNIX_TIMESTAMP ||
        epochTime2 > LATEST_SANE_UNIX_TIMESTAMP) {
        PRINTOUT(F("----- WARNING ----- !!!!!!!!!!!!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!! ----- WARNING ----- !!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!!!!!!!!!!!! ----- WARNING ----- \n"));
        PRINTOUT(F("The current clock timestamp is not valid!"));
        PRINTOUT(F("\n----- WARNING ----- !!!!!!!!!!!!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!! ----- WARNING ----- !!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!!!!!!!!!!!! ----- WARNING ----- "));
        return false;
    } else {
        return true;
    }
}

void loggerClock::begin() {
#if defined(MS_USE_RTC_ZERO)
    MS_DBG(F("Beginning internal real time clock"));
    zero_sleep_rtc.begin();
#endif
#if defined(MS_USE_RV8803)
    MS_DBG(F("Beginning RV-8803 real time clock"));
    rtc.begin();
    rtc.set24Hour();
    // void setTimeZoneQuarterHours(int8_t quarterHours);
    // Write the time zone to RV8803_RAM as int8_t (signed) in 15 minute
    // increments
    // This must happen here in the begin, not when setting the internal
    // timezone variable because this requires communication with the RTC which
    // can only happen during the run, not during compilation.
    rtc.setTimeZoneQuarterHours(
        (Logger::_loggerTimeZone - Logger::_loggerRTCOffset) * 4);
#elif defined(MS_USE_DS3231)
    MS_DBG(F("Beginning DS3231 real time clock"));
    rtc.begin();
#endif
    // Print out the current time
    PRINTOUT(F("Current RTC time is:"),
             formatDateTime_ISO8601(getNowUTCEpoch(epochStart::unix_epoch),
                                    _rtcTimeZone, epochStart::unix_epoch));
    MS_DBG(F("The processor is uses a"), _core_epoch.printEpochName(),
           F("epoch internally, which starts"), _core_epoch.printEpochStart(),
           F("and is offset from the Unix epoch by"),
           static_cast<uint32_t>(_core_epoch - epochStart::unix_epoch),
           F("seconds"));
}

// figure out where the epoch starts for the processor
// This is awkward, but I'm struggling to find any documentation on
// what the year component input should be for mktime  - and I'm pretty sure
// it varies across processors.  If both gmtime and strftime are time.h for
// the processor then this should work regardless of how the year is
// represented within the tm structs.
epochStart loggerClock::getProcessorEpochStart() {
    time_t     epoch_zero    = 0;
    struct tm* epoch_zero_tm = gmtime(&epoch_zero);
    // create a temporary buffer to put the timestamp into
    char epoch_zero_year[5];  // Max of yyyy with \0 terminator
    // use strftime (from time.h) to format the time
    strftime(epoch_zero_year, 5, "%Y", epoch_zero_tm);
    int zero_year = atoi(epoch_zero_year);

    epochStart ret_val;
    switch (zero_year) {
        default:
        case 1970: ret_val = epochStart::unix_epoch; break;
        case 2000: ret_val = epochStart::y2k_epoch; break;
        case 1980: ret_val = epochStart::gps_epoch; break;
        case 1900: ret_val = epochStart::nist_epoch; break;
    }
    return ret_val;
}
