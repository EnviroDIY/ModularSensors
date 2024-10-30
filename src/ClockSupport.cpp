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


// Configure the epoch used internally by the RTC
// This depends on **the underlying RTC library**, not necessarily what is in
// the RTC's datasheet.
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

uint32_t loggerClock::getNowAsEpoch(int8_t utcOffset, epochStart epoch) {
    uint32_t rtc_return = getRawRTCNow();
    MS_DEEP_DBG(F("Raw returned timestamp:"), rtc_return);
    return tsFromRawRTC(rtc_return, utcOffset, epoch);
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
    MS_DEEP_DBG(F("Time components: "), tmp->tm_year, F(" - "), tmp->tm_mon + 1,
                F(" - "), tmp->tm_mday, F("    "), tmp->tm_hour, F(" : "),
                tmp->tm_min, F(" : "), tmp->tm_sec);

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

    uint32_t converted_ts = tsToRawRTC(ts, utcOffset, epoch);
    MS_DEEP_DBG(F("Setting raw RTC value to:"), converted_ts);
    setRawRTCNow(converted_ts);
    PRINTOUT(F("Clock set!"));
    return true;
}

// This checks that the logger time is within a "sane" range
bool loggerClock::isRTCSane(void) {
    uint32_t curRTC  = getNowAsEpoch(0, epochStart::unix_epoch);
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


// Unfortunately, most RTC's do not seem to follow anything like a cron
// schedule. Recurring/Periodic alarms can generally be only on single
// seconds/minutes/hours/days not on custom intervals.
void loggerClock::enablePeriodicRTCInterrupts() {
    // Disable any previous interrupts
    disableRTCInterrupts();
    resetClockInterruptStatus();
    MS_DBG(F("Setting periodic alarm on"), MS_CLOCK_NAME,
           F("for every minute."));
#if defined(MS_USE_RV8803)
    // Enable a periodic update for every minute
    rtc.setPeriodicTimeUpdateFrequency(TIME_UPDATE_1_MINUTE);
    // Enable the hardware interrupt
    rtc.enableHardwareInterrupt(UPDATE_INTERRUPT);
#elif defined(MS_USE_DS3231)
    rtc.enableInterrupts(EveryMinute);
#elif defined(MS_USE_RTC_ZERO)
    // We need to set this to 59, because the wake actually occurs 1 second
    // later; see datasheet 19.6.3:
    // > When an alarm match occurs, the Alarm 0 Interrupt flag in the Interrupt
    // Flag Status and Clear registers (INTFLAG.ALARMn0) is set on the next
    // 0-to-1 transition of CLK_RTC_CNT. E.g. For a 1Hz clock counter, it means
    // the Alarm 0 Interrupt flag is set with a delay of 1s after the occurrence
    // of alarm match. A valid alarm match depends on the setting of the Alarm
    // Mask Selection bits in the Alarm
    zero_sleep_rtc.attachInterrupt(loggerClock::rtcISR);
    zero_sleep_rtc.setAlarmSeconds(59);
    zero_sleep_rtc.enableAlarm(zero_sleep_rtc.MATCH_SS);

#endif  // defined(MS_USE_RTC_ZERO)
}
void loggerClock::setNextRTCInterrupt(uint32_t ts, int8_t utcOffset,
                                      epochStart epoch) {
    // Disable any previous interrupts
    disableRTCInterrupts();
    resetClockInterruptStatus();
    MS_DBG(F("Setting the next alarms on the"), MS_CLOCK_NAME, F("to"), ts);
    uint32_t converted_ts = tsToRawRTC(ts, utcOffset, epoch);

    // Create a temporary variable for the epoch time
    time_t t = converted_ts;

    // Convert the time to the processor epoch.
    // This is only needed so the gmtime function will work - this is not
    // converting between offsets.
    if (_rtcEpoch != _core_epoch) {
        t += _rtcEpoch;    // convert to to the core epoch
        t -= _core_epoch;  // convert to processor epoch (used by gmtime)
    }

    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&t);
    MS_DEEP_DBG(F("Alarm will fire at: "), tmp->tm_hour, ':', tmp->tm_min, ':',
                tmp->tm_sec);

#if defined(MS_USE_RV8803)
    // NOTE: The RV-8803 hardware does **NOT** support alarms at finer frequency
    // than minutes! The alarm will fire when the minute turns (ie, at
    // hh:mm:00). To set an alarm at a specific second interval, you would have
    // to use a periodic countdown timer interrupt and start the interrupt timer
    // carefully on the second you want to match.
    rtc.setItemsToMatchForAlarm(
        true, true, false,
        false);  // Match hours and minute so the alarm will got off 1x per day
                 // at set hh:mm:ss
    if (tmp->tm_sec != 0) {
        tmp->tm_sec = 0;
        tmp->tm_min = tmp->tm_min + 1;
        MS_DBG(F("The RV-8803 does not support alarms at specified seconds! "
                 "Rounding alarm to"),
               tmp->tm_hour, ':', tmp->tm_min, ':', tmp->tm_sec);
    }
    rtc.setAlarmMinutes(tmp->tm_min);
    rtc.setAlarmHours(tmp->tm_hour);
    rtc.enableHardwareInterrupt(ALARM_INTERRUPT);

#elif defined(MS_USE_DS3231)
    // MATCH_HOURS = match hours *and* minutes, seconds, ie 1x per day at set
    // hh:mm:ss
    rtc.enableInterrupts(MATCH_HOURS, dateAlarmValue, tmp->tm_hour, tmp->tm_min,
                         tmp->tm_sec);  // interrupt at (h,m,s)
#elif defined(MS_USE_RTC_ZERO)
    // NOTE: The interrupt is fired 1s after the match, so we set the alarm 1
    // second early.
    rtc.setAlarmTime(tmp->tm_hour, tmp->tm_min, tmp->tm_sec - 1);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);  // Every day at the matched time
