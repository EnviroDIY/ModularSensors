# Modular Sensors Sleep Configurations<!--! {#page_sleep_config} -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [Modular Sensors Sleep Configurations](#modular-sensors-sleep-configurations)
  - [Steps for Putting All Boards to Sleep](#steps-for-putting-all-boards-to-sleep)
  - [Steps on Wake for All Boards](#steps-on-wake-for-all-boards)
  - [AVR](#avr)
    - [AVR Sleep Modes](#avr-sleep-modes)
    - [Steps in Putting an AVR board to sleep](#steps-in-putting-an-avr-board-to-sleep)
    - [Steps in Resuming Activity for an AVR board](#steps-in-resuming-activity-for-an-avr-board)
  - [SAMD51](#samd51)
    - [SAMD51 Sleep Modes](#samd51-sleep-modes)
    - [SAMD51 Pin Configuration During Sleep](#samd51-pin-configuration-during-sleep)
    - [SAMD51 Peripheral Clocks During Sleep](#samd51-peripheral-clocks-during-sleep)
    - [Steps in Putting an SAMD51 board to sleep](#steps-in-putting-an-samd51-board-to-sleep)
    - [Steps in Resuming Activity for a SAMD51 board](#steps-in-resuming-activity-for-a-samd51-board)
  - [SAMD21](#samd21)
    - [SAMD21 Sleep Modes](#samd21-sleep-modes)
    - [SAMD21 Pin Configuration During Sleep](#samd21-pin-configuration-during-sleep)
    - [Steps in Putting an SAMD21 board to sleep](#steps-in-putting-an-samd21-board-to-sleep)
    - [Steps in Resuming Activity for a SAMD21 board](#steps-in-resuming-activity-for-a-samd21-board)

<!--! @endif -->

## Steps for Putting All Boards to Sleep

All boards start their bedtime routine with these steps.

- Enable interrupts on the RTC
- Enable the wake ISR on the RTC wake pin
- Stop the I2C (Wire) library
  - **WARNING:** After stopping I2C, we can no longer communicate with  and I2C based RTCs!
Any calls to get the current time, change the alarm settings, reset the alarm flags, or any other event that involves communication with the RTC will fail!
  - For an AVR board, this disables the two-wire pin functionality and turns off the internal pull-up resistors.
    // For a SAMD board, this only disables the I2C sercom and does nothing with
    // the pins. The Wire.end() function does **NOT** force the pins low.
- Force the I2C pins to `LOW`
  - This only works if the SDA and SCL pins are defined in a boards pins.h file.
Not all boards define the SDA and SCL pins and those that do only define it for their "main" I2C/TWI interface
  - I2C devices have a nasty habit of stealing power from the SCL and SDA pins; this prevents that.
  - **WARNING:** Any calls to the I2C/Wire library when pins are forced low will cause an endless board hang.
- Disable the watch-dog timer
  - If it is enabled the watchdog timer will wake the board every ~8 seconds checking if the board has been inactive too long and needs to be reset.
  - We have to chose between allowing the watchdog to save us in a hand during sleep and saving power.
We've chosen to save power.

After this, the different processor types have different steps to finish preparing and finally falling asleep.

## Steps on Wake for All Boards

Immediately after their wake interrupt, each processor has different steps to resume operation.
But all processors finish their wake routine with these steps

- Re-enable the watch-dog timer
- Restart the I2C (Wire) interface
- Disable any unnecessary timeouts in the Wire library
  - These waits would be caused by a readBytes or parseX being called on wire after the Wire buffer has emptied.
The default stream functions - used by wire - wait a timeout period after reading the end of the buffer to see if an interrupt puts something into the buffer.
In the case of the Wire library, that will never happen and the timeout period is a useless delay.
- Detach RTC interrupt the from the wake pin
- Disable the RTC interrupt

## AVR<!--! {#sleep_config_avr} -->

### AVR Sleep Modes<!--! {#sleep_modes_avr} -->

> [!IMPORTANT]
> Modular Sensors uses **SLEEP_MODE_PWR_DOWN** sleep mode for AVR boards.

There are 5 AVR sleep modes supported by the Arduino core.
They are defined in the avr/sleep.h file.

The 5 sleep modes are:

- SLEEP_MODE_IDLE
  - the least power savings
- SLEEP_MODE_ADC
- SLEEP_MODE_PWR_SAVE
- SLEEP_MODE_STANDBY
- SLEEP_MODE_PWR_DOWN
  - the most power savings

### Steps in Putting an AVR board to sleep<!--! {#sleep_fxn_avr} -->

After completing the [steps for putting all boards to sleep](#steps-for-putting-all-boards-to-sleep), AVR boards finish their bedtime routine with these steps:

- Disable the onboard USB if it exists (ie, for a Leonardo)
  - Freeze the USB clock, turn off the USB PLL, and then disable the USB.
- Set the sleep mode to SLEEP_MODE_PWR_DOWN.
- Temporarily disables interrupts, so no mistakes are made when writing to the processor registers.
- Disable the processor ADC, (This must be disabled before the board will power down.)
- Turn off the brown-out detector, if possible.
- Disable all power-reduction modules (ie, the processor module clocks).
  - NOTE:  This only shuts down the various clocks on the processor via the power reduction register!
It does NOT actually disable the modules themselves or set the pins to any particular state!
This means that the I2C/Serial/Timer/etc pins will still be active and powered unless they are turned off prior to calling this function.
- Set the sleep enable bit.
- Wait until the serial ports have finished transmitting.
  - This isn't very important on AVR boards, but it's good practice.
- Actually put the processor into sleep mode.

### Steps in Resuming Activity for an AVR board<!--! {#wake_fxn_avr} -->

*Before* completing the [steps on wake for all boards](#steps-on-wake-for-all-boards), AVR boards start their wake routine with these steps:

- Temporarily disables interrupts, so no mistakes are made when writing to the processor registers.
- Re-enable all power modules (ie, the processor module clocks)
  - NOTE:  This only re-enables the various clocks on the processor!
The modules may need to be re-initialized after the clocks re-start.
- Clear the SE (sleep enable) bit.
- Re-enable the processor ADC
- Re-enables interrupts

## SAMD51<!--! {#sleep_config_samd51} -->

### SAMD51 Sleep Modes<!--! {#sleep_modes_samd51} -->

SAMD51 boards have multiple sleep configurations.

> [!IMPORTANT]
> Modular Sensors uses **STANDBY** sleep mode for the SAMD51.

The STANDBY mode is the lowest power configuration while keeping the state of the logic and the content of the RAM.
The HIBERNATE, BACKUP, and OFF modes do not retain RAM and a full reset occurs on wake. The watchdog timer also does not run in any sleep setting deeper than STANDBY.

- Idle
  - PM_SLEEPCFG_SLEEPMODE_IDLE_Val = 0x2
  - The CPU is stopped. Synchronous clocks are stopped except when requested. The logic is retained.
  - Wake-Up Sources:
    - Synchronous: interrupt generated on synchronous (APB or AHB) clock.
    - Asynchronous: interrupt generated on generic clock, external clock, or external event.
- Standby
  - PM_SLEEPCFG_SLEEPMODE_STANDBY_Val = 0x4
  - The CPU is stopped as well as the peripherals.
The logic is retained, and power domain gating can be used to fully or partially turn off the PDSYSRAM power domain.
  - Wake-Up Sources:
    - Synchronous interrupt only for peripherals configured to run in standby.
    - Asynchronous: interrupt generated on generic clock, external clock, or external event.
- Hibernate
  - PM_SLEEPCFG_SLEEPMODE_HIBERNATE_Val = 0x5
  - PDCORESW power domain is turned OFF.
The backup power domain is kept powered to allow few features to run (RTC, 32KHz clock sources, and wake-up from external pins).
The PDSYSRAM power domain can be retained according to software configuration.
  - Wake-Up Sources:
    - Hibernate reset detected by the RSTC
- Backup
  - PM_SLEEPCFG_SLEEPMODE_BACKUP_Val = 0x6
  - Only the backup domain is kept powered to allow few features to run (RTC, 32KHz clock sources, and wake-up from external pins).
The PDBKUPRAM power domain can be retained according to software configuration.
  - Wake-Up Sources:
    - Backup reset detected by the RSTC
- Off
  - PM_SLEEPCFG_SLEEPMODE_OFF_Val = 0x7
  - The entire device is powered off.
  - Wake-Up Sources:
    - External Reset

In hibernate and backup modes, "wake-up from external pins", refers **only** to the reset pin (`RESETN`) and dedicated RTC tamper detect pins (RTC INn [n=0..4]).
The datasheet is a bit misleading in this.
[See this Microchip FAQ article for details](https://microchip.my.site.com/s/article/SAM-E5x-D5x--Wakeup-from-Hibernate-Backup-sleep-modes-using-External-Interrupt)

RESETN is a dedicated pin.

[From Table 6-7 in the datasheet](https://onlinedocs.microchip.com/oxy/GUID-F5813793-E016-46F5-A9E2-718D8BCED496-en-US-14/GUID-6DC09BF1-A273-45E0-AB75-4E4181A06FC2.html?hl=tamper) the tamper detection pins are:

| RTC Signal | I/O Pin | VQFN 48 | TQFP/VQFN/WLCSP 64 | TQFP 100 | TFBGA 120 | TQFP 128 |
| :--------: | :-----: | :-----: | :----------------: | :------: | :-------: | :------: |
|    IN0     |  PB00   |    x    |       61/A7        |    97    |    A3     |   125    |
|    IN1     |  PB02   |   47    |       63/A8        |    99    |    A2     |   127    |
|    IN2     |  PA02   |    1    |       01/B8        |    1     |    A1     |    1     |
|    IN3     |  PC00   |    x    |         x          |    3     |    C1     |    3     |
|    IN4     |  PC01   |    x    |         x          |    4     |    C2     |    4     |
|    OUT     |  PB01   |    x    |       62/B7        |    98    |    B3     |   126    |

### SAMD51 Pin Configuration During Sleep<!--! {#sleep_pins_samd51} -->

> [!WARNING]
> All pin modes must be reset after the board wakes from sleep.
> If you are using any custom code (or a "complex loop") you **must** remember to reset your pin modes on wake.
> Reset pin modes by calling the `pinMode(pin_number, state);` and `digitalWrite(pin_number, value);` functions.

> [!NOTE]
> You can disable pin tri-state by calling `Logger::disablePinTristate(true)`.
> You can re-enable pin tri-state by calling `Logger::disablePinTristate(false)`.
> No pin modes are **not** changed when the `disablePinTristate()` function is called, only when the `systemSleep()` function is called.

To prevent power draw by any external pins during sleep, Modular Sensors sets all pins except the RTC interrupt pins to "tri-state."  Tri-state means that for *all* pins:

- the pin "direction" is set to input (`PORT->Group[port].DIRCLR.reg = pinMask;`)
- the pin input buffer is disabled (`PORT->Group[port].PINCFG[pin].bit.INEN   = 0;`)
- the internal pin pull-up and pull-down resistors are disabled (`PORT->Group[port].PINCFG[pin].bit.PULLEN = 0;`)

### SAMD51 Peripheral Clocks During Sleep<!--! {#sleep_peripherals_samd51} -->

> [!WARNING]
> This could potential be a problem if you are using an uncommon SAMD peripheral, especially if you are configuring it yourself.
> Some SAMD libraries that rely on the SAMD51 peripheral system configure the peripheral clocks on each call to the peripheral, but some do it only on when a `begin()` type function is called.
> If the peripheral you wish to use always reconfigures the clocks, this will never be a problem for you.
> If not, you will need to call whatever function is needed to reconfigure the clocks on wake.

> [!NOTE]
> You can disable disabling peripherals by calling `Logger::disablePeripheralShutdown(true)`.
> You can re-enable pin tri-state by calling `Logger::disablePeripheralShutdown(false)`.
> No peripheral settings are changed when the `disablePeripheralShutdown()` function is called, only when the `systemSleep()` function is called.

To decrease power use during sleep on the SAMD51, Modular Sensors explicitly disconnects all unused peripherals from the various clocks and and clock sources to prevent them from "[sleepwalking](https://onlinedocs.microchip.com/oxy/GUID-F5813793-E016-46F5-A9E2-718D8BCED496-en-US-14/GUID-FA7D618C-0F98-4A2C-9D24-669C4A3E3CA3.html)".
The SAMD51's standby mode turns off clocks *except those configured to be running sleepwalking tasks*.
By disconnecting all unused peripherals from a clock source, we ensure that the clocks aren't woken.

To do this, Modular Sensors disables generic clock generator 7 by disconnecting it from any oscillator source.
Then it ties all peripheral timer that are safe to shut down to the disabled generator.
This reduces power draw in sleep.

Some notes on what can and cannot be disabled:

- We CAN disable the EIC controller timer (4) because the controller clock source is set to OSCULP32K.
- We cannot disable the SERCOM peripheral timers for sleep because they're only reset with a begin(speed, config), which we do not call within the Modular Sensors library.
We force users to call the begin in their sketch so they can choose both the exact type of stream and the baud rate.
- We cannot disable the ADC peripheral timers because they're only set in the init for the ADC at startup.
- We CAN disable all of the timer clocks because they're reset every time they're used by SDI-12 (and others)

The numbers of all disabled peripherals are:

- 4,  5,  6,  9,  11, 12, 13, 14, 15, 16, 17,
- 18, 19, 20, 21, 22, 25, 26, 27, 28, 29, 30,
- 31, 32, 33, 38, 39, 42, 43, 44, 45, 46, 47

See [The SAMD clock file](@ref samd51_clock_other_libraries) for a list of which peripherals each of these numbers pertain to.

### Steps in Putting an SAMD51 board to sleep<!--! {#sleep_fxn_samd51} -->

After completing the [steps for putting all boards to sleep](#steps-for-putting-all-boards-to-sleep), SAMD51 boards finish their bedtime routine with these steps:

- Detach any USB devices (ie, the built in USB drivers for communication with a PC)
  - This is skipped if the TinyUSB library is called for some reason.
- Force all pins except the RTC wake and button pins to go to minimum power draw levels (tri-state)
- Configure GCLK7 to be disconnected from an oscillator source.
- Connect all unused peripherals to the source-less GCLK7.
- Wait for all serial ports to finish transmitting
  - This is crucial for the SAMD boards that will continuously wake if they have data remaining in the buffer.
- Clear the FPU interrupt because it can prevent us from sleeping.
  - [See this reference in the Circuit Python code.](https://github.com/maholli/circuitpython/blob/210ce1d1dc9b1c6c615ff2d3201dde89cb75c555/ports/atmel-samd/supervisor/port.c#L654)
- Set the sleep mode configuration to use STANDBY mode.
- Wait for the sleep mode setting to take
  - From datasheet 18.6.3.3: A small latency happens between the store instruction and actual writing of the SLEEPCFG register due to bridges.
Software must ensure that the SLEEPCFG register reads the desired value before executing a WFI instruction.
- Configure standby mode to retain all system RAM and disable fast wake.
- Wait for all the board to be ready to sleep.
  - From datasheet 18.6.3.3: After power-up, the MAINVREG low power mode takes some time to stabilize. O
Once stabilized, the INTFLAG.SLEEPRDY bit is set.
Before entering Standby, Hibernate or Backup mode, software must ensure that the INTFLAG.SLEEPRDY bit is set.
SRGD Note: I believe this only applies at power-on, but it's probably not a bad idea to check that the flag has been set.
- Call the data sync barrier (`__DSB();`) function to ensure outgoing memory accesses complete.
- Call wait for interrupts (`__WFI();`) to begin sleeping.
  - [See this link for tips on failing to sleep.](https://www.eevblog.com/forum/microcontrollers/crashing-through-__wfi/)

### Steps in Resuming Activity for a SAMD51 board<!--! {#wake_fxn_samd51} -->

*Before* completing the [steps on wake for all boards](#steps-on-wake-for-all-boards), SAMD51 boards start their wake routine with these steps:

- Re-attach the USB for PC communication
- Re-set the pin modes for the RTC wake pin, SD card SS pin, SD card power pin, button pin, and LED pin.
  - The pins were all set to tri-state to save power.

## SAMD21<!--! {#sleep_config_samd21} -->

### SAMD21 Sleep Modes<!--! {#sleep_modes_samd21} -->

> [!IMPORTANT]
> Modular Sensors uses **STANDBY** sleep mode for the SAMD21.

The SAMD21 has only two sleep modes, idle and standby, with standby having the most power saving.
Within the SAMD21 Arduino core and boot loaders, standby mode is referred to as SLEEPDEEP.

### SAMD21 Pin Configuration During Sleep<!--! {#sleep_pins_samd21} -->

The pin configurations for the SAMD21 are identical to those described above for the SAMD51.

### Steps in Putting an SAMD21 board to sleep<!--! {#sleep_fxn_samd21} -->

After completing the [steps for putting all boards to sleep](#steps-for-putting-all-boards-to-sleep), SAMD21 boards finish their bedtime routine with these steps:

- Detach any USB devices (ie, the built in USB drivers for communication with a PC)
  - This is skipped if the TinyUSB library is called for some reason.
- Force all pins except the RTC wake and button pins to go to minimum power draw levels (tri-state)
- Wait for all serial ports to finish transmitting
  - This is crucial for the SAMD boards that will continuously wake if they have data remaining in the buffer.
- Configure flash to **not** power down when in sleep.
  - Datasheet Errata 1.14.2 says this is required.
- Disable the systick interrupt.
  - See <https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode>.
  - Due to a hardware bug on the SAMD21, the SysTick interrupts become active before the flash has powered up from sleep, causing a hard  fault.
To prevent this the SysTick interrupts are disabled before entering sleep mode.
- Set the sleep mode configuration to use STANDBY mode.
- Call the data sync barrier (`__DSB();`) function to ensure outgoing memory accesses complete.
- Call wait for interrupts (`__WFI();`) to begin sleeping.
  - [See this link for tips on failing to sleep.](https://www.eevblog.com/forum/microcontrollers/crashing-through-__wfi/)

### Steps in Resuming Activity for a SAMD21 board<!--! {#wake_fxn_samd21} -->

*Before* completing the [steps on wake for all boards](#steps-on-wake-for-all-boards), SAMD21 boards start their wake routine with these steps:

- Re-enable the systick interrupt
- Re-attach the USB for PC communication
- Re-set the pin modes for the RTC wake pin, SD card SS pin, SD card power pin, button pin, and LED pin.
  - The pins were all set to tri-state to save power.

<!-- cspell: ignore SLEEPCFG SLEEPMODE INTFLAG RSTC INEN PULLEN RESETN VQFN TQFP WLCSP TFBGA DIRCLR MAINVREG systick -->
