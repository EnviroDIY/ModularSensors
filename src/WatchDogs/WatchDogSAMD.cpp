/**
 * @file WatchDogSAMD.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the extendedWatchDogSAMD class
 */

#include "WatchDogSAMD.h"

// Be careful to use a platform-specific conditional include to only make the
// code visible for the appropriate platform.  Arduino will try to compile and
// link all .cpp files regardless of platform.
#if defined(ARDUINO_ARCH_SAMD)

volatile uint32_t extendedWatchDogSAMD::_barksUntilReset = 0;
uint32_t          extendedWatchDogSAMD::_resetTime_s     = 900;


// One-time initialization of watchdog timer.
void extendedWatchDogSAMD::setupWatchDog(uint32_t resetTime_s) {
    _resetTime_s = resetTime_s;
    // Longest interrupt is 16s, so we loop that as many times as needed
    extendedWatchDogSAMD::_barksUntilReset = _resetTime_s /
        MAXIMUM_WATCHDOG_PERIOD;
    MS_DBG(F("Watch-dog configured to reset the board after"), _resetTime_s,
           F("sec with an early warning interrupt firing after"),
           MAXIMUM_WATCHDOG_PERIOD, F("sec and a total of"),
           extendedWatchDogSAMD::_barksUntilReset,
           F("warnings before the reset."));

    // configure the watch-dog source clock
    config32kOSC();
    configureClockGenerator();
    configureWDTClock();

    // Enable WDT early-warning interrupt
    NVIC_DisableIRQ(WDT_IRQn);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_SetPriority(WDT_IRQn,
                     1);  // Priority behind RTC, just in case we're using it!
    NVIC_EnableIRQ(WDT_IRQn);
    waitForWDTBitSync();
}


void extendedWatchDogSAMD::enableWatchDog() {
    // Steps:
    // - Disable watchdog for config
    // - Clear any pending interrupt flags
    // - Enable the early warning interrupt
    // - Set the watchdog time-out period to the maximum value
    //   - 0xB - 16384 clockcycles @ 1024hz = 16 seconds
    // - Set the watchdog window mode closed-window time-out period to the
    // maximum value
    //   - 0xB - 16384 clockcycles @ 1024hz = 16 seconds
    // - Set the watchdog early warning offset value to the minimum value.
    //   - 0x0 - 8 clockcycles @ 1024hz ~= 7.8ms
    //   - This gives us a very short window in which to clear the watchdog, but
    //   simplifies timing since we don't have to worry about extra time between
    //   the close of the window and the reset firing if the interrupt isn't
    //   cleared.
    // - Enable windowed mode
    MS_DEEP_DBG(F("Configuring the watchdog"));
#if defined(__SAMD51__)
    WDT->CTRLA.reg = 0;
#else
    WDT->CTRL.reg          = 0;
#endif
    waitForWDTBitSync();

    WDT->INTFLAG.bit.EW      = 1;    // Clear interrupt flag
    WDT->INTENSET.bit.EW     = 1;    // Enable early warning interrupt
    WDT->CONFIG.bit.PER      = 0xB;  // Max time out period
    WDT->CONFIG.bit.WINDOW   = 0xB;  // Max closed window period
    WDT->EWCTRL.bit.EWOFFSET = 0x0;  // Minimum open-window period after warning
    WDT->CTRLA.bit.WEN       = 1;    // Enable window mode
    waitForWDTBitSync();

    MS_DBG(F("Enabling watch dog..."));
    resetWatchDog();

    // Set the enable bit
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 1;
#else  // SAMD21
    WDT->CTRL.bit.ENABLE   = 1;
#endif
    waitForWDTBitSync();
}


void extendedWatchDogSAMD::disableWatchDog() {
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 0;
#else  // SAMD21
    WDT->CTRL.bit.ENABLE   = 0;
#endif
    waitForWDTBitSync();
    MS_DBG(F("Watch dog disabled."));
}


void extendedWatchDogSAMD::resetWatchDog() {
    MS_DEEP_DBG(F("Feeding the watch-dog!"));
    extendedWatchDogSAMD::_barksUntilReset = _resetTime_s /
        MAXIMUM_WATCHDOG_PERIOD;
}

void extendedWatchDogSAMD::setupEIC() {
    config32kOSC();
    configureClockGenerator();
    configureEICClock();
}

void extendedWatchDogSAMD::config32kOSC() {
#if defined(__SAMD51__)
    // SAMD51 WDT uses OSCULP32k as input clock, make sure it's enabled
    // section: 20.5.3
    MS_DEEP_DBG(F("Configuring the outputs of the ultra-low power internal 32k "
                  "oscillator."));
    OSC32KCTRL->OSCULP32K.bit.EN1K  = 1;  // Enable out 1K (for WDT)
    OSC32KCTRL->OSCULP32K.bit.EN32K = 1;  // Enable out 32K (for EIC)
    waitForWDTBitSync();

#else  // SAMD21
    // NOTE: There are no settings we need to configure for ultra-low power
    // internal oscillator (OSCULP32K). The only things that can be configured
    // are the write lock and over-writing the factory calibration. We don't
    // want to do either of those. The OSCULP32K is *always* running, no matter
    // what sleep mode is in use.
#endif
}