#endif
}
void loggerClock::disableRTCInterrupts() {
    MS_DBG(F("Unsetting all alarms on the"), MS_CLOCK_NAME);
#if defined(MS_USE_RV8803)
    rtc.disableAllInterrupts();
    // NOTE: This disables all clock. If we only wanted to disable the
    // periodic hardware interrupt (the one we set), we could instead use
    // rtc.disableHardwareInterrupt(UPDATE_INTERRUPT);
#elif defined(MS_USE_DS3231)
    rtc.disableInterrupts();
#elif defined(MS_USE_RTC_ZERO)
    zero_sleep_rtc.disableAlarm();
#endif
}

void loggerClock::resetClockInterruptStatus(void) {
    MS_DBG(F("Clearing all interrupt flags on the"), MS_CLOCK_NAME);
#if defined(MS_USE_RV8803)
    // NOTE: We're not going to bother to call getInterruptFlag(x) to see which
    // alarm caused the interrup, because we're already using
    // disableAllInterrupts() and clearAllInterruptFlags() which prevent any
    // other interrupts from outside code from working
    // Clear all flags in case any interrupts have occurred.
    rtc.clearAllInterruptFlags();
    // NOTE: This clears all interrupt flags. If we only wanted to clear the
    // UPDATE_INTERRUPT flag (the only one we set), we could instead use
    // rtc.clearInterruptFlag(FLAG_UPDATE);
#elif defined(MS_USE_DS3231)
    rtc.clearINTStatus();
#elif defined(MS_USE_RTC_ZERO)
    // We do NOT need to clear any flags here because the RTC_Handler in the
    // RTCZero library takes care of it for us.
    // If it wasn't handled there, we would need this:
    // RTC->MODE2.INTFLAG.reg =
    //     RTC_MODE2_INTFLAG_ALARM0;  // must clear flag at end
#endif
}

void loggerClock::rtcISR(void) {
    MS_DEEP_DBG(F("\nClock interrupt!"));
}

