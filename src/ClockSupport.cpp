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
#include "LoggerBase.h"

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
int8_t loggerClock::_rtcUTCOffset = 0;


#if defined(MS_USE_RV8803)
epochStart loggerClock::_rtcEpoch = epochStart::unix_epoch;
#elif defined(MS_USE_DS3231)
epochStart loggerClock::_rtcEpoch = epochStart::unix_epoch;
#elif defined(MS_USE_RTC_ZERO)
epochStart loggerClock::_rtcEpoch = epochStart::unix_epoch;
#endif


// Initialize the RTC
// Needed for static instances
// NOTE: The Sodaq DS3231 library externs the clock instance, so it's not needed
// here.
#if defined(MS_USE_RV8803)
RV8803 loggerClock::rtc;
#elif defined(MS_USE_RTC_ZERO)
RTCZero    loggerClock::zero_sleep_rtc;
#endif


// Sets the static offset from UTC that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC (ie, offset = 0)
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void loggerClock::setRTCOffset(int8_t offsetHours) {
    _rtcUTCOffset = offsetHours;
// Some helpful prints for debugging
#ifdef STANDARD_SERIAL_OUTPUT
    const char* prtout1 = "RTC is set to UTC";
    if (_rtcUTCOffset == 0) {
        PRINTOUT(prtout1);
    } else if (_rtcUTCOffset > 0) {
        PRINTOUT(prtout1, '+', _rtcUTCOffset);
    } else {
        PRINTOUT(prtout1, _rtcUTCOffset);
    }
#endif
}
int8_t loggerClock::getRTCOffset(void) {
    return loggerClock::_rtcUTCOffset;
}

uint32_t loggerClock::getNowAsEpoch(int8_t returnUTCOffset, epochStart epoch) {
    uint32_t rtc_return = loggerClock::getRawRTCNow();
    MS_DEEP_DBG(F("Raw returned timestamp:"), rtc_return);

    uint32_t epoch_change = epoch - loggerClock::_rtcEpoch;
    MS_DEEP_DBG(F("Adding"), epoch_change,
                F("to the timestamp to convert to the requested epoch."));

    // Do NOT apply an offset if the timestamp is obviously bad
    uint32_t tz_change = 0;
    if (isEpochTimeSane(rtc_return, _rtcUTCOffset, _rtcEpoch)) {
        tz_change = static_cast<uint32_t>(loggerClock::_rtcUTCOffset +
                                          returnUTCOffset) *
            3600;
        MS_DEEP_DBG(
            F("Adding"), tz_change,
            F("to the timestamp to convert to the requested timezone."));
    } else {
        MS_DEEP_DBG(
            F("Not converting timestamp to requested UTC offset because"),
            rtc_return, F("doesn't appear to be a valid timestamp"));
    }

    return rtc_return + tz_change + epoch_change;
}