void extendedWatchDogSAMD::configureClockGenerator() {
#if defined(__SAMD51__)
    // Do nothing
    // The SAMD51 WDT always uses the 1.024kHz CLK_WDT_OSC clock sourced from
    // the ULP32KOSC.  The SAMD51 can also use OSCULP32k directly for the EIC.
    // No separate clock generator is needed.
#else  // SAMD21
    // Per datasheet 15.6.2.6, the source for the generic clock generator can be
    // changed on the fly, so we don't need to disable it for configuration.

    // Configure the generic clock generator **divisor** for the clock
    // generator.
    // The divisor determines the relationship between the generic clock's tick
    // speed and the clock source's tick speed.
    // The divisor register must be configured before the generator control
    // register.
    // divisor = 32(2 ^ (DIV + 1)) = 4
    // With 32 divisor the actual clock speed is ~1024Hz clock.
    MS_DEEP_DBG(F("Configuring the divisor for generic clock generator"),
                GENERIC_CLOCK_GENERATOR_MS);
    GCLK->GENDIV.reg = static_cast<uint32_t>(
        GCLK_GENDIV_ID(
            GENERIC_CLOCK_GENERATOR_MS) |  // Select Generic Clock Generator
        GCLK_GENDIV_DIV(4));               // Divide the clock source by 32
    waitForGCLKBitSync();

    // Configure the generic clock **generator**
    // Use the built-in ultra-low power internal 32.768kHz oscillator for the
    // watchdog and the external interrupt controller. This is less accurate
    // than the 32k crystal, but uses less power. For the watchdog and the
    // external interrupts, we don't need very high accuracy, so lower power is
    // better.
    // NOTE: The generic clock generator must be enabled by performing a single
    // 32-bit write to the Generic Clock Generator Control register (GENCTRL) -
    // ie, do this all in one step.
    // NOTE: Per the manual 15.8.4, the run in standby setting
    // (GCLK_GENCTRL_RUNSTDBY) for the generic clock generator control only
    // applies if the generic clock generator has been configured to be output
    // to its dedicated GCLK_IO pin. "If GENCTRL.OE is zero, this bit has no
    // effect."
    // To keep a generic clock generator available for a generic clock, the
    // clock **source** needs to be configured to run in standby via the SYSCTRL
    // registers for that source.
    MS_DEEP_DBG(F("Configuring generic clock generator"),
                GENERIC_CLOCK_GENERATOR_MS);
    GCLK->GENCTRL.reg = static_cast<uint32_t>(
        GCLK_GENCTRL_ID(GENERIC_CLOCK_GENERATOR_MS) |  // Select GCLK
        GCLK_GENCTRL_GENEN |          // Enable the generic clock clontrol
        GCLK_GENCTRL_SRC_OSCULP32K |  // Select the built-in ultra-low power
                                      // internal oscillator
        GCLK_GENCTRL_IDC |            // improve duty cycle
        GCLK_GENCTRL_DIVSEL);         // Select to divide clock by
                                      // 2^(GENDIV.DIV+1).
    waitForGCLKBitSync();
#endif
}

void extendedWatchDogSAMD::configureWDTClock() {
#if defined(__SAMD51__)
    // Enable the WDT bus clock in the main clock module.
    // NOTE: this is the default setting at power on and is not changed by the
    // Arduino core so it's not really necessary.
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_WDT;
#else  // SAMD21
    // Per datasheet 16.6.3.3 the generic clock must be disabled before being
    // re-enabled with a new clock source setting.
    MS_DEEP_DBG(F("Disabling WDT peripeheral clock for configuration"));
    // this will set all bits but the ID to 0, disabling everything
    // See https://github.com/arduino-libraries/ArduinoLowPower/issues/30
    GCLK->CLKCTRL.reg = static_cast<uint16_t>(GCLK_CLKCTRL_ID(GCM_WDT));
    waitForGCLKBitSync();

    // Feed configured GCLK to WDT (Watchdog Timer)
    // NOTE: Only one clock control id can be set at one time! See
    // https://stackoverflow.com/questions/70303177/atsamd-gclkx-for-more-peripherals
    MS_DEEP_DBG(F("Configuring and enabling peripheral clock for WDT"));
    GCLK->CLKCTRL.reg = static_cast<uint16_t>(
        GCLK_CLKCTRL_GEN(GENERIC_CLOCK_GENERATOR_MS) |  // Select generic clock
                                                        // generator
        GCLK_CLKCTRL_CLKEN |        // Enable the generic clock clontrol
        GCLK_CLKCTRL_ID(GCM_WDT));  // Feed the GCLK to the WDT
    waitForGCLKBitSync();
#endif
}

