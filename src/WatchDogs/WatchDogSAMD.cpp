/**
 * @file WatchDogSAMD.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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

    // Enable WDT early-warning interrupt
    NVIC_DisableIRQ(WDT_IRQn);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_SetPriority(WDT_IRQn, 1);  // Priority behind RTC!
    NVIC_EnableIRQ(WDT_IRQn);

// Disable watchdog for config
#if defined(__SAMD51__)
    WDT->CTRLA.reg = 0;
#else
    WDT->CTRL.reg = 0;
#endif
    waitForWDTBitSync();

#if defined(__SAMD51__)
    // SAMD51 WDT uses OSCULP32k as input clock now
    // section: 20.5.3
    OSC32KCTRL->OSCULP32K.bit.EN1K  = 1;  // Enable out 1K (for WDT)
    OSC32KCTRL->OSCULP32K.bit.EN32K = 0;  // Disable out 32K

    waitForWDTBitSync();

    USB->DEVICE.CTRLA.bit.ENABLE = 0;  // Disable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE) {
        // Wait for synchronization
    }
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;  // Deactivate run on standby
    USB->DEVICE.CTRLA.bit.ENABLE   = 1;  // Enable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE) {
        // Wait for synchronization
    }

#else  // SAMD21

    // We're going to use generic clock generator *5*
    // Many watch-dog examples use 2, but this conflicts with RTC-zero
    // Generic clock generator 5, divisor = 32 (2^(DIV+1))  = 4
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(5) |  // Select Generic Clock Generator 5
        GCLK_GENDIV_DIV(4);                 // Divide the clock source by 32
    while (GCLK->STATUS.bit.SYNCBUSY) {
        // Wait for synchronization
    }

    // Enable clock generator 5 using low-power 32.768kHz oscillator.
    // With /32 divisor above, this yields 1024Hz clock.
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(5) |  // Select GCLK5
        GCLK_GENCTRL_GENEN |          // Enable the generic clock clontrol
        GCLK_GENCTRL_SRC_OSCULP32K |  // Select the ultra-low power oscillator
        GCLK_GENCTRL_IDC |            // Set the duty cycle to 50/50 HIGH/LOW
        GCLK_GENCTRL_DIVSEL;  // Select to divide clock by the prescaler above
    while (GCLK->STATUS.bit.SYNCBUSY) {
        // Wait for synchronization
    }

    // Feed GCLK5 to WDT (Watchdog Timer)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK5 |  // Select generic clock 5
        GCLK_CLKCTRL_CLKEN |  // Enable the generic clock clontrol
        GCLK_CLKCTRL_ID_WDT;  // Feed the GCLK to the WDT
    while (GCLK->STATUS.bit.SYNCBUSY) {
        // Wait for synchronization
    }

#endif

    // Set up the watch dog control parameters
    WDT->CTRL.bit.WEN = 0;       // Disable window mode
    waitForWDTBitSync();         // ?? Needed here ??
    WDT->CTRL.bit.ALWAYSON = 0;  // NOT always on!
    waitForWDTBitSync();         // ?? Needed here ??

    WDT->CONFIG.bit.PER =
        0xB;  // Period = 16384 clockcycles @ 1024hz = 16 seconds
    WDT->EWCTRL.bit.EWOFFSET = 0xA;  // Early Warning Interrupt Time Offset 0xA
                                     // = 8192 clockcycles @ 1024hz = 8 seconds
    WDT->INTENSET.bit.EW = 1;        // Enable early warning interrupt
    waitForWDTBitSync();             // ?? Needed here ??

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
    WDT->CTRL.bit.ENABLE = 1;
#endif
    waitForWDTBitSync();
}


void extendedWatchDogSAMD::disableWatchDog() {
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 0;
#else
    WDT->CTRL.bit.ENABLE = 0;
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
