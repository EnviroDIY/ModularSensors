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
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)

volatile uint32_t extendedWatchDogSAMD::_barksUntilReset = 0;

extendedWatchDogSAMD::extendedWatchDogSAMD() {}
extendedWatchDogSAMD::~extendedWatchDogSAMD() {
    disableWatchDog();
}

// One-time initialization of watchdog timer.
void extendedWatchDogSAMD::setupWatchDog(uint32_t resetTime_s) {
    _resetTime_s = resetTime_s;
    // Longest interrupt is 16s, so we loop that as many times as needed
    extendedWatchDogSAMD::_barksUntilReset = _resetTime_s / 8;

    MS_DBG(F("Setting up watch-dog timeout for"), _resetTime_s,
           F("sec with the interrupt firing"),
           extendedWatchDogSAMD::_barksUntilReset,
           F("times before the reset."));

    // Disable watchdog for config
    MS_DEEP_DBG(F("Disabling the watchdog for configuration."));
#if defined(__SAMD51__)
    WDT->CTRLA.reg = 0;
#else
    WDT->CTRL.reg          = 0;
#endif
    waitForWDTBitSync();

#if defined(__SAMD51__)
    MS_DEEP_DBG(F("Making sure the the USB will be disabled on standby."));
    USB->DEVICE.CTRLA.bit.ENABLE = 0;  // Disable the USB peripheral for config
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE) {
        // Wait for synchronization
    }
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;  // Deactivate run on standby
    USB->DEVICE.CTRLA.bit.ENABLE   = 1;  // Re-enable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE) {
        // Wait for synchronization
    }
#endif
    config32kOSC();
    configureWDTClockSource();

    // Enable WDT early-warning interrupt
    NVIC_DisableIRQ(WDT_IRQn);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_SetPriority(WDT_IRQn, 1);  // Priority behind RTC!
    NVIC_EnableIRQ(WDT_IRQn);

    // Set up the watch dog control parameters
#if defined(__SAMD51__)
    WDT->CTRLA.bit.WEN = 0;  // Disable window mode
#else
    WDT->CTRL.bit.WEN      = 0;  // Disable window mode
#endif
    waitForWDTBitSync();
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ALWAYSON = 0;  // NOT always on!
#else
    WDT->CTRL.bit.ALWAYSON = 0;  // NOT always on!
#endif

    waitForWDTBitSync();

    WDT->CONFIG.bit.PER =
        0xB;  // Period = 16384 clockcycles @ 1024hz = 16 seconds
    WDT->EWCTRL.bit.EWOFFSET = 0xA;  // Early Warning Interrupt Time Offset 0xA
                                     // = 8192 clockcycles @ 1024hz = 8 seconds
    WDT->INTENSET.bit.EW = 1;        // Enable early warning interrupt
    waitForWDTBitSync();

    /*In normal mode, the Early Warning interrupt generation is defined by the
    Early Warning Offset in the Early Warning Control register
    (EWCTRL.EWOFFSET). The Early Warning Offset bits define the number of
    GCLK_WDT clocks before the interrupt is generated, relative to the start of
    the watchdog time-out period. For example, if the WDT is operating in normal
    mode with CONFIG.PER = 0x2 and EWCTRL.EWOFFSET = 0x1, the Early Warning
    interrupt is generated 16 GCLK_WDT clock cycles from the start of the
    watchdog time-out period, and the watchdog time-out system reset is
    generated 32 GCLK_WDT clock cycles from the start of the watchdog time-out
    period.  The user must take caution when programming the Early Warning
    Offset bits. If these bits define an Early Warning interrupt generation time
    greater than the watchdog time-out period, the watchdog time-out system
    reset is generated prior to the Early Warning interrupt. Thus, the Early
    Warning interrupt will never be generated.*/
}


void extendedWatchDogSAMD::enableWatchDog() {
    MS_DBG(F("Enabling watch dog..."));
    resetWatchDog();

    // Set the enable bit
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 1;
#else
    WDT->CTRL.bit.ENABLE   = 1;
#endif
    waitForWDTBitSync();
}


void extendedWatchDogSAMD::disableWatchDog() {
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 0;
#else
    WDT->CTRL.bit.ENABLE   = 0;
#endif
    waitForWDTBitSync();
    MS_DBG(F("Watch dog disabled."));
}