// This converts an epoch time (seconds since a fixed epoch start) into a
// ISO8601 formatted string. It assumes the supplied date/time is in the
// LOGGER's timezone and adds the LOGGER's offset as the time zone offset in the
// string. code modified from parts of the SparkFun RV-8803 library
String loggerClock::formatDateTime_ISO8601(uint32_t   epochTime,
                                           int8_t     epochTimeUTCOffset,
                                           epochStart epoch) {
    MS_DEEP_DBG(F("Input epoch time:"), epochTime, F("; input epoch:"),
                static_cast<uint32_t>(epoch));

    // NOTE: for AVR boards time_t is a typedef for uint32_t, defined in
    // time.h For SAMD time_t is a typedef for __int_least64_t _timeval.h
    // implicit cast to time_t

    // Create a temporary variable for the epoch time
    time_t t = epochTime;
    MS_DEEP_DBG(F("Input time as a time_t:"), t);

    // Convert the time to the processor epoch.
    // This is only needed so the gmtime function will work - this is not
    // converting between offsets.
    if (epoch != _core_epoch) {
        t += epoch;        // convert to to unix
        t -= _core_epoch;  // convert to processor epoch (used by gmtime)
    }
    MS_DEEP_DBG(F("Input time converted to processor epoch:"), t);

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

    // Correct the timezone format
    // NOTE: the %z format from strftime formats the timezone as +hhmm, but we
    // need +hh:mm
    char   isotz[8];
    int8_t quarterHours = epochTimeUTCOffset * 4;
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
bool loggerClock::setRTClock(uint32_t ts, int8_t utcOffset, epochStart epoch) {
    MS_DEEP_DBG(F("Raw input timestamp:"), ts);

    // If the timestamp is not in the valid range, just exit
    if (!isEpochTimeSane(ts, utcOffset, epoch)) {
        PRINTOUT(F("Bad timestamp, not setting clock."));
        return false;
    }

    // The "setTime" is the number of seconds since the start of the input
    // epoch We're interested in the setTime in the logger's and RTC's
    // timezone The RTC's timezone is equal to the logger's timezone minus
    // the offset between the logger and the RTC.
    uint32_t new_rtc_value = ts;

    // Check the current RTC time
    uint32_t prev_rtc_value = getNowAsEpoch(utcOffset, epoch);
    MS_DBG(F("    Current Time on RTC:"), prev_rtc_value, F("->"),
           formatDateTime_ISO8601(prev_rtc_value, utcOffset, epoch));
    MS_DBG(F("    Offset between input and RTC (seconds):"),
           abs(new_rtc_value - prev_rtc_value));

    // NOTE:  Because we take the time to do some UTC/Local conversions and
    // print stuff out, the clock might end up being set up to a few
    // milliseconds behind the input time.  Given some of the supported clocks
    // are only accurate to seconds (not milliseconds or less), I don't think
    // this is a problem.

    // If the RTC is alredy within 5 seconds of the input time, just quit
    if (abs(new_rtc_value - prev_rtc_value) < 5) {
        PRINTOUT(F("Clock already within 5 seconds of time."));
        return false;
    }

    uint32_t tz_change =
        static_cast<uint32_t>(loggerClock::_rtcUTCOffset - utcOffset) * 3600;
    MS_DEEP_DBG(F("Subtracting"), tz_change,
                F("from the timestamp to convert to the RTC's UTC offset."));
    uint32_t epoch_change = epoch - loggerClock::_rtcEpoch;
    MS_DEEP_DBG(F("Subtracting"), epoch_change,
                F("from the timestamp to convert to the RTC epoch."));
    MS_DEEP_DBG(F("Setting raw RTC value to:"), ts - tz_change - epoch_change);
    loggerClock::setRawRTCNow(ts - tz_change - epoch_change);
    PRINTOUT(F("Clock set!"));
    return true;
}

// This checks that the logger time is within a "sane" range
bool loggerClock::isRTCSane(void) {
    uint32_t curRTC = getNowAsEpoch(0, epochStart::unix_epoch);
    bool     is_sane = isEpochTimeSane(curRTC, 0, epochStart::unix_epoch);
    if (!is_sane) {
        PRINTOUT(F("----- WARNING ----- !!!!!!!!!!!!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!! ----- WARNING ----- !!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!!!!!!!!!!!! ----- WARNING ----- \n"));
        PRINTOUT(F("The current clock timestamp is not valid!"));
        PRINTOUT(F("\n----- WARNING ----- !!!!!!!!!!!!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!! ----- WARNING ----- !!!!!!!!!!"));
        PRINTOUT(F("!!!!!!!!!!!!!!!!!!!! ----- WARNING ----- "));
    }
    return is_sane;
}
bool loggerClock::isEpochTimeSane(uint32_t ts, int8_t utcOffset,
                                  epochStart epoch) {
    uint32_t epochTime2 = ts - utcOffset - (epoch - epochStart::unix_epoch);
    if (epochTime2 < EARLIEST_SANE_UNIX_TIMESTAMP ||
        epochTime2 > LATEST_SANE_UNIX_TIMESTAMP) {
        return false;
    } else {
        return true;
    }
}


void loggerClock::enableRTCInterrupts() {
#if defined(MS_USE_RV8803)
    // Disable any previous interrupts
    rtc.disableAllInterrupts();
    // Clear all flags in case any interrupts have occurred.
    rtc.clearAllInterruptFlags();
    // Enable a periodic update for every minute
    rtc.setPeriodicTimeUpdateFrequency(TIME_UPDATE_1_MINUTE);
    // Enable the hardware interrupt
    rtc.enableHardwareInterrupt(UPDATE_INTERRUPT);

#elif defined(MS_USE_DS3231)

    // Unfortunately, because of the way the alarm on the DS3231 is set up, it
    // cannot interrupt on any frequencies other than every second, minute,
    // hour, day, or date.  We could set it to alarm hourly every 5 minutes past
    // the hour, but not every 5 minutes.  This is why we set the alarm for
    // every minute and use the checkInterval function.  This is a hardware
    // limitation of the DS3231; it is not due to the libraries or software.
    MS_DBG(F("Setting alarm on DS3231 RTC for every minute."));
    rtc.enableInterrupts(EveryMinute);

    // Clear the last interrupt flag in the RTC status register
    // The next timed interrupt will not be sent until this is cleared
    rtc.clearINTStatus();

#elif defined(MS_USE_RTC_ZERO)

    // Make sure interrupts are enabled for the clock
    NVIC_EnableIRQ(RTC_IRQn);       // enable RTC interrupt
    NVIC_SetPriority(RTC_IRQn, 0);  // highest priority

    // Alarms on the RTC built into the SAMD21 appear to be identical to those
    // in the DS3231.  See more notes below.
    // We're setting the alarm seconds to 59 and then seting it to go off
    // whenever the seconds match the 59.  I'm using 59 instead of 00
    // because there seems to be a bit of a wake-up delay
    MS_DBG(F("Setting alarm on SAMD built-in RTC for every minute."));
    zero_sleep_rtc.attachInterrupt(Logger::wakeISR);
    zero_sleep_rtc.setAlarmSeconds(59);
    zero_sleep_rtc.enableAlarm(zero_sleep_rtc.MATCH_SS);

#endif  // defined(MS_USE_RTC_ZERO)
}
void loggerClock::disableRTCInterrupts() {
    // Stop the clock from sending out any interrupts while we're awake.
    // There's no reason to waste thought on the clock interrupt if it
    // happens while the processor is awake and doing other things.
#if defined(MS_USE_RV8803)
    MS_DEEP_DBG(F("Unsetting the alarm on the RV-8803"));
    rtc.disableHardwareInterrupt(UPDATE_INTERRUPT);
#elif defined(MS_USE_DS3231)
    MS_DEEP_DBG(F("Unsetting the alarm on the DS2321"));
    rtc.disableInterrupts();
#elif defined(MS_USE_RTC_ZERO)
    MS_DEEP_DBG(F("Unsetting the alarm on the built in RTC"));
    zero_sleep_rtc.disableAlarm();
#endif
}

void loggerClock::begin() {
    MS_DBG(F("Getting the epoch the processor uses for gmtime"));
    _core_epoch = getProcessorEpochStart();
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
        (Logger::_loggerUTCOffset - Logger::_loggerRTCOffset) * 4);
#elif defined(MS_USE_DS3231)
    MS_DBG(F("Beginning DS3231 real time clock"));
    rtc.begin();
#endif
    // Print out the current time
    PRINTOUT(F("Current RTC time is:"),
             formatDateTime_ISO8601(getNowAsEpoch(_rtcUTCOffset, _rtcEpoch),
                                    _rtcUTCOffset, _rtcEpoch));
    MS_DBG(F("The processor is uses a"), _core_epoch.printEpochName(),
           F("epoch internally, which starts"), _core_epoch.printEpochStart(),
           F("and is offset from the Unix epoch by"),
           static_cast<uint32_t>(_core_epoch - epochStart::unix_epoch),
           F("seconds"));
    MS_DBG(F("The attached RTC uses a"), _rtcEpoch.printEpochName(),
           F("epoch internally, which starts"), _rtcEpoch.printEpochStart(),
           F("and is offset from the Unix epoch by"),
           static_cast<uint32_t>(_rtcEpoch - epochStart::unix_epoch),
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


#if defined(MS_USE_RV8803)
uint32_t loggerClock::getRawRTCNow() {
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
    return rtc.getEpoch(_core_epoch == epochStart::y2k_epoch);
}
void loggerClock::setRawRTCNow(uint32_t ts) {
    // bool setEpoch(uint32_t value, bool use1970sEpoch = false, int8_t
    // timeZoneQuarterHours = 0);
    // If timeZoneQuarterHours is non-zero, update RV8803_RAM. Add the zone to
    // the epoch before setting
    rtc.setEpoch(ts, _core_epoch == epochStart::y2k_epoch);
}

#elif defined(MS_USE_DS3231)
uint32_t loggerClock::getRawRTCNow() {
    return rtc.now().getEpoch();
}
void loggerClock::setRawRTCNow(uint32_t ts) {
    rtc.setEpoch(ts);
}

#elif defined(MS_USE_RTC_ZERO)

uint32_t loggerClock::getRawRTCNow() {
    return zero_sleep_rtc.getEpoch();
}
void loggerClock::setRawRTCNow(uint32_t ts) {
    zero_sleep_rtc.setEpoch(ts);
}

#endif