void loggerClock::begin() {
    MS_DBG(F("Getting the epoch the processor uses for gmtime"));
    _core_epoch = getProcessorEpochStart();
    PRINTOUT(F("An"), MS_CLOCK_NAME, F("will be used as the real time clock"));
    MS_DBG(F("Beginning"), MS_CLOCK_NAME, F("real time clock"));
    rtcBegin();
    // Print out the current time
    PRINTOUT(F("Current"), MS_CLOCK_NAME, F("time is:"),
             formatDateTime_ISO8601(getNowAsEpoch(_rtcUTCOffset, _rtcEpoch),
                                    _rtcUTCOffset, _rtcEpoch));
    MS_DBG(F("The processor is uses a"), _core_epoch.printEpochName(),
           F("epoch internally, which starts"), _core_epoch.printEpochStart(),
           F("and is offset from the Unix epoch by"),
           static_cast<uint32_t>(_core_epoch - epochStart::unix_epoch),
           F("seconds"));
    MS_DBG(F("The attached"), MS_CLOCK_NAME, F("uses a"),
           _rtcEpoch.printEpochName(), F("epoch internally, which starts"),
           _rtcEpoch.printEpochStart(),
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

inline uint32_t loggerClock::tsToRawRTC(uint32_t ts, int8_t utcOffset,
                                        epochStart epoch) {
    uint32_t tz_change =
        static_cast<uint32_t>(loggerClock::_rtcUTCOffset - utcOffset) * 3600;
    MS_DEEP_DBG(F("Subtracting"), tz_change,
                F("from the timestamp to convert to the RTC's UTC offset."));
    uint32_t epoch_change = epoch - loggerClock::_rtcEpoch;
    MS_DEEP_DBG(F("Subtracting"), epoch_change,
                F("from the timestamp to convert to the RTC epoch."));
    MS_DEEP_DBG(F("Setting raw RTC value to:"), ts - tz_change - epoch_change);
    return ts - tz_change - epoch_change;
}
inline uint32_t loggerClock::tsFromRawRTC(uint32_t ts, int8_t utcOffset,
                                          epochStart epoch) {
    uint32_t epoch_change = epoch - loggerClock::_rtcEpoch;
    MS_DEEP_DBG(F("Adding"), epoch_change,
                F("to the timestamp to convert to the requested epoch."));

    // Do NOT apply an offset if the timestamp is obviously bad
    uint32_t tz_change = 0;
    if (isEpochTimeSane(ts, utcOffset, epoch)) {
        tz_change =
            static_cast<uint32_t>(loggerClock::_rtcUTCOffset + utcOffset) *
            3600;
        MS_DEEP_DBG(
            F("Adding"), tz_change,
            F("to the timestamp to convert to the requested timezone."));
    } else {
        MS_DEEP_DBG(
            F("Not converting timestamp to requested UTC offset because"), ts,
            F("doesn't appear to be a valid timestamp"));
    }

    return ts + tz_change + epoch_change;
}


#if defined(MS_USE_RV8803)
void loggerClock::rtcBegin() {
    rtc.begin();
    rtc.set24Hour();
    // void setTimeZoneQuarterHours(int8_t quarterHours);
    // Write the time zone to RV8803_RAM as int8_t (signed) in 15 minute
    // increments
    // This must happen here in the begin, not when setting the internal
    // timezone variable because this requires communication with the RTC which
    // can only happen during the run, not during compilation.
    rtc.setTimeZoneQuarterHours(loggerClock::_rtcUTCOffset * 4);
}
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
                static_cast<uint32_t>(_core_epoch));
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
void loggerClock::rtcBegin() {
    rtc.begin();
}
uint32_t loggerClock::getRawRTCNow() {
    return rtc.now().getEpoch();
}
void loggerClock::setRawRTCNow(uint32_t ts) {
    rtc.setEpoch(ts);
}

#elif defined(MS_USE_RTC_ZERO)
void loggerClock::rtcBegin() {
    zero_sleep_rtc.begin();
    // Make sure interrupts are enabled for the clock
    NVIC_EnableIRQ(RTC_IRQn);       // enable RTC interrupt
    NVIC_SetPriority(RTC_IRQn, 0);  // highest priority
}
uint32_t loggerClock::getRawRTCNow() {
    return zero_sleep_rtc.getEpoch();
}
void loggerClock::setRawRTCNow(uint32_t ts) {
    zero_sleep_rtc.setEpoch(ts);
}

#endif