void extendedWatchDogSAMD::configureEICClock() {
#if defined(__SAMD51__)
    // Enable the EIC bus clock in the main clock module.
    // NOTE: this is the default setting at power on and is not changed by the
    // Arduino core so it's not really necessary.
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;

    MS_DEEP_DBG(F("Disabling EIC controller for configuration"));
    EIC->CTRLA.bit.ENABLE = 0;
    while (EIC->SYNCBUSY.bit.ENABLE == 1) {}

    MS_DEEP_DBG(F("Selecting the ULP32K clock as the source for the EIC"));
    // NOTE: this is the default
    // The EIC can be clocked either by GCLK_EIC (when a frequency higher
    // than 32.768 KHz is required for filtering) or by CLK_ULP32K (when power
    // consumption is the priority).
    // Since we're interested in power consumption, stick to the CLK_ULP32K.
    // Using the ULP32K also saves us the trouble of configuring a generic clock
    // for the EIC. This bit is not Write-Synchronized.
    EIC->CTRLA.bit.CKSEL = 1;  // 0 for GCLK_EIC, 1 for CLK_ULP32K

    MS_DEEP_DBG(F("Re-enabling the EIC"));
    EIC->CTRLA.bit.ENABLE = 1;
    while (EIC->SYNCBUSY.bit.ENABLE == 1) {}
#else  // SAMD21
    // Per datasheet 16.6.3.3 the generic clock must be disabled before being
    // re-enabled with a new clock source setting.
    MS_DEEP_DBG(F("Disabling EIC peripeheral clock for configuration"));
    // this will set all bits but the ID to 0, disabling everything
    // See https://github.com/arduino-libraries/ArduinoLowPower/issues/30
    GCLK->CLKCTRL.reg = static_cast<uint16_t>(GCLK_CLKCTRL_ID(GCM_EIC));
    waitForGCLKBitSync();

    // Feed configured GCLK to EIC (external interrupt controller)
    // NOTE: Only one clock control id can be set at one time! See
    // https://stackoverflow.com/questions/70303177/atsamd-gclkx-for-more-peripherals
    MS_DEEP_DBG(F("Configuring and enabling peripheral clock for EIC"));
    GCLK->CLKCTRL.reg = static_cast<uint16_t>(
        GCLK_CLKCTRL_GEN(GENERIC_CLOCK_GENERATOR_MS) |  // Select generic clock
                                                        // generator
        GCLK_CLKCTRL_CLKEN |        // Enable the generic clock clontrol
        GCLK_CLKCTRL_ID(GCM_EIC));  // Feed the GCLK to the EIC
    waitForGCLKBitSync();

    // Enable the EIC clock within the power management configuration
    // NOTE: this is the default setting at power on and is not changed by the
    // Arduino core.
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;

    // Re-enable EIC after configuring its clock
    EIC->CTRL.bit.ENABLE = 1;
    while (EIC->STATUS.bit.SYNCBUSY == 1) {}
#endif
}


void extendedWatchDogSAMD::clearWDTInterrupt() {
    MS_DEEP_DBG(F("Clearing the processor watchdog interrupt"));
    // Write the clear key
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
    waitForWDTBitSync();
    // Clear Early Warning (EW) Interrupt Flag
    WDT->INTFLAG.bit.EW = 1;
}

void extendedWatchDogSAMD::waitForWDTBitSync() {
#if defined(__SAMD51__)
    while (WDT->SYNCBUSY.reg) {
        // Wait for synchronization
    }
#else  // SAMD21
    while (WDT->STATUS.bit.SYNCBUSY) {
        // Wait for synchronization
    }
#endif
}

void extendedWatchDogSAMD::waitForGCLKBitSync() {
#if defined(__SAMD51__)
    while (GCLK->SYNCBUSY.reg &
           GCLK_SYNCBUSY_GENCTRL(GENERIC_CLOCK_GENERATOR_MS))
        ;  // Wait for the clock generator sync busy bit to clear
#else      // SAMD21
    while (GCLK->STATUS.bit.SYNCBUSY)
        ;  // Wait for synchronization
#endif
}


// ISR for watchdog early warning
void WDT_Handler(void) {
    MS_DEEP_DBG(F("\nWatchdog early warning interrupt!"));
#if defined(MS_WATCHDOGSAMD_DEBUG_DEEP)
    _MS_SERIAL_OUTPUT.println();
#endif
    // Increment down the counter, makes multi cycle WDT possible
    extendedWatchDogSAMD::_barksUntilReset--;
    if (extendedWatchDogSAMD::_barksUntilReset <= 0) {
        MS_DEEP_DBG(F("The dog has barked enough; resetting the board."));
        // Clear Early Warning (EW) Interrupt Flag
        WDT->INTFLAG.bit.EW = 1;
        // Writing a value different than WDT_CLEAR_CLEAR_KEY causes reset
        WDT->CLEAR.reg = 0xFF;
        while (true) {
            // wait
        }
    } else {
        MS_DEEP_DBG(F("There will be"), extendedWatchDogSAMD::_barksUntilReset,
                    F("more barks until total time is"),
                    extendedWatchDogSAMD::_resetTime_s, F("and board resets"));
    }
    extendedWatchDogSAMD::clearWDTInterrupt();
}
#endif