void extendedWatchDogSAMD::resetWatchDog() {
    extendedWatchDogSAMD::_barksUntilReset = _resetTime_s / 8;
    // Write the watchdog clear key value (0xA5) to the watchdog
    // clear register to clear the watchdog timer and reset it.
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
    waitForWDTBitSync();
    // Clear Early Warning (EW) Interrupt Flag
    WDT->INTFLAG.bit.EW = 1;
}


void extendedWatchDogSAMD::config32kOSC() {
#if defined(__SAMD51__)
    // SAMD51 WDT uses OSCULP32k as input clock now
    // section: 20.5.3
    MS_DEEP_DBG(F("Configuring the output of the ultra-low power internal 32k "
                  "oscillator for the watchdog."));
    OSC32KCTRL->OSCULP32K.bit.EN1K  = 1;  // Enable out 1K (for WDT)
    OSC32KCTRL->OSCULP32K.bit.EN32K = 0;  // Disable out 32K
    waitForWDTBitSync();

#else  // SAMD21

    /** Within the SAMD core for the SAMD21
     * SystemInit() in startup.c configures these clocks:
     * 1) Enable XOSC32K clock (external on-board 32.768Hz oscillator) or
     *    OSC32K (if crystalless).
     *     - This will be used as DFLL48M reference.
     *     - SRGD NOTE: The SystemInit() function **blocks** while waiting for
     *       oscillator stabilization.  It uses a default start-up configuration
     *       of 0x6 for the (X)OSC (65536 OSCULP32K clock cycles + 3 (X)OSC32K
     *       clock cycles = 2000092μs ~= 2s before PCLKSR.XOSC32KRDY is set.)
     * 2) Put XOSC32K as source of Generic Clock Generator 1
     * 3) Put Generic Clock Generator 1 as source for Generic Clock Multiplexer
     *    0 (DFLL48M reference)
     * 4) Enable DFLL48M clock
     * 5) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
     * 6) Modify PRESCaler value of OSCM to have 8MHz
     * 7) Put OSC8M as source for Generic Clock Generator 3
     * See:
     * https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/startup.c#L311
     *
     * The ZeroPowerManager library changes clocks (including the main clock
     * source, GCLK_MAIN, which is always sourced from GCLKGEN[0]) to reduce
     * power draw. Changing the GCLK_MAIN configuration will cause some
     * functions like delay() to operate incorrectly. See:
     * https://github.com/ee-quipment/ZeroPowerManager/blob/master/ZeroPowerManager.c#L170
     * To avoid any confusing with delay(), we're not going to change anything
     * with GCLK0. This means we won't be in the lowest power state like that
     * offered by ZeroPowerManager
     * TODO: Revisit this decision
     *
     * After a power-on reset, the clock generators for peripherals default to:
     * RTC: GCLK0
     * WDT: GCLK2
     * Anything else: GCLK0
     *
     * We're going to configure a new generic clock generator for the watchdog
     * (WDT) and the external interrupt controller (EIC).
     *
     * The watchdog must be attached to a clock source so it can tell how much
     * time has passed and whether it needs to bite.
     *
     * The external interrupt controller must be attached to a currently-on
     * clock to tell the difference between rising and HIGH or falling and LOW
     * interrupts. If the external interupt controller is not attached to a
     * running clock, then interrupts will not work! Thus, if the clock source
     * for interrupts is not running in standby, the interrupts will not be able
     * to wake the device. In WInterrupts.c in the Adafruit SAMD core, generic
     * clock generator 0 (ie GCLK_MAIN) is used for the EIC peripheral. See:
     * https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/WInterrupts.c#L56
     */

    // NOTE: There are no settings we need to configure for ultra-low power
    // internal oscillator (OSCULP32K). The only things that can be configured
    // are the write lock and over-writing the factory calibration. We don't
    // want to do either of those.
#endif
}

