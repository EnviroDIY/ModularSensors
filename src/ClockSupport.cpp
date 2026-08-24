/**
 * @file ClockSupport.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the loggerClock class for working with real-time clocks.
 */
#include "ClockSupport.h"
#include "LoggerBase.h"

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
#if defined(MS_USE_RV8803)
RV8803 loggerClock::rtc;
#elif defined(MS_USE_DS3231)
// pass
// The Sodaq DS3231 library externs the clock instance, so it's not needed here
#elif defined(MS_USE_RTC_ZERO)
RTCZero loggerClock::zero_sleep_rtc;
#endif


// Sets the static offset from UTC that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC (i.e., offset = 0)
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void loggerClock::setRTCOffset(int8_t offsetHours) {
    _rtcUTCOffset = offsetHours;
// Some helpful prints for debugging
#if !defined(MS_SILENT)
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
int8_t loggerClock::getRTCOffset() {
    return loggerClock::_rtcUTCOffset;
}

time_t loggerClock::getNowAsEpoch(int8_t utcOffset, epochStart epoch) {
    time_t rtc_return = getRawRTCNow();
    MS_DEEP_DBG(F("Raw returned RTC timestamp (getNowAsEpoch):"), rtc_return);
    return tsFromRawRTC(rtc_return, utcOffset, epoch);
}


void loggerClock::getNowAsParts(int8_t& seconds, int8_t& minutes, int8_t& hours,
                                int8_t& day, int8_t& month, int16_t& year,
                                uint8_t tz_offset) {
    // Check the current RTC time
    time_t rtc_return = getRawRTCNow();
    MS_DEEP_DBG(F("Raw returned RTC timestamp (getNowAsParts):"), rtc_return);
    // convert to the core epoch and the input timezone offset
    time_t rtc_as_core = tsFromRawRTC(rtc_return, tz_offset,
                                      TimeUtils::getCoreEpochStart());
    MS_DEEP_DBG(F("Input time converted to processor epoch:"), rtc_as_core, '(',
                TimeUtils::printEpochName(TimeUtils::getCoreEpochStart()), ')');

    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&rtc_as_core);
    MS_DEEP_DBG(F("Populated time components: "), tmp->tm_year + 1900, F(" - "),
                tmp->tm_mon + 1, F(" - "), tmp->tm_mday, F("    "),
                tmp->tm_hour, F(" : "), tmp->tm_min, F(" : "), tmp->tm_sec);

    seconds = tmp->tm_sec;
    minutes = tmp->tm_min;
    hours   = tmp->tm_hour;
    day     = tmp->tm_mday;
    month   = tmp->tm_mon + 1;      // tm_mon is 0-11
    year    = tmp->tm_year + 1900;  // tm_year is years since 1900
}

// This sets the real time clock to the given time
bool loggerClock::setRTClock(time_t ts, int8_t utcOffset, epochStart epoch) {
    MS_DEEP_DBG(F("Raw input timestamp:"), ts);
    return setRTClock(
        epochTime(ts, static_cast<int32_t>(utcOffset) * 3600, epoch),
        utcOffset);
}
bool loggerClock::setRTClock(epochTime in_time, int8_t utcOffset) {
    // If the timestamp is not in the valid range, just exit
    if (!TimeUtils::isTimeSane(in_time)) {
        PRINTOUT(F("Bad timestamp, not setting clock."));
        return false;
    }

    // The "setTime" is the number of seconds since the start of the input
    // epoch We're interested in the setTime in the logger's and RTC's
    // timezone The RTC's timezone is equal to the logger's timezone minus
    // the offset between the logger and the RTC.
    time_t new_rtc_value = TimeUtils::convertOffsetAndEpoch(
        in_time.getTimestamp(), static_cast<int32_t>(utcOffset) * 3600,
        epochStart::unix_epoch, static_cast<int32_t>(_rtcUTCOffset) * 3600,
        _rtcEpoch);

    // Check the current RTC time
    time_t prev_rtc_value = getNowAsEpoch(_rtcUTCOffset, _rtcEpoch);
    MS_DBG(F("    Current Time on RTC (in RTC's epoch):"), prev_rtc_value,
           F("->"),
           TimeUtils::formatISO8601(prev_rtc_value, _rtcUTCOffset, _rtcEpoch));
    MS_DBG(F("    Offset between input and RTC (seconds):"),
           abs(new_rtc_value - prev_rtc_value));

    // NOTE:  Because we take the time to do some UTC/Local conversions and
    // print stuff out, the clock might end up being set up to a few
    // milliseconds behind the input time.  Given some of the supported clocks
    // are only accurate to seconds (not milliseconds or less), I don't think
    // this is a problem.

    // If the RTC is already within 5 seconds of the input time, just quit
    if (abs(new_rtc_value - prev_rtc_value) < 5) {
        PRINTOUT(F("Clock already within 5 seconds of time."));
        // return true because the clock is correctly set, even if we didn't
        // actually set it
        return true;
    }

    MS_DEEP_DBG(F("Setting raw RTC value to:"), new_rtc_value);
    setRawRTCNow(new_rtc_value);
    PRINTOUT(F("Clock set!"));
    PRINTOUT(F("Current RTC time is now"),
             TimeUtils::formatISO8601(getNowAsEpoch(_rtcUTCOffset, _rtcEpoch),
                                      _rtcUTCOffset, _rtcEpoch));
    return true;
}

