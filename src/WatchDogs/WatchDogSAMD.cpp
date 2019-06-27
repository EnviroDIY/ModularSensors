/*
 *WatchDogSAMD.cpp

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
*/

#include "WatchDogSAMD.h"

// Be careful to use a platform-specific conditional include to only make the
// code visible for the appropriate platform.  Arduino will try to compile and
// link all .cpp files regardless of platform.
#if defined(ARDUINO_ARCH_SAMD)

extendedWatchDogSAMD::extendedWatchDogSAMD(){}
extendedWatchDogSAMD::~extendedWatchDogSAMD()
{
    disableWatchDog();
}

// One-time initialization of watchdog timer.
void extendedWatchDogSAMD::setupWatchDog(uint32_t resetTime_s)
{
    _resetTime_s = resetTime_s;
    // Longest interrupt is 16s, so we loop that as many times as needed
    extendedWatchDog::_barksUntilReset = _resetTime_s/16;

    MS_DBG(F("Setting up watch-dog timeout for"),
           _resetTime_s,
           F("with the interrupt firing"),
           extendedWatchDog::_barksUntilReset,
           F("times before the reset."));

#if defined(__SAMD51__)
    // SAMD51 WDT uses OSCULP32k as input clock now
    // section: 20.5.3
    OSC32KCTRL->OSCULP32K.bit.EN1K  = 1; // Enable out 1K (for WDT)
    OSC32KCTRL->OSCULP32K.bit.EN32K = 0; // Disable out 32K

    while(WDT->SYNCBUSY.reg);

    USB->DEVICE.CTRLA.bit.ENABLE = 0;         // Disable the USB peripheral
    while(USB->DEVICE.SYNCBUSY.bit.ENABLE);   // Wait for synchronization
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;       // Deactivate run on standby
    USB->DEVICE.CTRLA.bit.ENABLE = 1;         // Enable the USB peripheral
    while(USB->DEVICE.SYNCBUSY.bit.ENABLE);   // Wait for synchronization

#else  // SAMD21

    // Generic clock generator 2, divisor = 32 (2^(DIV+1))  = 4
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4);
    // Enable clock generator 2 using low-power 32.768kHz oscillator.
    // With /32 divisor above, this yields 1024Hz clock.
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |
                        GCLK_GENCTRL_GENEN |
                        GCLK_GENCTRL_SRC_OSCULP32K |
                        GCLK_GENCTRL_DIVSEL;
    while(GCLK->STATUS.bit.SYNCBUSY);
    // WDT clock = clock gen 2
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK2;
#endif
}


void extendedWatchDogSAMD::enableWatchDog()
{
    MS_DBG(F("Enabling watch dog..."));

    // Enable WDT early-warning interrupt
    NVIC_DisableIRQ(WDT_IRQn);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_SetPriority(WDT_IRQn, 0); // Top priority
    NVIC_EnableIRQ(WDT_IRQn);

#if defined(__SAMD51__)
    WDT->CTRLA.reg = 0; // Disable watchdog for config
    while(WDT->SYNCBUSY.reg);
#else
    WDT->CTRL.reg = 0; // Disable watchdog for config
    while(WDT->STATUS.bit.SYNCBUSY);
#endif

#if defined(__SAMD51__)
    WDT->INTFLAG.bit.EW      = 1;     // Clear interrupt flag
#endif

    WDT->CTRL.bit.WEN        = 0;     // Disable window mode
    WDT->CONFIG.bit.PER      = 0xB;   // Period = 16384 clockcycles @ 1024hz = 16 seconds
    WDT->EWCTRL.bit.EWOFFSET = 0x0;   // Early Warning Interrupt Time Offset 0x0 - 8 clockcycles = 7ms => trigger ISR
    WDT->INTENSET.bit.EW     = 1;     // Enable early warning interrupt

#if defined(__SAMD51__)
    while(WDT->SYNCBUSY.reg);
#else
    while(WDT->STATUS.bit.SYNCBUSY);
#endif

    resetWatchDog();
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 1;
    while(WDT->SYNCBUSY.reg);
#else
    WDT->CTRL.bit.ENABLE = 1;
    while(WDT->STATUS.bit.SYNCBUSY);
#endif

MS_DBG(F("Watch dog is enabled in normal mode at 16s with a interrupt 7ms before reset."),
       F("The interrupt should fire"),
       extendedWatchDog::_barksUntilReset,
       F("times before the system resets."));
}


void extendedWatchDogSAMD::disableWatchDog()
{
    NVIC_DisableIRQ(WDT_IRQn);        // disable IRQ
    NVIC_ClearPendingIRQ(WDT_IRQn);
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 0;
    while(WDT->SYNCBUSY.reg);
#else
    WDT->CTRL.bit.ENABLE = 0;
    while(WDT->STATUS.bit.SYNCBUSY);
#endif
}


void extendedWatchDogSAMD::resetWatchDog()
{
    extendedWatchDog::_barksUntilReset = _resetTime_s/16;
    // Write the watchdog clear key value (0xA5) to the watchdog
    // clear register to clear the watchdog timer and reset it.
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
#if defined(__SAMD51__)
    while(WDT->SYNCBUSY.reg);
#else
    while(WDT->STATUS.bit.SYNCBUSY);
#endif
}


void WDT_Handler(void)  // ISR for watchdog early warning
{
    extendedWatchDog::extendedWatchDog::_barksUntilReset--;  // Increament down the counter, makes multi cycle WDT possible
    if (extendedWatchDog::extendedWatchDog::_barksUntilReset<=0)
    {   // Software EWT counter run out of time : Reset
        WDT->CLEAR.reg = 0xFF;  // value different than WDT_CLEAR_CLEAR_KEY causes reset
        while(true);
    }
    else
    {
        WDT->INTFLAG.bit.EW = 1;              // Clear INT EW Flag
        WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY; // Clear WTD bit
     #if defined(__SAMD51__)
        while(WDT->SYNCBUSY.reg);
     #else
        while(WDT->STATUS.bit.SYNCBUSY); // Sync CTRL write
     #endif
    }
}

#endif