void extendedWatchDogSAMD::configureWDTClockSource() {
#if !defined(__SAMD51__)
    // Configure the generic clock generator divisor for the clock generator
    // The divisor register must be configured before generator control register
    // divisor = 32 (2^(DIV+1))  = 4
    GCLK->GENDIV.reg =
        GCLK_GENDIV_ID(
            GENERIC_CLOCK_GENERATOR_MS) |  // Select Generic Clock Generator
        GCLK_GENDIV_DIV(4);                // Divide the clock source by 32
    waitForGCLKBitSync();

    // Use the built-in ultra-low power internal 32.768kHz oscillator for the
    // watchdog and the external interrupt controller. This is less accurate
    // than the 32k crystal, but uses less power. For the watchdog and the
    // external interrupts, we don't need very high accuracy, so lower power is
    // better.

    // source GCLK from the external oscillator
    MS_DEEP_DBG(F("Setting the ultra-low power internal 32k oscillator as the "
                  "clock source for generic clock generator"),
                GENERIC_CLOCK_GENERATOR_MS);
    GCLK->GENCTRL.reg =
        GCLK_GENCTRL_ID(GENERIC_CLOCK_GENERATOR_MS) |  // Select GCLK
        GCLK_GENCTRL_GENEN |          // Enable the generic clock clontrol
        GCLK_GENCTRL_SRC_OSCULP32K |  // Select the built-in ultra-low power
                                      // internal oscillator
        GCLK_GENCTRL_RUNSTDBY |       // DO run in standby
        GCLK_GENCTRL_DIVSEL;          // Select to divide clock by
                                      // the prescaler above
    waitForGCLKBitSync();

    // Feed configured GCLK to WDT (Watchdog Timer) **AND** the EIC (external
    // interrupt controller)
    // NOTE: This must be done in two steps, only one clock control id can be
    // set at one time! See
    // https://stackoverflow.com/questions/70303177/atsamd-gclkx-for-more-peripherals
    MS_DEEP_DBG(F("Feeding configured GCLK"), GENERIC_CLOCK_GENERATOR_MS,
                F("to WDT"));
    GCLK->CLKCTRL.reg =
        (uint16_t)(GCLK_CLKCTRL_GEN(
                       GENERIC_CLOCK_GENERATOR_MS) |  // Select generic clock
                                                      // generator
                   GCLK_CLKCTRL_CLKEN |  // Enable the generic clock clontrol
                   GCLK_CLKCTRL_ID(GCM_WDT));  // Feed the GCLK to the WDT
    waitForGCLKBitSync();
    MS_DEEP_DBG(F("Feeding configured GCLK"), GENERIC_CLOCK_GENERATOR_MS,
                F("to EIC"));
    GCLK->CLKCTRL.reg =
        (uint16_t)(GCLK_CLKCTRL_GEN(
                       GENERIC_CLOCK_GENERATOR_MS) |  // Select generic clock
                                                      // generator
                   GCLK_CLKCTRL_CLKEN |  // Enable the generic clock clontrol
                   GCLK_CLKCTRL_ID(GCM_EIC));  // Feed the GCLK to the EIC
    waitForGCLKBitSync();

    // Enable EIC after configuring its clock
    EIC->CTRL.bit.ENABLE = 1;
    while (EIC->STATUS.bit.SYNCBUSY == 1) {}

#endif
}

void extendedWatchDogSAMD::waitForWDTBitSync() {
#if defined(__SAMD51__)
    while (WDT->SYNCBUSY.reg) {
        // Wait for synchronization
    }
#else
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
#else
    while (GCLK->STATUS.bit.SYNCBUSY)
        ;  // Wait for synchronization
#endif
}


// ISR for watchdog early warning
void WDT_Handler(void) {
    // Increament down the counter, makes multi cycle WDT possible
    extendedWatchDogSAMD::_barksUntilReset--;
    // MS_DBG(F("\nWatchdog interrupt!"),
    // extendedWatchDogSAMD::_barksUntilReset);
    if (extendedWatchDogSAMD::_barksUntilReset <=
        0) {  // Clear Early Warning (EW) Interrupt Flag
        WDT->INTFLAG.bit.EW = 1;
        // Writing a value different than WDT_CLEAR_CLEAR_KEY causes reset
        WDT->CLEAR.reg = 0xFF;
        while (true) {
            // wait
        }
    } else {
        // Write the clear key
        WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
#if defined(__SAMD51__)
        while (WDT->SYNCBUSY.reg) {
            // wait
        }
#else
        while (WDT->STATUS.bit.SYNCBUSY) {
            // wait
        }
#endif
        // Clear Early Warning (EW) Interrupt Flag
        WDT->INTFLAG.bit.EW = 1;
    }
}
#endif
