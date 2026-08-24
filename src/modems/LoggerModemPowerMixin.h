/**
 * @file LoggerModemPowerMixin.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief CRTP mixin providing power management functionality for modem classes.
 *
 * This mixin contains all power management functions including power up/down,
 * wake/sleep, reset, and pin control methods.
 */

#ifndef SRC_LOGGERMODEMPOWERMIXIN_H_
#define SRC_LOGGERMODEMPOWERMIXIN_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEMPOWER_DEBUG
#define MS_DEBUGGING_STD "LoggerModemPower"
#endif
#ifdef MS_LOGGERMODEMPOWER_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModemPower"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

#include <Arduino.h>

/**
 * @brief CRTP mixin class providing power management for modems.
 *
 * This class implements the Curiously Recurring Template Pattern (CRTP) to
 * provide power management functionality. The derived class must implement
 * the pure virtual functions modemSleepFxn() and modemWakeFxn().
 *
 * @tparam Derived The derived class type that uses this mixin
 */
template <typename Derived>
class loggerModemPowerMixin {
 protected:
    /**
     * @brief Get a reference to the derived class.
     * @return Reference to the derived class.
     */
    inline Derived& derived() {
        return static_cast<Derived&>(*this);
    }
    /**
     * @brief Get a const reference to the derived class.
     * @return Const reference to the derived class.
     */
    inline const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }


    /**
     * @anchor modem_power_functions_impl
     * @name Modem power management function implementations
     * Implementations for the functions to power up or down the modem, wake it
     * from sleep, or put it to sleep.
     */
    /**@{*/
 protected:
    /// @copydoc loggerModem::modemPowerUp()
    virtual void modemPowerUpImpl() {
        if (derived()._powerPin >= 0) {
            if (derived()._modemSleepRqPin >= 0) {
                // For most modules, the sleep pin should be held high during
                // power up. After some warm-up time, that pin is usually pulsed
                // low to wake the module.
                MS_DBG(F("Setting sleep pin"), derived()._modemSleepRqPin,
                       F("to"), !derived()._wakeLevel ? F("HIGH") : F("LOW"),
                       F("while powering on"), derived().getModemName());
                digitalWrite(derived()._modemSleepRqPin, !derived()._wakeLevel);
            }
            MS_DBG(F("Powering"), derived().getModemName(), F("with pin"),
                   derived()._powerPin);
            pinMode(derived()._powerPin, OUTPUT);
            digitalWrite(derived()._powerPin, HIGH);
            // Mark the time that the sensor was powered
            derived()._millisPowerOn = millis();
        } else {
            MS_DBG(F("Power to"), derived().getModemName(),
                   F("is not controlled by this library."));
            // Mark the power-on time, just in case it had not been marked
            if (derived()._millisPowerOn == 0)
                derived()._millisPowerOn = millis();
        }
    }

    /// @copydoc loggerModem::modemPowerDown()
    virtual void modemPowerDownImpl() {
        if (derived()._powerPin >= 0) {
            MS_DBG(F("Turning off power to"), derived().getModemName(),
                   F("with pin"), derived()._powerPin);
            digitalWrite(derived()._powerPin, LOW);
            // Unset the power-on time
            derived()._millisPowerOn = 0;
        } else {
            MS_DBG(F("Power to"), derived().getModemName(),
                   F("is not controlled by this library."));
        }
    }

    /// @copydoc loggerModem::modemWake()
    virtual bool modemWakeImpl() {
        // Set-up pin modes.
        // Because the modem calls wake BEFORE the first setup, we must set the
        // pin modes in the wake function.
        derived().setModemPinModes();

        // Power up
        if (derived()._millisPowerOn == 0) modemPowerUpImpl();
        // wait for warmup
        if (millis() - derived()._millisPowerOn < derived()._wakeDelayTime_ms) {
            MS_DBG(F("Wait"),
                   derived()._wakeDelayTime_ms -
                       (millis() - derived()._millisPowerOn),
                   F("ms longer for warm-up"));
            while (millis() - derived()._millisPowerOn <
                   derived()._wakeDelayTime_ms) {
                yield();
            }
        }

        if (isModemAwakeImpl()) {
            MS_DBG(derived().getModemName(),
                   F("was already on! Will not run wake function."));
        } else {  // Run the specific modemWakeFxn()
            MS_DBG(F("Running wake function for"), derived().getModemName());
            if (!derived().modemWakeFxn()) {
                MS_DBG(F("Wake function for"), derived().getModemName(),
                       F("did not run as expected!"));
            }
        }

        uint8_t resets  = 0;
        bool    success = false;
        while (!success && resets < 2) {
            // Check that the modem is responding to AT commands.
            MS_START_DEBUG_TIMER;
            MS_DBG(F("\nWaiting up to"), derived()._max_at_response_time_ms,
                   F("ms for"), derived().getModemName(),
                   F("to respond to AT commands..."));
            success = derived().gsmModem.testAT(
                derived()._max_at_response_time_ms + 500);
            if (success) {
                MS_DBG(F("... AT OK after"), MS_PRINT_DEBUG_TIMER,
                       F("milliseconds!"));
            } else {  // Hard reset is there's no AT response.
                MS_DBG(F("No response to AT commands!"));
                MS_DBG(F("Attempting a hard reset on the modem! "), resets + 1);
                if (!modemHardResetImpl()) {
                    // Exit if we can't hardreset.
                    break;
                } else {
                    resets++;
                }
            }
        }

        // Clean any junk out of the modem buffer.
        derived().gsmModem.streamClear();

        // Re-run the modem init, or setup if necessary.
        // This will turn off echo, which often turns itself back on after a
        // reset/power loss.
        // This also checks the SIM card state.
        if (!derived()._hasBeenSetup) {
            // If we run setup, take success value entirely from that.
            success = derived().modemSetup();
        } else {
            success &= derived().gsmModem.init();
            derived().syncNTP();
        }

        if (success) {
            derived().modemLEDOn();
            MS_DBG(derived().getModemName(),
                   F("should be awake and ready to go."));
        } else {
            MS_DBG(derived().getModemName(), F("failed to wake!"));
        }

        return success;
    }

    /// @copydoc loggerModem::modemSleep()
    virtual bool modemSleepImpl() {
        bool success = true;
        MS_DBG(F("Putting"), derived().getModemName(), F("to sleep."));

        // If there's a status pin available, check before running the sleep
        // function NOTE:  It's possible that the modem could still be in the
        // process of turning on and thus status pin isn't valid yet.  In that
        // case, we wouldn't yet know it's coming on and so we'd mistakenly
        // assume it's already off and not turn it back off. This only applies
        // to modules with a pulse wake (i.e., non-zero wake time). For all
        // modules that do pulse on, where possible I've selected a pulse time
        // that is sufficient to wake but not quite long enough to put it to
        // sleep and am using AT commands to sleep.  This *should* keep
        // everything lined up.
        if (!isModemAwakeImpl()) {
            MS_DBG(derived().getModemName(),
                   F("is already off!  Will not run sleep function."));
        } else {
            // Run the sleep function
            MS_DBG(F("Running given sleep function for"),
                   derived().getModemName());
            success &= derived().modemSleepFxn();
            derived().modemLEDOff();
        }
        return success;
    }

    /// @copydoc loggerModem::modemSleepPowerDown()
    virtual bool modemSleepPowerDownImpl() {
        bool     success = true;
        uint32_t start   = millis();
        MS_DBG(F("Turning"), derived().getModemName(), F("off."));

        // Put the modem to sleep
        modemSleepImpl();

        // Now power down
        if (derived()._powerPin >= 0) {
            // If there's a status pin available, wait until modem shows it's
            // ready to be powered off This allows the modem to shut down
            // gracefully.
            if (derived()._statusPin >= 0) {
                MS_DBG(
                    F("Waiting up to"), derived()._disconnectTime_ms,
                    F("milliseconds for graceful shutdown as indicated by pin"),
                    derived()._statusPin, F("going"),
                    !derived()._statusLevel ? F("HIGH") : F("LOW"), F("..."));
                while (millis() - start < derived()._disconnectTime_ms &&
                       digitalRead(derived()._statusPin) ==
                           static_cast<int>(derived()._statusLevel)) {
                    yield();
                }
                if (digitalRead(derived()._statusPin) ==
                    static_cast<int>(derived()._statusLevel)) {
                    MS_DBG(F("... "), derived().getModemName(),
                           F("did not successfully shut down!"));
                } else {
                    MS_DBG(F("... shutdown complete after"), millis() - start,
                           F("ms."));
                }
            } else if (derived()._disconnectTime_ms > 0) {
                MS_DBG(F("Waiting"), derived()._disconnectTime_ms,
                       F("ms for graceful shutdown."));
                while (millis() - start < derived()._disconnectTime_ms) {
                    yield();
                }
            }

            MS_DBG(F("Turning off power to"), derived().getModemName(),
                   F("with pin"), derived()._powerPin);
            digitalWrite(derived()._powerPin, LOW);
            // Unset the power-on time
            derived()._millisPowerOn = 0;
        } else {
            // If we're not going to power the modem down, there's no reason to
            // hold up the main processor while waiting for the modem to shut
            // down. It can just do its thing unwatched while the main processor
            // sleeps.
            MS_DBG(F("Power to"), derived().getModemName(),
                   F("is not controlled by this library - not waiting for "
                     "shut-down to complete."));
        }

        return success;
    }

    /// @copydoc loggerModem::modemHardReset()
    virtual bool modemHardResetImpl() {
        if (derived()._modemResetPin >= 0) {
            MS_DBG(F("Doing a hard reset on the modem by setting pin"),
                   derived()._modemResetPin,
                   derived()._resetLevel ? F("HIGH") : F("LOW"), F("for"),
                   derived()._resetPulse_ms, F("ms"));
            digitalWrite(derived()._modemResetPin, derived()._resetLevel);
            delay(derived()._resetPulse_ms);
            digitalWrite(derived()._modemResetPin, !derived()._resetLevel);
            return true;
        } else {
            MS_DBG(F("No pin has been provided to reset the modem!"));
            return false;
        }
    }

    /// @copydoc loggerModem::isModemAwake()
    virtual bool isModemAwakeImpl() {
        if (derived()._wakePulse_ms == 0 && derived()._modemSleepRqPin >= 0) {
            // If the wake up is one where a pin is held (0 wake time) and that
            // pin is defined, then we're going to check the level of the held
            // pin as the indication of whether attempts were made to wake the
            // modem before entering the setup function.
            int8_t sleepRqBitNumber =
                log(digitalPinToBitMask(derived()._modemSleepRqPin)) / log(2);
            bool currentRqPinState =
                bitRead(*portInputRegister(
                            digitalPinToPort(derived()._modemSleepRqPin)),
                        sleepRqBitNumber);
            MS_DBG(F("Current state of sleep request pin"),
                   derived()._modemSleepRqPin, '=',
                   currentRqPinState ? F("HIGH") : F("LOW"), F("meaning"),
                   derived().getModemName(), F("should be"),
                   currentRqPinState == derived()._wakeLevel ? F("on")
                                                             : F("off"));
            return currentRqPinState == derived()._wakeLevel;
        } else if (derived()._statusPin >= 0) {
            // If there's a status pin, use that to determine if the modem is
            // awake
            bool levelNow = digitalRead(derived()._statusPin);
            MS_DBG(derived().getModemName(), F("status pin"),
                   derived()._statusPin, F("level = "),
                   levelNow ? F("HIGH") : F("LOW"), F("meaning"),
                   derived().getModemName(), F("should be"),
                   levelNow == derived()._statusLevel ? F("on") : F("off"));
            return levelNow == derived()._statusLevel;
        } else {
            // If we can't determine status by pin level, try checking if the
            // modem responds to AT commands.
            MS_DEEP_DBG(F("No modem status pins, checking if modem is awake by "
                          "testing AT response"));
            int8_t i   = 5;
            bool   res = false;
            while (i && !res) {
                derived().gsmModem.sendAT(GF(""));
                res = derived().gsmModem.waitResponse(100) == 1;
                if (res) break;
                delay(50);
                i--;
            }
            MS_DBG(F("Tested AT command and got"),
                   res ? F("OK") : F("no response"), F("meaning"),
                   derived().getModemName(),
                   res ? F("must be awake") : F("is probably asleep"));
            return res;
        }
    }
    /**@}*/


    /* ===================================================================== */
    /* Pin Level Functions                                                   */
    /* ===================================================================== */
    /**
     * @anchor modem_pin_levels_impl
     * @name Pin setting function implementations
     * Functions to set the pin levels of the modem control pins.  These are
     * called by the public functions in LoggerModemImpl.h and are not intended
     * to be called directly.
     */
    /**@{*/
 public:
    /// @copydoc loggerModem::setModemLED()
    void setModemLEDImpl(int8_t modemLEDPin) {
        derived()._modemLEDPin = modemLEDPin;
        if (derived()._modemLEDPin >= 0) {
            pinMode(derived()._modemLEDPin, OUTPUT);
            digitalWrite(derived()._modemLEDPin, LOW);
        }
    }

    /// @copydoc loggerModem::setModemStatusLevel()
    void setModemStatusLevelImpl(bool level) {
        derived()._statusLevel = level;
    }

    /// @copydoc loggerModem::setModemWakeLevel()
    void setModemWakeLevelImpl(bool level) {
        derived()._wakeLevel = level;
    }

    /// @copydoc loggerModem::setModemResetLevel()
    void setModemResetLevelImpl(bool level) {
        derived()._resetLevel = level;
    }
    /**@}*/

    /* ===================================================================== */
    /* Helper Functions                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_power_helper_functions
     * @name Power Management Helper Functions
     */
    /**@{*/
 public:
    /**
     * @brief Turn on the modem LED/alert pin - sets it `HIGH`
     */
    void modemLEDOn() {
        if (derived()._modemLEDPin >= 0) {
            digitalWrite(derived()._modemLEDPin, HIGH);
        }
    }

    /**
     * @brief Turn off the modem LED/alert pin - sets it `LOW`
     */
    void modemLEDOff() {
        if (derived()._modemLEDPin >= 0) {
            digitalWrite(derived()._modemLEDPin, LOW);
        }
    }
    /**@}*/


    /* ===================================================================== */
    /* Internal Members                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_ctor_variables
     * @name Member variables set in the constructor
     * These are all related to expected modem response times and the pin
     * connections between the modem module and the logger MCU.
     */
    /**@{*/
 protected:
    /**
     * @brief The digital pin number of the mcu pin controlling power to the
     * modem (active `HIGH`).
     *
     * Should be set to a negative number if the modem should be continuously
     * powered or the power cannot be controlled by the MCU.
     */
    int8_t _powerPin;
    /**
     * @brief The digital pin number of the mcu pin connected to the modem
     * status output pin.
     *
     * Should be set to a negative number if the modem status pin cannot be
     * read.
     */
    int8_t _statusPin;
    /**
     * @brief The level (`LOW` or `HIGH`) of the #_statusPin when the modem
     * is active.
     */
    bool _statusLevel;
    /**
     * @brief The digital pin number of the pin on the mcu attached to the hard
     * or panic reset pin of the modem.
     *
     * Should be set to a negative number if the modem reset pin is not
     * connected to the MCU.
     */
    int8_t _modemResetPin;
    /**
     * @brief The level (`LOW` or `HIGH`) of the #_modemResetPin which will
     * cause the modem to reset.
     */
    bool _resetLevel;
    /**
     * @brief The length of time in milliseconds at #_resetLevel needed on
     * #_modemResetPin to reset the modem.
     */
    uint32_t _resetPulse_ms;
    /**
     * @brief The digital pin number of a pin on the mcu used to request the
     * modem enter its lowest possible power state.
     *
     * Should be set to a negative number if there is no pin usable for deep
     * sleep modes or it is not accessible to the MCU.
     */
    int8_t _modemSleepRqPin;
    /**
     * @brief The level (`LOW` or `HIGH`) on the #_modemSleepRqPin used to
     * **wake** the modem.
     */
    bool _wakeLevel;
    /**
     * @brief The length of pulse in milliseconds at #_wakeLevel needed on the
     * #_modemSleepRqPin to wake the modem.
     *
     * Set to 0 if the pin must be continuously held at #_wakeLevel to keep the
     * modem active.
     */
    uint32_t _wakePulse_ms;
    /**
     * @brief The time in milliseconds between when loggerModem::modemWake() is
     * run and when the #_statusPin is expected to be at #_statusLevel.
     */
    uint32_t _statusTime_ms;
    /**
     * @brief The maximum length of time in milliseconds between when the modem
     * is requested to enter lowest power state (loggerModem::modemSleep()) and
     * when it should have completed necessary steps to shut down.
     */
    uint32_t _disconnectTime_ms;
    /**
     * @brief The time in milliseconds between when the modem is powered and
     * when it is able to receive a wake command.
     *
     * That is, the time that should be allowed between
     * loggerModem::modemPowerUp() and loggerModem::modemWake().
     */
    uint32_t _wakeDelayTime_ms;
    /**
     * @brief The processor elapsed time when the power was turned on for the
     * modem.
     *
     * The #_millisPowerOn value is set in the loggerModem::modemPowerUp()
     * function.  It is un-set in the loggerModem::modemSleepPowerDown()
     * function.
     */
    uint32_t _millisPowerOn = 0;

    /**
     * @brief The digital pin number of a pin on the mcu used to indicate the
     * modem's current activity state.
     *
     * Intended to be a pin attached to a LED.
     *
     * Should be set to a negative number if no LED is available.
     */
    int8_t _modemLEDPin;
    /**@}*/
};

#endif  // SRC_LOGGERMODEMPOWERMIXIN_H_
