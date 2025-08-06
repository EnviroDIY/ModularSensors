# The SAMD clock system<!--! {#page_samd_clocks} -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [The SAMD clock system](#the-samd-clock-system)
  - [Terms](#terms)
  - [SAMD21](#samd21)
    - [The SAMD21 clock system](#the-samd21-clock-system)
    - [SAMD21 Clock Requirements Relevant to ModularSensors](#samd21-clock-requirements-relevant-to-modularsensors)
    - [SAMD21 Settings at Power On](#samd21-settings-at-power-on)
    - [SAMD21 Arduino Core Setup Clock Generator Configuration](#samd21-arduino-core-setup-clock-generator-configuration)
    - [SAMD21 Arduino Core Library Clock Configuration](#samd21-arduino-core-library-clock-configuration)
    - [Clocks Used by Non-Core Libraries for the SAMD21](#clocks-used-by-non-core-libraries-for-the-samd21)
  - [SAMD51 and SAME51](#samd51-and-same51)
    - [The SAMD51 clock system](#the-samd51-clock-system)
    - [SAMD51 Clock Requirements Relevant to ModularSensors](#samd51-clock-requirements-relevant-to-modularsensors)
    - [SAMD51 Settings at Power On](#samd51-settings-at-power-on)
    - [SAMD51 Arduino Core Setup Clock Generator Configuration](#samd51-arduino-core-setup-clock-generator-configuration)
      - [Resulting generic clock speeds](#resulting-generic-clock-speeds)
    - [SAMD51 Arduino Core Library Clock and Peripheral Configuration](#samd51-arduino-core-library-clock-and-peripheral-configuration)
    - [Clocks Used by Non-Core Libraries for the SAMD51](#clocks-used-by-non-core-libraries-for-the-samd51)
    - [Summary of Peripheral Clock Assignments](#summary-of-peripheral-clock-assignments)
- [The Non-Volatile Interrupt Controller (NVIC)](#the-non-volatile-interrupt-controller-nvic)
  - [NVIC Overview](#nvic-overview)
  - [Priority Levels](#priority-levels)
  - [Configuring Interrupt Priority](#configuring-interrupt-priority)
  - [Enabling IRQs Globally](#enabling-irqs-globally)
  - [NVIC-Specific Functions](#nvic-specific-functions)
    - [Datasheet Table 10-1. Interrupt Line Mapping](#datasheet-table-10-1interrupt-line-mapping)
  - [Exception and Interrupt Handlers](#exception-and-interrupt-handlers)
  - [NVIC Interrupts Defined in the Adafruit SAMD U2F Bootloader and Arduino Core](#nvic-interrupts-defined-in-the-adafruit-samd-u2f-bootloader-and-arduino-core)
    - [SAMD51 NVIC](#samd51-nvic)
    - [SAMD21 NVIC](#samd21-nvic)
  - [NVIC Interrupts Defined in Other Popular Libraries](#nvic-interrupts-defined-in-other-popular-libraries)

<!--! @endif -->

## Terms

Essentially every microprocess or computer needs a consistent way of their own speed of operation so they can communicate with internal components and external devices.

An *[oscillator](https://en.wikipedia.org/wiki/Electronic_oscillator)* is a circuit that makes an oscillating signal - ie, it switches back and forth between to states at a consistent rate.
The oscillator works like a metronome.
An oscillator alone does not keep track of time; it ticks, but it doesn't count how many ticks have passed.

SAMD processors use these types of oscillators:

- [crystal oscillators](https://en.wikipedia.org/wiki/Crystal_oscillator) - which are tiny pieces of quartz that vibrate under current.
This is just like the crystals in a quartz watch.
- Digital frequency locked loops (DFLL) and fractional digital phase locked loops (FDPLL) - these [phase locked loops (PLL)](https://wirelesspi.com/how-a-frequency-locked-loop-fll-works/) use a reference clock (like the external crystal) to create a consistent (faster) output frequency.
- Ultra-low-power oscillators - circuits which generate the same frequecency fibrations as a crystal power but using lower power consumption to get a less consistent signal.

For any of the oscillors to be useful in keeping track of time, they need to be connected to something else that will count the number of ticks.
The oscillator acts as the source for the clock/counter.
There can also be a 'divisor' between the ticking source and the counter - that is, the counter can record every 'x' ticks instead of every single tick.

See also: <https://blog.thea.codes/understanding-the-sam-d21-clocks/>

## SAMD21<!--! {#samd21_clocks} -->

### The SAMD21 clock system<!--! {#samd21_clock_system} -->

From the SAMD21 Datasheet 14.1:

> The clock system on the SAM D21 consists of :
>
> - *Clock sources*, controlled by SYSCTRL
>   – A clock source provides a time base that is used by other components, such as Generic Clock Generators.
> Example clock sources are the internal 8MHz oscillator(OSC8M), External crystal oscillator(XOSC) and the Digital frequency locked loop (DFLL48M).
> - *Generic Clock Controller(GCLK)* which controls the clock distribution system, made up of:
>   - *Generic Clock Generators*: These are programmable prescalers that can use any of the system clock sources as a time base.  The Generic Clock Generator 0 generates the clock signal GCLK_MAIN, which is used by the Power Manager, which in turn generates synchronous clocks.
>     - Generic clock generators are configured with `GCLK->GENCTRL`
>   - *Generic Clocks*: These are clock signals generated by Generic Clock Generators and output by the Generic Clock Multiplexer, and serve as clocks for the peripherals of the system.  Multiple instances of a peripheral will typically have a separate Generic Clock for each instance. Generic Clock 0 serves as the clock source for the DFLL48M clock input (when multiplying another clock source).
>     - Generic clocks are configured with `GCLK->CLKCTRL`
> - Power Manager (PM)
>   - The PM generates and controls the synchronous clocks on the system.
> This includes the CPU, bus clocks (APB, AHB) as well as the synchronous (to the CPU) user interfaces of the peripherals.  It contains clock masks that can turn on/off the user interface of a peripheral as well as prescalers for the CPU and bus clocks

### SAMD21 Clock Requirements Relevant to ModularSensors<!--! {#samd21_clock_reqs} -->

The watchdog's peripheral clock must be attached to a currently-on clock source so it can tell how much time has passed and whether it needs to bite.
The watchdog peripheral clock is not configured by the core.
The flow from a clock source to the WDT on the SAMD21 is:

- Enable the clock source and the WDT periperhal in the power management system
- Configure said a clock source to run in standby
- Configure a divisor between the above source clock and a generic clock generators
- Configure a generic clock to tie the watchdog's peripheral clock to the above generic clock generator
- Configure the watchdog itself.

The external interrupt controller must also be attached to a currently-on clock to tell the difference between rising and HIGH or falling and LOW interrupts.
If the external interupt controller is not attached to a running clock, then interrupts will not work!
Thus, if the clock source for interrupts is not running in standby, the interrupts will not be able to wake the device.
The flow from a clock source to the EIC is the same as that for the WDT.

### SAMD21 Settings at Power On<!--! {#samd21_clock_power_on} -->

After a power-on reset, the clock generators for peripherals default to:

- RTC: GCLK0
- WDT: GCLK2
- Anything else: GCLK0
- See 14.8 in the SAMD21 Datasheet

See [section 13.7 of the datasheet](https://onlinedocs.microchip.com/oxy/GUID-F5813793-E016-46F5-A9E2-718D8BCED496-en-US-13/GUID-4F945BA9-D138-4F79-AA1A-5CFF0E67A977.html)

### SAMD21 Arduino Core Setup Clock Generator Configuration<!--! {#samd21_clock_core_setup} -->

[Within the SAMD core](https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/startup.c#L311) SystemInit() in startup.c configures clocks with these steps:

- Enable XOSC32K clock (external on-board 32.768Hz oscillator) or OSC32K (if crystalless).
  - This will be used as DFLL48M reference.
  - SystemInit() uses a default start-up configuration of 0x6 for the (X)OSC (65536 OSCULP32K clock cycles + 3 (X)OSC32K clock cycles = 2000092μs ~= 2s before PCLKSR.XOSC32KRDY is set.)

> [!NOTE]
> The SystemInit() function **blocks** while waiting for oscillator stabilization (~2s).

- Put XOSC32K or OSC32K as source of Generic Clock Generator 1
- Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0 (DFLL48M reference)
- Enable DFLL48M clock in *closed loop* mode, if there is an external crystal available, or in open loop mode if crystalless
- Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
- Modify prescaler value of OSCM to have 8MHz
- Put OSC8M as source for Generic Clock Generator 3

Resulting generic clock generator speeds:

- GCLKGEN0 = 48 MHz; sourced from DFLL48M, which does not run in standby.
- GCLKGEN1 = 32kHz; sourced from XOSC32K or OSC32K, which does not run in standby.
- GCLKGEN2 = *not configured*
- GCLKGEN3 = 8 MHz; sourced from OSC8M, which does not run in standby.
- GCLKGEN4-GCLKGEN8 = *not configured*

### SAMD21 Arduino Core Library Clock Configuration<!--! {#samd21_clock_core_configs} -->

- [WInterrupts.c](https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/WInterrupts.c#L56)
  - Configures the external interrupt controller (EIC) clock (`GCM_EIC`) to use GCLKGEN0 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN0
- [wiring_analog.c](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/wiring_analog.c#L621)
  - Configures timer counter clocks (`GCM_TC4_TC5` or `GCM_TC6_TC7`, depending on the analog pin) to use GCLKGEN0 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN0
- [wiring.c](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/wiring.c#L164)
  - Configures the ADC (`GCM_ADC`) and DAC (`GCM_DAC`) to use GCLKGEN0 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN0
- [SERCOM](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/SERCOM.cpp#L872)
  - Configures the various SERCOM clocks to use GCLKGEN0 at 48 MHz
    - GCM_SERCOM0_CORE -> GCM_SERCOM5_CORE, depending on how many SERCOM's are defined in variant.h
  - Does not change the source or other configuration for GCLKGEN0
- [Tone](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/Tone.cpp#L93)
  - Configures timer counter 4 and 5 clocks (`GCM_TC4_TC5`) to use GCLKGEN0 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN0
- [I2S](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/libraries/I2S/src/I2S.cpp#L449)
  - Configures the I2S's generic clocks (`I2S_GCLK_ID_0` and `I2S_GCLK_ID_1`) to use the generic clock generator specified for I2S in variant.h (likely GCLKGEN3)
  - Sets the source of the specified generic clock generator to be the DFLL48 or the OSC8M, depending on the I2S speed
- USB (host, core, TinyUSB)
  - Configures the USB (`GCLK_USB` = 0x6) to use GCLKGEN0 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN0
  - NOTE: For some reason, the code uses 0x6 directly instead of the `GCLK_USB` macro for the generic clock selection ID
- [Servo](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/libraries/Servo/src/samd/Servo.cpp#L190)
  - Configures timer counter clocks (specific clock depending on the analog pin) to use GCLKGEN0 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN0

### Clocks Used by Non-Core Libraries for the SAMD21<!--! {#samd21_clock_other_libraries} -->

- [RTCZero](https://github.com/arduino-libraries/RTCZero/)
  - Configures the RTC's generic clock (GCM_RTC) to use generic clock generator 2
  - Sets the source for GCKL2 as a 32k oscillator
    - external preferred, internal ultra-low power if cyrstalless
    - *Forces the external 32K oscillator to remain on in standby*
  - Uses a 32x divisor to get 1024Hz(ish) clock for time keeping.
- [Adafruit SleepDog](https://github.com/adafruit/Adafruit_SleepyDog)
  - Configures the WDT's generic clock (GCM_WDT) to use generic clock generator 2.
  - Sets the source for GCKL2 as the internal ultra-low power 32k oscillator
  - Uses a 32x divisor to get a 1024Hz(ish) clock to manage the watchdog.

> [!NOTE]
> RTCZero and Adafruit's sleepy dog choose different sources for GCKL2!

- [Arduino Low Power](https://github.com/arduino-libraries/ArduinoLowPower)
  - Configures the EIC and ADC's generic clocks (GCM_EIC and GCM_ADC) to use generic clock generator 6
  - Sets the source for GCKL6 as the internal ultra-low power 32k oscillator
  - Does *NOT* use any clock divisor

> [!NOTE]
> The ZeroPowerManager library changes other clocks (including the main clock source, GCLK_MAIN, which is always sourced from GCLKGEN[0]) to reduce power draw.
> Changing the GCLK_MAIN configuration will cause some functions like delay() to operate incorrectly.
> To avoid any confusing with delay(), we're not going to change anything with GCLK0.
> This means we won't be in the lowest power state like that offered by ZeroPowerManager
> @todo: Revisit this decision

- [EnviroDIY SDI-12](https://github.com/EnviroDIY/Arduino-SDI-12)
  - Configures the TCC2/TC3 clock (GCM_TCC2_TC3) to use generic clock generator 4 (`GENERIC_CLOCK_GENERATOR_SDI12` = 4)
  - Sets the source for GCKL4 as the DFLL48M - which is in-turn coming from "main" clock set up in SystemInit() in startup.c.
  - Uses a 6x divisor.
- [Modular Sensors (this library)](https://github.com/EnviroDIY/ModularSensors)
  - Configures the EIC and WDT's generic clocks (GCM_EIC and GCM_WDT) to use generic clock generator 5 (`GENERIC_CLOCK_GENERATOR_MS` = 5)
  - Sets the source for GCKLGEN5 as the internal ultra-low power 32k oscillator.
  - Uses a 32x divisor to get a 1024Hz(ish) clock to manage the watchdog and EIC.

## SAMD51 and SAME51<!--! {#samd51_clocks} -->

### The SAMD51 clock system<!--! {#samd51_clock_system} -->

[From 13.1 of the SAMD51 Datasheet](https://onlinedocs.microchip.com/oxy/GUID-F5813793-E016-46F5-A9E2-718D8BCED496-en-US-13/GUID-FC6F58EC-FCDA-4478-AE60-45F2E160FF94.html))

> The SAM D5x/E5x clock system consists of:
>
> - *Clock sources*, i.e. oscillators controlled by OSCCTRL and OSC32KCTRL
>   - A clock source provides a time base that is used by other components, such as Generic Clock Generators.  Example clock sources include the external crystal oscillators (XOSC0 and XOSC1) and the Digital Frequency Locked Loop (DFLL48M).
> - *Generic Clock Controller (GCLK)*, which generates, controls and distributes asynchronous clocks consisting of:
>   - *Generic Clock Generators:* These are programmable prescalers that can use any of the system clock sources as a time base. The Generic Clock Generator 0 generates the clock signal GCLK_MAIN, which is used by the Power Manager and the Main Clock (MCLK) module, which in turn generates synchronous clocks.
> - *Generic Clocks:* These are clock signals generated by Generic Clock Generators and output by the Peripheral Channels, and serve as clocks for the peripherals of the system. Multiple instances of a peripheral will typically have a separate Generic Clock for each instance. Generic Clock 0 serves as the clock source for the DFLL48M clock input (when multiplying another clock source).
> - *Main Clock Controller (MCLK)*
>   - The MCLK generates and controls the synchronous clocks on the system. This includes the CPU, bus clocks (APB, AHB) as well as the synchronous (to the CPU) user interfaces of the peripherals. It contains clock masks that can turn on/off the user interface of a peripheral as well as prescalers for the CPU and bus clocks.

### SAMD51 Clock Requirements Relevant to ModularSensors<!--! {#samd51_clock_reqs} -->

The SAMD51 WDT uses the 1K output from the OSCULP32k; there are no other possible clock sources for the WDT.
No separate clock generator or peripheral clock configuration is needed.
The OSCULP32k cannot be turned off in standby.

The external interrupt controller must be attached to a currently-on clock to tell the difference between rising and HIGH or falling and LOW interrupts.
If the external interupt controller is not attached to a running clock, then interrupts will not work!
One the SAMD51, the EIC *can* be configured to work directly with the OSCULP32k without having to setup a separate clock generator.
If the EIC is not configured to connect directly to the OSCULP32k, the GCLK it is configured to must be set to run in standby.
The path from a clock source to the EIC on the SAMD51 is very similar to that of the SAMD21.

### SAMD51 Settings at Power On<!--! {#samd51_clock_power_on} -->

[See section 13.7 of the datasheet.](https://onlinedocs.microchip.com/oxy/GUID-F5813793-E016-46F5-A9E2-718D8BCED496-en-US-13/GUID-4F945BA9-D138-4F79-AA1A-5CFF0E67A977.html)

### SAMD51 Arduino Core Setup Clock Generator Configuration<!--! {#samd51_clock_core_setup} -->

[Within the SAMD core](https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/startup.c#L57) SystemInit() in startup.c configures clocks with these steps:

- Enable XOSC32K clock (external on-board 32.768Hz oscillator) if a crystal is available
  - Also enable the 32k output, the 1k output, standard gain mode, and configure the oscillator pins for a crystal connected between XIN32 and XOUT32
  - Sets start-up time to 0x0 = 2048 clock cycles ~= 65.592ms
  - Sets on-demand and run-in-standby both to 0, so the external 32k oscillator will always run in active or idle mode and will only run if requested by peripheral in standby mode (table 29-1)
- Set XOSC32K or OSC32K (if crystalless) as source of Generic Clock Generator 3
- Set OSCULP32K (internal ultra-low power 32.768Hz oscillator) as the source for Generic Clock Generator 0
  - NOTE: The default power-on source of generic clock generator 0 is the DFLL48.
  - NOTE: Farther down, generic clock generator 0 is resourced from DPLL0 (at 120MHz)
- Configure the DFLL48M clock in open loop mode (no reference clock)
  - Source generic clock generator 5 (`GENERIC_CLOCK_GENERATOR_1M`) from the DFLL with a divisor of 48
  - Sets on-demand and run-in-standby both to 0, so the DFLL48M will always run in active or idle mode and will not run in standby mode (28.8.7, slightly different than table 29-1 for (X)OSC32K)
- Configure DPLL peripheral clocks
  - Configure PLL0 at 120MHz (or F_CPU), using generic clock generator 5 (DFLL48M / 48) as the reference clock
  - Configure PLL1 at 100MHz, using generic clock generator 5 (DFLL48M / 48) as the reference clock
  - Sets on-demand and run-in-standby both to 0 for both FDPLL, so the PLLs will always run in active or idle mode and will not run in standby mode (28.8.12)
- Configure other generic clock generators to use for peripheral clocks
  - Source generic clock generator 1 from the DFLL48M with no divisor
    - 48MHz clock generator; used for the USB and "stuff"
  - Source generic clock generator 2 from the PLL1 at 100MHz with no divisor
    - 100MHz clock generator; used for "other peripherals"
  - Source generic clock generator 4 from the DFLL48M at 48MHz with 4x divisor
    - 12MHz clock generator; used for the DAC
- Set the main clock source as the source for the main generic clock generator
  - `MAIN_CLOCK_SOURCE` = `GCLK_GENCTRL_SRC_DPLL0`
  - `GENERIC_CLOCK_GENERATOR_MAIN` = 0
  - This is resetting the source of generic clock generator 0 to be the DPLL0 at 120MHz, it previously had been set to the OSCULP32K.

#### Resulting generic clock speeds

- GCLK0 = F_CPU (likely 120MHz, but could be different depending on variant.h); sourced from DLPP0, which does not run in standby.
- GCLK1 = 48 MHz; sourced from DFLL48M, which does not run in standby.
- GCLK2 = 100 MHz; sourced from DLPP1, which does not run in standby.
- GCLK3 = 32 kHz; sourced from XOSC32K, which does run in standby if requested by peripheral (I think?)
- GCLK4 = 12 MHz; sourced from DFLL48M, which does not run in standby.

### SAMD51 Arduino Core Library Clock and Peripheral Configuration<!--! {#samd51_clock_core_configs} -->

- [WInterrupts.c](https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/WInterrupts.c#L48)
  - Configures the external interrupt controller (EIC) clock (`EIC_GCLK_ID`) to use GCLKGEN2 at 100 MHz
  - Does not change the source or other configuration for GCLKGEN2
- [wiring_analog.c](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/wiring_analog.c#L505)
  - Configures timer counter clocks (specific clock depending on the analog pin) to use GCLKGEN0 at 120 MHz
  - Does not change the source or other configuration for GCLKGEN0
- [wiring.c](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/wiring.c#L119)
  - Configures the ADC (`ADC0_GCLK_ID` and `ADC1_GCLK_ID`) to use GCLKGEN1 at 48 MHz
  - Does not change the source or other configuration for GCLKGEN1
  - Configures the DAC (`DAC_GCLK_ID`) to use GCLKGEN4 at 12 MHz
  - Does not change the source or other configuration for GCLKGEN4
- [SERCOM](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/SERCOM.cpp#L803)
  - Configures the various SERCOM clocks to various sources depending on the speed of the SERCOM peripheral
    - SERCOM0_GCLK_ID_CORE/SERCOM0_GCLK_ID_SLOW -> SERCOM7_GCLK_ID_CORE/SERCOM7_GCLK_ID_SLOW, depending on how many SERCOM's are defined in variant.h
  - Does not change the source or other configuration of any of the clock generators
- [Tone](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/cores/arduino/Tone.cpp#L90)
  - Configures timer counter 0 clock (`TONE_TC_GCLK_ID` = `TC0_GCLK_ID`) to use GCLKGEN0 at 120 (or F_CPU) MHz
  - Does not change the source or other configuration for GCLKGEN0
- [I2S](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/libraries/I2S/src/I2S.cpp#L449)
  - Configures the I2S's generic clocks (`I2S_GCLK_ID_0` and `I2S_GCLK_ID_1`) to use the generic clock generator specified for I2S in variant.h (likely GCLKGEN3)
  - Sets the source of the specified generic clock generator to be the DFLL48 or the OSC8M, depending on the I2S speed
- USB (host, core, TinyUSB)
  - Configures the USB (`USB_GCLK_ID`) to use GCLKGEN0 at 120 (or F_CPU) MHz
  - Does not change the source or other configuration for GCLKGEN0
- [Servo](https://github.com/adafruit/ArduinoCore-samd/blob/bb552819ba79fbda21d868dd1c838d52bce383ba/libraries/Servo/src/samd/Servo.cpp#L186)
  - Configures timer counter clocks (specific clock depending on the analog pin) to use GCLKGEN0 at 120 MHz
  - Does not change the source or other configuration for GCLKGEN0

Thus, if the clock source for interrupts is not running in standby, the interrupts will not be able to wake the device.
In [WInterrupts.c](https://github.com/adafruit/ArduinoCore-samd/blob/ce20340620bfd9c545649ee5c4873888ee0475d0/cores/arduino/WInterrupts.c#L48) in the Adafruit SAMD core, generic clock generator 2 (from the PLL1 at 100MHz with no divisor) is used for the EIC peripheral.
The Arduino core does *NOT* configure the generic clock generator 0 (ie GCLK_MAIN = DFLL48M) to stay awake in standby!

### Clocks Used by Non-Core Libraries for the SAMD51<!--! {#samd51_clock_other_libraries} -->

- [RTCZero](https://github.com/arduino-libraries/RTCZero/)
  - The RTC of the SAMD51 is sourced directly from an oscillator, not via the generic clock generator system. This library only configures the RTC oscillator on the SAMD51.
- [Adafruit SleepDog](https://github.com/adafruit/Adafruit_SleepyDog)
  - The WDT on the SAMD51 is directly attached to OSCULP32K. This library doesn't make any clock changes for the SAMD51.
- [Arduino Low Power](https://github.com/arduino-libraries/ArduinoLowPower)
  - This library doesn't support the SAMD51
- [EnviroDIY SDI-12](https://github.com/EnviroDIY/Arduino-SDI-12)
  - Configures the TC2 clock (`TC2_GCLK_ID`) to use generic clock generator 6 (`GENERIC_CLOCK_GENERATOR_SDI12` = 6)
  - Sets the source for GCKL6 as the DPLL0 at 120MHz
  - Uses a 15x divisor between the DPLL0 and GCKL6.
- [Modular Sensors (this library)](https://github.com/EnviroDIY/ModularSensors)
  - This library sets OSCULP32K as the source for the EIC and *disables* GCLK_EIC.
  - This library also resets GCLK7 so it is disconnected from any source.
  - This library disables the following peripeheral timers and ties them to the disabled GCLK7:
    - 4 - GCLK_EIC
    - 5 - GCLK_FREQM_MSR
    - 6 - GCLK_FREQM_REF
    - 9 - GCLK_TC0/GCLK_TC1
    - 11 - GCLK_EVSYS0
    - 12 - GCLK_EVSYS1
    - 13 - GCLK_EVSYS2
    - 14 - GCLK_EVSYS3
    - 15 - GCLK_EVSYS4
    - 16 - GCLK_EVSYS5
    - 17 - GCLK_EVSYS6
    - 18 - GCLK_EVSYS7
    - 19 - GCLK_EVSYS8
    - 20 - GCLK_EVSYS9
    - 21 - GCLK_EVSYS10
    - 22 - GCLK_EVSYS11
    - 25 - GCLK_TCC0/GCLK_TCC1
    - 26 - GCLK_TC2/GCLK_TC3
    - 27 - GCLK_CAN0
    - 28 - GCLK_CAN1
    - 29 - GCLK_TCC2/GCLK_TCC3
    - 30 - GCLK_TC4/GCLK_TC5
    - 31 - GCLK_PDEC
    - 32 - GCLK_AC
    - 33 - GCLK_CCL
    - 38 - GCLK_TCC4
    - 39 - GCLK_TC6/GCLK_TC7
    - 42 - GCLK_DAC
    - 43 - GCLK_I2S
    - 44 - GCLK_I2S
    - 45 - GCLK_SDHC0
    - 46 - GCLK_SDHC1
    - 47 - GCLK_CM4_TRACE

### Summary of Peripheral Clock Assignments<!--! {#samd51_clock_summary} -->

- GCLK_SERCOM[0..7]_SLOW/GCLK_SDHC0_SLOW/GCLK_SDHC0_SLOW (3) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_EIC (4) - GCLKGEN0 (F_CPU; sourced from DLPP0) - WInterrupts.c
  - *This is disabled by Modular Sensors!*
- GCLK_TC0/GCLK_TC1 (9) - GCLKGEN0 (F_CPU; sourced from DLPP0) - Tone.cpp, Servo.cpp, wiring_analog.c
- GCLK_USB (10) - GCLKGEN0 (F_CPU; sourced from DLPP0) - samd21_host.c, USBCore.cpp, Adafruit_TinyUSB_samd.cpp
- GCLK_SERCOM2_CORE (23) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_SERCOM3_CORE (24) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_TC2/GCLK_TC3 (26) - GCLKGEN0 (F_CPU; sourced from DLPP0) - Servo.cpp, wiring_analog.c *potential conflict with SDI-12, be cautious*
- GCLK_TC2/GCLK_TC3 (26) - GCLKGEN6 (F_CPU; sourced from DLPP0) - SDI12_boards.cpp *potential conflict with servo, be cautious*
- GCLK_TC4/GCLK_TC5 (30) - GCLKGEN0 (F_CPU; sourced from DLPP0) - Servo.cpp, wiring_analog.c
- GCLK_SERCOM4_CORE (34) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_SERCOM5_CORE (35) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_SERCOM6_CORE (36) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_SERCOM7_CORE (37) - potentially any of GCLK0-GCLK4 - SERCOM.cpp
- GCLK_TC6/GCLK_TC7 (39) - GCLKGEN0 (F_CPU; sourced from DLPP0) - Servo.cpp, wiring_analog.c
- GCLK_ADC0 (40) - GCLKGEN1 (48 MHz; sourced from DFLL48M) - wiring.c
- GCLK_ADC12 (41) - GCLKGEN1 (48 MHz; sourced from DFLL48M) - wiring.c
- GCLK_DAC (42) - GCLKGEN4 (12 MHz; sourced from DFLL48M) - wiring.c
- GCLK_I2S[0] (43) - GCLKGEN3 (32 kHz; sourced from XOSC32K) *most likely* - I2S.cpp
- GCLK_I2S[1] (44) - GCLKGEN3 (32 kHz; sourced from XOSC32K) *most likely* - I2S.cpp

# The Non-Volatile Interrupt Controller (NVIC)

This entire section is copyied selections from [Microchip's developer help on the NVIC](https://developerhelp.microchip.com/xwiki/bin/view/products/mcu-mpu/32bit-mcu/sam/samd21-mcu-overview/samd21-processor-overview/samd21-nvic-overview/).

## NVIC Overview

## Priority Levels

Cortex-M0+ processors support three fixed highest priority levels for three of the system exceptions, and four programmable levels for all other exceptions, including interrupts.
The four possible programmable priority levels are 0x00 (highest urgency), 0x40, 0x80, and 0xC0 (lowest urgency).

- Out of reset, all interrupts and exceptions with configurable priority have the same default priority of 0x00. This priority number represents the highest-possible interrupt urgency.
- If two exceptions happen at the same time and they have the same programmed priority level, the exception with the lower CMSIS IRQ number will be processed first.

## Configuring Interrupt Priority

CMSIS provides a number of functions for NVIC control, including the following for setting priority:
`void NVIC_SetPriority(IRQn_t IRQn, uint32_t priority);`
Where priority values (0, 1, 2, 3) correspond to interrupt priority register (IPRx) settings 0x00, 0x40, 0x80, 0xC0.
The CMSIS IRQ numbers (the peripheral-interrupt-to-CMSIS-IRQ-number mapping) are defined in the processor specifc include files within the bootloader.
That is [this file](https://github.com/adafruit/uf2-samdx1/blob/master/lib/samd51/include/samd51n19a.h) for the UF2 bootloader for the SAMD51 variant the EnviroDIY Stonefly is based on.

## Enabling IRQs Globally

Often in real-time embedded programming, it is necessary to perform certain operations atomically to prevent data corruption. The simplest way to achieve the atomicity is to briefly disable and re-enable interrupts. The PRIMASK CPU-core register prevents activation of all exceptions with configurable priority.

```cpp
/* Disable **All** Interrupts */
void __disable_irq(void);

/* Enable **ALL** Interrupts */
void __enable_irq(void);
```

## NVIC-Specific Functions

CMSIS provides a number of functions for NVIC control, such as:

```cpp
/* Set the priority for an interrupt */
void NVIC_SetPriority(IRQn_t IRQn, uint32_t priority);

/* Enable a device specific interrupt */
void NVIC_EnableIRQ (IRQn_Type IRQn);

/* Disable a device specific interrupt */
void NVIC_DisableIRQ (IRQn_Type IRQn)
```

[CMSIS functions](https://arm-software.github.io/CMSIS_5/Core/html/group__NVIC__gr.html) associated with NVIC are located in the core_cm0plus.h header file.
Functions are implemented as inline code.

From the SAMD51 datasheet, here are the interrupt line mapping numbers for interrupts in the NVIC
### [Datasheet Table 10-1. Interrupt Line Mapping](https://onlinedocs.microchip.com/oxy/GUID-F5813793-E016-46F5-A9E2-718D8BCED496-en-US-14/GUID-DA8CB38A-18D7-4512-965B-BB439142B281.html?hl=icpr#GUID-DA8CB38A-18D7-4512-965B-BB439142B281__TABLE_CYS_KLX_S5)

<table data-ofb data-cols="3">
  <caption data-caption-side="top" data-is-repeated="true">
    <span><span>Table 10-1. </span></span
    ><span>Interrupt Line Mapping</span>
  </caption>
  <colgroup>
    <col />
    <col />
    <col />
  </colgroup>
  <thead>
    <tr>
      <th>Module</th>
      <th>Source</th>
      <th>Line</th>
      <th>Enabled?</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>EIC NMI - External Interrupt Control</td><td>NMI</td><td>NMI</td><td>enabled</td></tr>
    <tr><td>PM - Power Manager</td><td>SLEEPRDY</td><td>0</td><td>disabled</td></tr>
    <tr><td>MCLK - Main Clock</td><td>CKRDY</td><td>1</td><td>disabled</td></tr>
    <tr>
      <td rowspan="17">OSCCTRL - Oscillators Control</td><td>XOSCFAIL0</td>
      <td rowspan="2">2</td><td rowspan="2">disabled</td></tr>
    <tr><td>XOSCRDY0</td></tr>
    <tr><td>XOSCFAIL1</td>
      <td rowspan="2">3</td><td rowspan="2">disabled</td></tr>
    <tr><td>XOSCRDY1</td></tr>
    <tr><td>DFLLLCKC</td>
      <td rowspan="5">4</td><td rowspan="5">disabled</td></tr>
    <tr><td>DFLLLCKF</td></tr>
    <tr><td>DFLLOOB</td></tr>
    <tr><td>DFLLRCS</td></tr>
    <tr><td>DFLLRDY</td></tr>
    <tr><td>DPLL00LCKF</td>
      <td rowspan="4">5</td><td rowspan="4">disabled</td></tr>
    <tr><td>DPLL0LCKR</td></tr>
    <tr><td>DPLL0LDRTO</td></tr>
    <tr><td>DPLL0LTO</td></tr>
    <tr><td>DPLL1LCKF</td>
      <td rowspan="4">6</td><td rowspan="4">disabled</td></tr>
    <tr><td>DPLL1LCKR</td></tr>
    <tr><td>DPLL1LDRTO</td></tr>
    <tr><td>DPLL1LTO</td></tr>
    <tr>
      <td rowspan="2">OSC32KCTRL - 32 kHz Oscillators Control</td><td>XOSC32KFAIL</td>
      <td rowspan="2">7</td><td rowspan="2">disabled</td></tr>
    <tr><td>XOSC32KRDY</td></tr>
    <tr>
      <td rowspan="5">SUPC - Supply Controller</td><td>BOD33RDY</td>
      <td rowspan="4">8</td><td rowspan="4">disabled</td></tr>
    <tr><td>B33SRDY</td></tr>
    <tr><td>VCORERDY</td></tr>
    <tr><td>VREGRDY</td></tr>
    <tr><td>BOD33DET</td><td>9</td><td>disabled</td></tr>
    <tr><td>WDT - Watchdog Timer</td><td>EW</td><td>10</td><td><b>enabled</b></td></tr>
    <tr>
      <td rowspan="16">RTC - Real-Time Counter</td><td>CMP0</td>
      <td rowspan="16">11</td><td rowspan="16">disabled</td></tr>
    <tr><td>CMP1</td></tr>
    <tr><td>CMP2</td></tr>
    <tr><td>CMP3</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>PER0</td></tr>
    <tr><td>PER1</td></tr>
    <tr><td>PER2</td></tr>
    <tr><td>PER3</td></tr>
    <tr><td>PER4</td></tr>
    <tr><td>PER5</td></tr>
    <tr><td>PER6</td></tr>
    <tr><td>PER7</td></tr>
    <tr><td>TAMPER</td></tr>
    <tr><td>ALARM0</td></tr>
    <tr><td>ALARM1</td></tr>
    <tr>
      <td rowspan="16">EIC - External Interrupt Controller</td><td>EXTINT 0</td><td>12</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 1</td><td>13</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 2</td><td>14</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 3</td><td>15</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 4</td><td>16</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 5</td><td>17</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 6</td><td>18</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 7</td><td>19</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 8</td><td>20</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 9</td><td>21</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 10</td><td>22</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 11</td><td>23</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 12</td><td>24</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 13</td><td>25</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 14</td><td>26</td><td><b>enabled</b></td></tr>
    <tr><td>EXTINT 15</td><td>27</td><td><b>enabled</b></td></tr>
    <tr><td>FREQM - Frequency Meter</td><td>DONE</td><td>28</td><td>disabled</td></tr>
    <tr>
      <td rowspan="11">
        NVMCTRL - Non-Volatile Memory Controller
      </td><td>DONE</td>
      <td rowspan="8">29</td><td rowspan="8">disabled</td></tr>
    <tr><td>ADDRE</td></tr>
    <tr><td>PROGE</td></tr>
    <tr><td>LOCKE</td></tr>
    <tr><td>ECCSE</td></tr>
    <tr><td>ECCDE</td></tr>
    <tr><td>NVME</td></tr>
    <tr><td>SUSPE</td></tr>
    <tr><td>SEESFULL</td>
      <td rowspan="3">30</td><td rowspan="3">disabled</td></tr>
    <tr><td>SEESOVF</td></tr>
    <tr><td>SEEWRC</td></tr>
    <tr>
      <td rowspan="15">DMAC - Direct Memory Access Controller</td><td>SUSP 0</td>
      <td rowspan="3">31</td><td rowspan="3">enabled</td></tr>
    <tr><td>TCMPL 0</td></tr>
    <tr><td>TERR 0</td></tr>
    <tr><td>SUSP 1</td>
      <td rowspan="3">32</td><td rowspan="3">enabled</td></tr>
    <tr><td>TCMPL 1</td></tr>
    <tr><td>TERR 1</td></tr>
    <tr><td>SUSP 2</td>
      <td rowspan="3">33</td><td rowspan="3">enabled</td></tr>
    <tr><td>TCMPL 2</td></tr>
    <tr><td>TERR 2</td></tr>
    <tr><td>SUSP 3</td>
      <td rowspan="3">34</td><td rowspan="3">enabled</td></tr>
    <tr><td>TCMPL 3</td></tr>
    <tr><td>TERR 3</td></tr>
    <tr><td>SUSP 4..31</td>
      <td rowspan="3">35</td><td rowspan="3">enabled</td></tr>
    <tr><td>TCMPL 4..31</td></tr>
    <tr><td>TERR 4..31</td></tr>
    <tr>
      <td rowspan="10">EVSYS - Event System Interface</td><td>EVD 0</td>
      <td rowspan="2">36</td><td rowspan="2">disabled</td></tr>
    <tr><td>OVR 0</td></tr>
    <tr><td>EVD 1</td>
      <td rowspan="2">37</td><td rowspan="2">disabled</td></tr>
    <tr><td>OVR 1</td></tr>
    <tr><td>EVD 2</td>
      <td rowspan="2">38</td><td rowspan="2">disabled</td></tr>
    <tr><td>OVR 2</td></tr>
    <tr><td>EVD 3</td>
      <td rowspan="2">39</td><td rowspan="2">disabled</td></tr>
    <tr><td>OVR 3</td></tr>
    <tr><td>EVD 4..11</td>
      <td rowspan="2">40</td><td rowspan="2">disabled</td></tr>
    <tr><td>OVR 4..11</td></tr>
    <tr><td>PAC - Peripheral Access Controller</td><td>ERR</td><td>41</td><td>disabled</td></tr>
    <tr>
      <td rowspan="2">RAM ECC</td><td>SINGLEE</td>
      <td rowspan="2">45</td><td rowspan="2">disabled</td></tr>
    <tr><td>DUALE</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM0 - Serial Communication Interface 0
      </td><td>0</td><td>46</td><td>disabled</td></tr>
    <tr><td>1</td><td>47</td><td>disabled</td></tr>
    <tr><td>2</td><td>48</td><td>disabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">49</td><td rowspan="4">disabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM1 - Serial Communication Interface 1
      </td><td>0</td><td>50</td><td>disabled</td></tr>
    <tr><td>1</td><td>51</td><td>disabled</td></tr>
    <tr><td>2</td><td>52</td><td>disabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">53</td><td rowspan="4">disabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM2 - Serial Communication Interface 2
      </td><td>0</td><td>54</td><td>enabled</td></tr>
    <tr><td>1</td><td>55</td><td>enabled</td></tr>
    <tr><td>2</td><td>56</td><td>enabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">57</td><td rowspan="4">enabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM3 - Serial Communication Interface 3
      </td><td>0</td><td>58</td><td>enabled</td></tr>
    <tr><td>1</td><td>59</td><td>enabled</td></tr>
    <tr><td>2</td><td>60</td><td>enabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">61</td><td rowspan="4">enabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM4 - Serial Communication Interface 4
      </td><td>0</td><td>62</td><td>enabled</td></tr>
    <tr><td>1</td><td>63</td><td>enabled</td></tr>
    <tr><td>2</td><td>64</td><td>enabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">65</td><td rowspan="4">enabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM5 - Serial Communication Interface 5
      </td><td>0</td><td>66</td><td>enabled</td></tr>
    <tr><td>1</td><td>67</td><td>enabled</td></tr>
    <tr><td>2</td><td>68</td><td>enabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">69</td><td rowspan="4">enabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM6 - Serial Communication Interface 6
      </td><td>0</td><td>70</td><td>enabled</td></tr>
    <tr><td>1</td><td>71</td><td>enabled</td></tr>
    <tr><td>2</td><td>72</td><td>enabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">73</td><td rowspan="4">enabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="7">
        SERCOM7 - Serial Communication Interface 7
      </td><td>0</td><td>74</td><td>disabled</td></tr>
    <tr><td>1</td><td>75</td><td>disabled</td></tr>
    <tr><td>2</td><td>76</td><td>disabled</td></tr>
    <tr><td>3</td>
      <td rowspan="4">77</td><td rowspan="4">disabled</td></tr>
    <tr><td>4</td></tr>
    <tr><td>5</td></tr>
    <tr><td>7</td></tr>
    <tr>
      <td rowspan="2">CAN0 - Control Area Network 0</td><td>LINE 0</td>
      <td rowspan="2">78</td><td rowspan="2">disabled</td></tr>
    <tr><td>LINE 1</td></tr>
    <tr>
      <td rowspan="2">CAN1 - Control Area Network 1</td><td>LINE 0</td>
      <td rowspan="2">79</td><td rowspan="2">disabled</td></tr>
    <tr><td>LINE 1</td></tr>
    <tr>
      <td rowspan="16">USB - Universal Serial Bus</td><td>EORSM/DNRSM</td>
      <td rowspan="13">80</td><td rowspan="13">enabled</td></tr>
    <tr><td>EORST/RST</td></tr>
    <tr><td>LPM/DCONN</td></tr>
    <tr><td>LPMSUSP/DDISC</td></tr>
    <tr><td>RAMACER</td></tr>
    <tr><td>RXSTP/TXSTP 0..7</td></tr>
    <tr><td>STALL0/STALL 0..7</td></tr>
    <tr><td>STALL1 0..7</td></tr>
    <tr><td>SUSPEND</td></tr>
    <tr><td>TRFAIL0/TRFAIL 0..7</td></tr>
    <tr><td>TRFAIL1/PERR 0..7</td></tr>
    <tr><td>UPRSM</td></tr>
    <tr><td>WAKEUP</td></tr>
    <tr><td>SOF/HSOF</td><td>81</td><td>enabled</td></tr>
    <tr><td>TRCPT0 0..7</td><td>82</td><td>enabled</td></tr>
    <tr><td>TRCPT1 0..7</td><td>83</td><td>enabled</td></tr>
    <tr>
      <td rowspan="2">GMAC - Ethernet MAC</td><td>GMAC</td>
      <td rowspan="2">84</td><td rowspan="2">disabled</td></tr>
    <tr><td>WOL</td></tr>
    <tr>
      <td rowspan="16">TCC0 - Timer Counter Control 0</td><td>CNT</td>
      <td rowspan="10">85</td><td rowspan="10">disabled</td></tr>
    <tr><td>DFS</td></tr>
    <tr><td>ERR</td></tr>
    <tr><td>FAULTA</td></tr>
    <tr><td>FAULTB</td></tr>
    <tr><td>FAULT0</td></tr>
    <tr><td>FAULT1</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>TRG</td></tr>
    <tr><td>UFS</td></tr>
    <tr><td>MC0</td><td>86</td><td>disabled</td></tr>
    <tr><td>MC1</td><td>87</td><td>disabled</td></tr>
    <tr><td>MC2</td><td>88</td><td>disabled</td></tr>
    <tr><td>MC3</td><td>89</td><td>disabled</td></tr>
    <tr><td>MC4</td><td>90</td><td>disabled</td></tr>
    <tr><td>MC5</td><td>91</td><td>disabled</td></tr>
    <tr>
      <td rowspan="14">TCC1 - Timer Counter Control 1</td><td>CNT</td>
      <td rowspan="10">92</td><td rowspan="10">disabled</td></tr>
    <tr><td>DFS</td></tr>
    <tr><td>ERR</td></tr>
    <tr><td>FAULTA</td></tr>
    <tr><td>FAULTB</td></tr>
    <tr><td>FAULT0</td></tr>
    <tr><td>FAULT1</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>TRG</td></tr>
    <tr><td>UFS</td></tr>
    <tr><td>MC0</td><td>93</td><td>disabled</td></tr>
    <tr><td>MC1</td><td>94</td><td>disabled</td></tr>
    <tr><td>MC2</td><td>95</td><td>disabled</td></tr>
    <tr><td>MC3</td><td>96</td><td>disabled</td></tr>
    <tr>
      <td rowspan="13">TCC2 - Timer Counter Control 2</td><td>CNT</td>
      <td rowspan="10">97</td><td rowspan="10">disabled</td></tr>
    <tr><td>DFS</td></tr>
    <tr><td>ERR</td></tr>
    <tr><td>FAULTA</td></tr>
    <tr><td>FAULTB</td></tr>
    <tr><td>FAULT0</td></tr>
    <tr><td>FAULT1</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>TRG</td></tr>
    <tr><td>UFS</td></tr>
    <tr><td>MC0</td><td>98</td><td>disabled</td></tr>
    <tr><td>MC1</td><td>99</td><td>disabled</td></tr>
    <tr><td>MC2</td><td>100</td><td>disabled</td></tr>
    <tr>
      <td rowspan="12">TCC3 - Timer Counter Control 3</td><td>CNT</td>
      <td rowspan="10">101</td><td rowspan="10">disabled</td></tr>
    <tr><td>DFS</td></tr>
    <tr><td>ERR</td></tr>
    <tr><td>FAULTA</td></tr>
    <tr><td>FAULTB</td></tr>
    <tr><td>FAULT0</td></tr>
    <tr><td>FAULT1</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>TRG</td></tr>
    <tr><td>UFS</td></tr>
    <tr><td>MC0</td><td>102</td><td>disabled</td></tr>
    <tr><td>MC1</td><td>103</td><td>disabled</td></tr>
    <tr>
      <td rowspan="12">TCC4 - Timer Counter Control 4</td><td>CNT</td>
      <td rowspan="10">104</td><td rowspan="10">disabled</td></tr>
    <tr><td>DFS</td></tr>
    <tr><td>ERR</td></tr>
    <tr><td>FAULTA</td></tr>
    <tr><td>FAULTB</td></tr>
    <tr><td>FAULT0</td></tr>
    <tr><td>FAULT1</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>TRG</td></tr>
    <tr><td>UFS</td></tr>
    <tr><td>MC0</td><td>105</td><td>disabled</td></tr>
    <tr><td>MC1</td><td>106</td><td>disabled</td></tr>
    <tr>
      <td rowspan="4">TC0 - Basic Timer Counter 0</td><td>ERR</td>
      <td rowspan="4">107</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC1 - Basic Timer Counter 1</td><td>ERR</td>
      <td rowspan="4">108</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC2 - Basic Timer Counter 2</td><td>ERR</td>
      <td rowspan="4">109</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC3 - Basic Timer Counter 3</td><td>ERR</td>
      <td rowspan="4">110</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC4 - Basic Timer Counter 4</td><td>ERR</td>
      <td rowspan="4">111</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC5 - Basic Timer Counter 5</td><td>ERR</td>
      <td rowspan="4">112</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC6 - Basic Timer Counter 6</td><td>ERR</td>
      <td rowspan="4">113</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="4">TC7 - Basic Timer Counter 7</td><td>ERR</td>
      <td rowspan="4">114</td><td rowspan="4">disabled</td></tr>
    <tr><td>MC0</td></tr>
    <tr><td>MC1</td></tr>
    <tr><td>OVF</td></tr>
    <tr>
      <td rowspan="6">PDEC - Position Decoder</td><td>DIR</td>
      <td rowspan="4">115</td><td rowspan="4">disabled</td></tr>
    <tr><td>ERR</td></tr>
    <tr><td>OVF</td></tr>
    <tr><td>VLC</td></tr>
    <tr><td>MC0</td><td>116</td><td>disabled</td></tr>
    <tr><td>MC1</td><td>117</td><td>disabled</td></tr>
    <tr>
      <td rowspan="3">ADC0 - Analog Digital Converter 0</td><td>OVERRUN</td>
      <td rowspan="2">118</td><td rowspan="2">disabled</td></tr>
    <tr><td>WINMON</td></tr>
    <tr><td>RESRDY</td><td>119</td><td>disabled</td></tr>
    <tr>
      <td rowspan="3">ADC1 - Analog Digital Converter 1</td><td>OVERRUN</td>
      <td rowspan="2">120</td><td rowspan="2">disabled</td></tr>
    <tr><td>WINMON</td></tr>
    <tr><td>RESRDY</td><td>121</td><td>disabled</td></tr>
    <tr>
      <td rowspan="3">AC - Analog Comparators</td><td>COMP0</td>
      <td rowspan="3">122</td><td rowspan="3">disabled</td></tr>
    <tr><td>COMP1</td></tr>
    <tr><td>WIN0</td></tr>
    <tr>
      <td rowspan="8">DAC - Digital-to-Analog Converter</td><td>OVERRUN0</td>
      <td rowspan="4">123</td><td rowspan="4">disabled</td></tr>
    <tr><td>OVERRUN1</td></tr>
    <tr><td>UNDERRUN0</td></tr>
    <tr><td>UNDERRUN1</td></tr>
    <tr><td>EMPTY0</td><td>124</td><td>disabled</td></tr>
    <tr><td>EMPTY1</td><td>125</td><td>disabled</td></tr>
    <tr><td>RESRDY0</td><td>126</td><td>disabled</td></tr>
    <tr><td>RESRDY1</td><td>127</td><td>disabled</td></tr>
    <tr>
      <td rowspan="8">I2S - Inter-IC Sound Interface</td><td>RXOR0</td>
      <td rowspan="8">128</td><td rowspan="8">disabled</td></tr>
    <tr><td>RXOR1</td></tr>
    <tr><td>RXRDY0</td></tr>
    <tr><td>RXRDY1</td></tr>
    <tr><td>TXRDY0</td></tr>
    <tr><td>TXRDY1</td></tr>
    <tr><td>TXUR0</td></tr>
    <tr><td>TXUR1</td></tr>
    <tr>
      <td rowspan="2">PCC - Parallel Capture Controller</td><td>DRDY</td>
      <td rowspan="2">129</td><td rowspan="2">disabled</td></tr>
    <tr><td>OVRE</td></tr>
    <tr>
      <td rowspan="2">AES - Advanced Encryption Standard</td><td>ENCCMP</td>
      <td rowspan="2">130</td><td rowspan="2">disabled</td></tr>
    <tr><td>GFMCMP</td></tr>
    <tr><td>TRNG - True Random Generator</td><td>DATARDY</td><td>131</td><td>disabled</td></tr>
    <tr><td>ICM - Integrity Check Monitor</td><td>All ICM Interrupts</td><td>132</td><td>disabled</td></tr>
    <tr><td>Reserved</td><td>Reserved</td><td>133</td><td>disabled</td></tr>
    <tr>
      <td rowspan="6">QSPI - Quad SPI interface</td><td>RXC</td>
      <td rowspan="6">134</td><td rowspan="6">disabled</td></tr>
    <tr><td>DRE</td></tr>
    <tr><td>TXC</td></tr>
    <tr><td>ERROR</td></tr>
    <tr><td>CSRISE</td></tr>
    <tr><td>INSTREND</td></tr>
    <tr><td>SDHC0 - SD/MMC Host Controller 0</td><td>All SDHC0 Interrupts</td><td>135</td><td>disabled</td></tr>
    <tr><td>SDHC1 - SD/MMC Host Controller 1</td><td>All SDHC1 Interrupts</td><td>136</td><td>disabled</td></tr>
  </tbody>
</table>


## Exception and Interrupt Handlers

Default exception handler functions are defined in startup_samd21.c.
They're defined as “weak” functions, so you can override the default implementation with your own.

## NVIC Interrupts Defined in the Adafruit SAMD U2F Bootloader and Arduino Core

### SAMD51 NVIC

- SERCOMs - sercom.cpp - priority of 3
  - `SERCOM0_0_IRQn` -> `SERCOM5_3_IRQn`
  - `SERCOM6_0_IRQn` -> `SERCOM6_3_IRQn`, iff variant defines `SERCOM6`
  - `SERCOM7_0_IRQn` -> `SERCOM7_3_IRQn`, iff variant defines `SERCOM7`
- Timers:
  - TC0 (`TC5_IRQn`) - Tone.cpp - priority of 5(?)
  - TC1 (`TC1_IRQn`) - Servo.cpp - priority of 0
    - NOTE: The sevro library has a to-do flag in it to allow a second timer, but as of November of 2024 only supports one timer for servo in the SAMD core.
- External Interrupt Controller - WInterrupts.c - priority of **0**
  - `EIC_0_IRQn` -> `EIC_15_IRQn`
  - A single EIC interrupt for the USB pin is also enabled by Adafruit_USBH_Host.cpp if TinyUSB is used.
- USB - samd21_host.c and USBCore.cpp - priority 0
  - NOTE: Dispite the name, samd21_host.c is used for all SAMD boards
  - `USB_0_IRQn` -> `USB_3_IRQn`
- DMAC - Adafruit_ZeroDMA.cpp or I2S/../DMA.cpp
  - `DMAC_0_IRQn` -> `DMAC_4_IRQn` - priority of 3

### SAMD21 NVIC

- SERCOMs - sercom.cpp - priority of 3
  - `SERCOM0_IRQn` -> `SERCOM3_IRQn`
  - `SERCOM4_IRQn`, iff variant defines `SERCOM4`
  - `SERCOM5_IRQn`, iff variant defines `SERCOM5`
- Timers:
  - TC5 (`TC5_IRQn`) - Tone.cpp - priority of 5(?)
  - TC4 (`TC4_IRQn`) - Servo.cpp - priority of 0
    - NOTE: The sevro library has a to-do flag in it to allow a second timer, but as of November of 2024 only supports one timer for servo in the SAMD core.
- External Interrupt Controller - WInterrupts.c - priority of **0**
  - `EIC_IRQn`
  - The EIC interrupts are also enabled by Adafruit_USBH_Host.cpp if TinyUSB is used.
- USB - samd21_host.c and USBCore.cpp - priority 0
  - `USB_IRQn`
- DMAC - Adafruit_ZeroDMA.cpp or I2S/../DMA.cpp
  - `DMAC_IRQn` - priority of 3

## NVIC Interrupts Defined in Other Popular Libraries