// This checks that the logger time is within a "sane" range
bool loggerClock::isRTCSane() {
    time_t curRTC  = getRawRTCNow();
    bool   is_sane = TimeUtils::isTimeSane(curRTC, loggerClock::_rtcUTCOffset,
                                           _rtcEpoch);
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

void loggerClock::setNextRTCInterrupt(time_t ts, int8_t utcOffset,
                                      epochStart epoch) {
    MS_DEEP_DBG(F("Raw input alarm timestamp:"), ts);
    setNextRTCInterrupt(
        epochTime(ts, static_cast<int32_t>(utcOffset) * 3600, epoch),
        utcOffset);
}
void loggerClock::setNextRTCInterrupt(epochTime in_time, int8_t utcOffset) {
    // Disable any previous interrupts
    disableRTCInterrupts();
    resetClockInterruptStatus();

    // Use the conversion function to get a temporary variable for the epoch
    // time in the epoch used by the processor core (i.e., used by gmtime).
    time_t t = TimeUtils::convertOffsetAndEpoch(
                   in_time.getTimestamp(),
                   static_cast<int32_t>(utcOffset) * 3600,
                   epochStart::unix_epoch,
                   static_cast<int32_t>(_rtcUTCOffset) * 3600, _rtcEpoch) -
        static_cast<time_t>(utcOffset * 3600);
    MS_DBG(F("Setting the next alarm on the"), MS_CLOCK_NAME, F("to"),
           static_cast<uint32_t>(t));

    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&t);
    MS_DEEP_DBG(F("Alarm will fire at: "), tmp->tm_hour, ':', tmp->tm_min, ':',
                tmp->tm_sec);

#if defined(MS_USE_RV8803)
    // NOTE: The RV-8803 hardware does **NOT** support alarms at finer frequency
    // than minutes! The alarm will fire when the minute turns (i.e., at
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
    // MATCH_HOURS = match hours *and* minutes, seconds, i.e., 1x per day at set
    // hh:mm:ss
    rtc.enableInterrupts(MATCH_HOURS, 0, tmp->tm_hour, tmp->tm_min,
                         tmp->tm_sec);  // interrupt at (h,m,s)
#elif defined(MS_USE_RTC_ZERO)
    // NOTE: The interrupt is fired 1s after the match, so we set the alarm 1
    // second early.
    zero_sleep_rtc.setAlarmTime(tmp->tm_hour, tmp->tm_min, tmp->tm_sec - 1);
    zero_sleep_rtc.enableAlarm(
        zero_sleep_rtc.MATCH_HHMMSS);  // Every day at the matched time
#endif
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

void loggerClock::resetClockInterruptStatus() {
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

void loggerClock::rtcISR() {
#if defined(MS_CLOCKSUPPORT_DEBUG) || defined(MS_LOGGERBASE_DEBUG_DEEP)
    // This is bad practice - calling a Serial.print from an ISR
    // But.. it's so helpful for debugging!
    // Turn off MS_CLOCKSUPPORT_DEBUG and MS_LOGGERBASE_DEBUG_DEEP in production
    // code!
    PRINTOUT(F("\nClock interrupt!\n"));
#endif
}

void loggerClock::begin() {
    MS_DBG(F("Initializing core epoch and timezone detection"));
    TimeUtils::begin();  // Initialize core epoch and timezone
    PRINTOUT(F("An"), MS_CLOCK_NAME, F("will be used as the real time clock"));
    MS_DBG(F("Beginning"), MS_CLOCK_NAME, F("real time clock"));
    rtcBegin();
    // Print out the current time
    PRINTOUT(F("Current"), MS_CLOCK_NAME, F("time is:"),
             TimeUtils::formatISO8601(getNowAsEpoch(_rtcUTCOffset, _rtcEpoch),
                                      _rtcUTCOffset, _rtcEpoch));
    MS_DBG(F("The processor uses a"),
           TimeUtils::printEpochName(TimeUtils::getCoreEpochStart()),
           F("epoch internally, which starts"),
           TimeUtils::printEpochStart(TimeUtils::getCoreEpochStart()),
           F("and is offset from the Unix epoch by"),
           static_cast<uint32_t>(
               static_cast<uint32_t>(TimeUtils::getCoreEpochStart()) -
               static_cast<uint32_t>(epochStart::unix_epoch)),
           F("seconds"));
    MS_DBG(F("The processor considers local time to be"),
           TimeUtils::getCoreTimeZone(), F("seconds ("),
           TimeUtils::getCoreTimeZone() / 3600, F("hours) offset from UTC"));
    MS_DBG(F("The attached"), MS_CLOCK_NAME, F("uses a"),
           TimeUtils::printEpochName(_rtcEpoch),
           F("epoch internally, which starts"),
           TimeUtils::printEpochStart(_rtcEpoch),
           F("and is offset from the Unix epoch by"),
           static_cast<uint32_t>(static_cast<uint32_t>(_rtcEpoch) -
                                 static_cast<uint32_t>(epochStart::unix_epoch)),
           F("seconds"));
}

inline time_t loggerClock::tsToRawRTC(time_t ts, int8_t utcOffset,
                                      epochStart epoch) {
    time_t tz_change =
        static_cast<time_t>(loggerClock::_rtcUTCOffset - utcOffset) * 3600;
    MS_DEEP_DBG(F("Subtracting"), tz_change,
                F("from the timestamp to convert to the RTC's UTC offset."));
    time_t ts_conv = TimeUtils::convertEpoch(ts - tz_change, epoch,
                                             loggerClock::_rtcEpoch);
    MS_DEEP_DBG(F("Equivalent raw RTC value is:"), ts_conv);
    return ts_conv;
}
inline time_t loggerClock::tsFromRawRTC(time_t ts, int8_t utcOffset,
                                        epochStart epoch) {
    time_t ts_conv = TimeUtils::convertEpoch(ts, loggerClock::_rtcEpoch, epoch);
    MS_DEEP_DBG(F("In"), TimeUtils::printEpochName(epoch),
                F("epoch, RTC would be:"), ts_conv);

    // Do NOT apply an offset if the timestamp is obviously bad
    time_t tz_change = 0;
    if (TimeUtils::isTimeSane(ts_conv, utcOffset, epoch)) {
        tz_change =
            static_cast<time_t>(loggerClock::_rtcUTCOffset + utcOffset) * 3600;
        MS_DEEP_DBG(F("Adding"), tz_change,
                    F("to the timestamp to convert to UTC"),
                    (utcOffset >= 0) ? '+' : '-', abs(utcOffset), F("hours"));
    } else {
        MS_DEEP_DBG(
            F("Not converting timestamp to requested UTC offset because"), ts,
            F("doesn't appear to be a valid timestamp"));
    }

    return ts_conv + tz_change;
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
time_t loggerClock::getRawRTCNow() {
    // uint32_t getEpoch(bool use1970sEpoch = false);
    // The use1970sEpoch works properly ONLY on AVR/8-bit boards!!
    //   - Setting use1970sEpoch to false returns the seconds from Jan 1, 2000.
    //   - Setting use1970sEpoch to true returns the seconds from Jan 1, 1970.
    // For 32-bit boards, you must set use1970sEpoch to FALSE to get the 1970's
    // epoch. See:
    // https://github.com/sparkfun/SparkFun_RV-8803_Arduino_Library/issues/29

    // Get the epoch - with the time zone subtracted (i.e. return UTC epoch)
    rtc.updateTime();
    MS_DEEP_DBG(F("Set use1970sEpoch to"),
                TimeUtils::getCoreEpochStart() == epochStart::y2k_epoch,
                F("because the processor epoch is"),
                TimeUtils::printEpochName(TimeUtils::getCoreEpochStart()), '(',
                static_cast<uint32_t>(TimeUtils::getCoreEpochStart()), ')');
    return static_cast<time_t>(
        rtc.getEpoch(TimeUtils::getCoreEpochStart() == epochStart::y2k_epoch));
}
void loggerClock::setRawRTCNow(time_t ts) {
    // bool setEpoch(uint32_t value, bool use1970sEpoch = false, int8_t
    // timeZoneQuarterHours = 0);
    // If timeZoneQuarterHours is non-zero, update RV8803_RAM. Add the zone to
    // the epoch before setting
    rtc.setEpoch(static_cast<uint32_t>(ts),
                 TimeUtils::getCoreEpochStart() == epochStart::y2k_epoch);
}

#elif defined(MS_USE_DS3231)
void loggerClock::rtcBegin() {
    rtc.begin();
}
time_t loggerClock::getRawRTCNow() {
    return static_cast<time_t>(rtc.now().getEpoch());
}
void loggerClock::setRawRTCNow(time_t ts) {
    rtc.setEpoch(static_cast<uint32_t>(ts));
}

#elif defined(MS_USE_RTC_ZERO)
void loggerClock::rtcBegin() {
    zero_sleep_rtc.begin();
    // Make sure interrupts are enabled for the clock
    NVIC_EnableIRQ(RTC_IRQn);       // enable RTC interrupt
    NVIC_SetPriority(RTC_IRQn, 0);  // highest priority
}
time_t loggerClock::getRawRTCNow() {
    return static_cast<time_t>(zero_sleep_rtc.getEpoch());
}
void loggerClock::setRawRTCNow(time_t ts) {
    zero_sleep_rtc.setEpoch(static_cast<uint32_t>(ts));
}

#endif

// cSpell:ignore nist hhmm NVIC prtout1
