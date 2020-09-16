/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "variant.h"

/*
 * Pins descriptions
 */

//TCC0 IOSET 6
//TCC1 IOSET 1
//TC0 IOSET 1
//TC1 IOSET 1
//TC2 IOSET 2
//TC3 IOSET 1
//TC4 IOSET 1

const PinDescription g_APinDescription[]=
{
  // 0..13 - Digital pins
  // ----------------------
  // 0/1 - SERCOM/UART (Serial1)
  { PORTB, 17, PIO_SERCOM, PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH4, NOT_ON_TIMER, EXTERNAL_INT_1 }, // RX: SERCOM5/PAD[1]
  { PORTB, 16, PIO_SERCOM, PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH5, NOT_ON_TIMER, EXTERNAL_INT_0 }, // TX: SERCOM5/PAD[0]

  // 2..12
  // Digital Low
  { NOT_A_PORT, 0,  PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { NOT_A_PORT, 0,  PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { PORTA, 14, PIO_DIGITAL, PIN_ATTR_PWM_E, No_ADC_Channel, TC3_CH0, TC3_CH0, EXTERNAL_INT_14 },
  { PORTA,  16, PIO_TIMER_ALT, PIN_ATTR_PWM_F, No_ADC_Channel, TCC1_CH0, TC2_CH0, EXTERNAL_INT_0  },
  { PORTA,  18, PIO_TIMER_ALT, PIN_ATTR_PWM_F, No_ADC_Channel, TCC1_CH2, TC3_CH0, EXTERNAL_INT_2 },
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_TIMER, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },

  // Digital High
  { PORTB, 3, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, //neopix
  { PORTA,  19, PIO_TIMER_ALT, PIN_ATTR_PWM_F, No_ADC_Channel, TCC1_CH3, TC3_CH1, EXTERNAL_INT_3 },
  { PORTA,  20, PIO_TIMER_ALT, PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH0, NOT_ON_TIMER, EXTERNAL_INT_4 },
  { PORTA, 21, PIO_DIGITAL, PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH1, NOT_ON_TIMER, EXTERNAL_INT_5 },
 
  //12 Digital or SERIAL3_Tx
#ifdef SERIAL3_EN
  { PORTA, 22, PIO_SERCOM,     PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH5, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // TX: SERCOM3/PAD[0]
#else
  { PORTA, 22, PIO_DIGITAL, PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH2, NOT_ON_TIMER, EXTERNAL_INT_6 },
#endif //SERIAL3_EN
  // 13 (LED) or SERIAL3_RX
#ifdef SERIAL3_EN
  { PORTA, 23, PIO_SERCOM,     PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH4, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // RX: SERCOM3/PAD[1]
#else
  { PORTA, 23, PIO_DIGITAL, PIN_ATTR_PWM_G, No_ADC_Channel, TCC0_CH3, TC4_CH1, EXTERNAL_INT_7 }, // TCC2/WO[1]
#endif //SERIAL3_EN
  /* 14..19 - Analog pins or
    14 Analog
    15 Analog SERIAL2_RX/SERCOM0
    16 Analog SERIAL4_TX/SERCOM4
    17 Analog SERIAL4_RX/SERCOM4
    18 Analog SERIAL2_TX/SERCOM0
    19 A5 Analog SERIAL2_TE/SERCOM0  SERIAL2_RTS
       Where is SERIAL2_CTS
  */
  // --------------------
/*14*/{ PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 },
#if ((defined(SERIAL2_EN) || defined(SERIAL2_TE_CNTL)) && !defined(SERIAL2_TE_HALF_DUPLEX)) || defined(SERIAL2_RTS_CTS)
/*15*/{ PORTA, 05, PIO_SERCOM_ALT, PIN_ATTR_ANALOG, ADC_Channel5, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // RX: SERCOM0/PAD[1]
#else
/*15*/{ PORTA,  5, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel5, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_5 },
#endif //SERIAL2_EN
#ifdef SERIAL4_EN
/*16*/{ PORTB,  8, PIO_SERCOM_ALT, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel2, TC4_CH0, TC4_CH0, EXTERNAL_INT_NONE },
#else
/*16*/{ PORTB,  8, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel2, TC4_CH0, TC4_CH0, EXTERNAL_INT_8 },
#endif //SERIAL4_EN
#ifdef SERIAL4_EN
/*17*/{ PORTB,  9, PIO_SERCOM_ALT, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel3, TC4_CH1, TC4_CH1, EXTERNAL_INT_NONE },

#else
/*17*/{ PORTB,  9, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel3, TC4_CH1, TC4_CH1, EXTERNAL_INT_9 },
#endif //SERIAL4_EN
#if defined(SERIAL2_EN) || defined(SERIAL2_TE_CNTL) || defined(SERIAL2_TE_HALF_DUPLEX) || defined(SERIAL2_RTS_CTS)
/*18*/{ PORTA, 04, PIO_SERCOM_ALT, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E),  ADC_Channel4, TC0_CH0, TC0_CH0, EXTERNAL_INT_NONE }, // TX: SERCOM0/PAD[2]
#else
  { PORTA,  4, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel4, TC0_CH0, TC0_CH0, EXTERNAL_INT_6 },
#endif //SERIAL2_EN
#if  defined(SERIAL2_TE_CNTL) || defined(SERIAL2_TE_HALF_DUPLEX) || defined(SERIAL2_RTS_CTS)
/*19*/{ PORTA,  06, PIO_SERCOM_ALT, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel6, TC1_CH0, TC1_CH0, EXTERNAL_INT_10 },
#else
/*19*/{ PORTA,  6, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM_E), ADC_Channel6, TC1_CH0, TC1_CH0, EXTERNAL_INT_10 },
#endif //SERIAL2_TE_CNTL
  // A6, D20 - VDiv!
  { PORTB,  1, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel13, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_1 },


  // 21..22 I2C pins (SDA/SCL)
  // ----------------------
  { PORTA, 12, PIO_SERCOM, PIN_ATTR_PWM_E, No_ADC_Channel, TC2_CH0, TC2_CH0, EXTERNAL_INT_12 }, // SDA: SERCOM2/PAD[0]
  { PORTA, 13, PIO_SERCOM, PIN_ATTR_PWM_E, No_ADC_Channel, TC2_CH1, TC2_CH1, EXTERNAL_INT_13 }, // SCL: SERCOM2/PAD[1]

  // 23..25 - SPI pins (MISO,MOSI,SCK)
  // ----------------------
  { PORTB, 22, PIO_SERCOM, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_6 }, // MISO: SERCOM1/PAD[2]
  { PORTB, 23, PIO_SERCOM, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_7 }, // MOSI: SERCOM1/PAD[3]
  { PORTA, 17, PIO_SERCOM, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_1 }, // SCK: SERCOM1/PAD[1]

  // 26..27 - RX/TX LEDS  -- unused
  // --------------------
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },

  // 28..30 - USB
  // --------------------
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB Host enable DOES NOT EXIST ON THIS BOARD
  { PORTA, 24, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DM
  { PORTA, 27, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DP

  // 31 (AREF)
  { PORTA, 3, PIO_ANALOG, PIN_ATTR_ANALOG, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // DAC/VREFP

  // ----------------------
  // 32..33 - Alternate use of A0 (DAC output)
  { PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, DAC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // DAC/VOUT0
  { PORTA,  5, PIO_ANALOG, PIN_ATTR_ANALOG, DAC_Channel1, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // DAC/VOUT1

  // ----------------------
  // 34..39 QSPI (SCK, CS, IO0, IO1, IO2, IO3)
  { PORTB, 10, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { PORTB, 11, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { PORTA, 8, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { PORTA, 9, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { PORTA, 10, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  { PORTA, 11, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },

} ;

const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TCC3, TCC4, TC0, TC1, TC2, TC3, TC4, TC5 } ;
const uint32_t GCLK_CLKCTRL_IDs[TCC_INST_NUM+TC_INST_NUM] = { TCC0_GCLK_ID, TCC1_GCLK_ID, TCC2_GCLK_ID, TCC3_GCLK_ID, TCC4_GCLK_ID, TC0_GCLK_ID, TC1_GCLK_ID, TC2_GCLK_ID, TC3_GCLK_ID, TC4_GCLK_ID, TC5_GCLK_ID } ;

// Multi-serial objects instantiation
SERCOM sercom0( SERCOM0 ) ;
SERCOM sercom1( SERCOM1 ) ;
SERCOM sercom2( SERCOM2 ) ;
SERCOM sercom3( SERCOM3 ) ;
SERCOM sercom4( SERCOM4 ) ;
SERCOM sercom5( SERCOM5 ) ;


/* 
  FeatherM4express Serial1 allocated SERCOM5 as standard
  SERCOM0 is undefinied in std variant.cpp on Feather M4 express
  Function FeatherM4Pin [PinDescriptionindex] cpuPin Port Sercom#
   Rx   D0   [0] cpuPin40/PB17 SERCOM5 Pad#2
   Tx   D1   [1] cpuPin39/PB16 SERCOM5 Pad#0
   Te   D10 [10] cpupin15/PA20 SERCOM5 Pad#2 - shared SERCOM
*/
Uart Serial1( &sercom5, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;

void SERCOM5_0_Handler()
{
  Serial1.IrqHandler();
}
void SERCOM5_1_Handler()
{
  Serial1.IrqHandler();
}
void SERCOM5_2_Handler()
{
  Serial1.IrqHandler();
}
void SERCOM5_3_Handler()
{
  Serial1.IrqHandler();
}

#if defined(SERIAL2_EN) || defined(SERIAL2_TE_CNTL) || defined(SERIAL2_TE_HALF_DUPLEX) || defined(SERIAL2_RTS_CTS)
/* TESTED for full duplex using Rx/A1 Tx/A4 Te/A5
   Not Tested for half duplex Tx/A4 Te/A5
  FeatherM4express Serial2 allocated SERCOM0 with custom variant.cpp
  SERCOM0 is undefinied in std variant.cpp on Feather M4 express
  Function FeatherM4Pin [PinDescriptionindex] cpuPin Port Sercom#
   Rx   A1 [15] cpuPin14/PA05 SERCOM0 Pad#1
   Tx   A4 [18] cpuPin13/PA04 SERCOM0 Pad#0
   Te   A5 [19] cpupin15/PA06 SERCOM0 Pad#2
  Half duplex set by specifying the same PAD for Rx as Tx - needs checking. 
*/
#if defined(SERIAL2_TE_HALF_DUPLEX)
#undef PIN_SERIAL2_RX
#undef PAD_SERIAL2_RX
#define PIN_SERIAL_2_RX PIN_SERIAL_2_TX 
#define PAD_SERIAL2_RX (SercomUartTXPad) UART_TX_TE_PAD_0_2
#endif 
#if defined(SERIAL2_TE_CNTL) || defined(SERIAL2_TE_HALF_DUPLEX)
//  Ext function dedicated TE defined by TXPO/UART_TX_TE_PAD_0_2  pin TE operation defined above
Uart Serial2( &sercom0, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, (SercomUartTXPad) UART_TX_TE_PAD_0_2); //Full duplex
#elif defined(SERIAL2_RTS_CTS)
//  Ext function dedicated RTS & CTS defined by TXPO/UART_TX_RTS_CTS_PAD_0_2_3.  pin RTS and CTS operation defined above
Uart Serial2( &sercom0, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX,UART_TX_RTS_CTS_PAD_0_2_3); //Full duplex
#else 
Uart Serial2( &sercom0, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX); //Full duplex
#endif //SERIAL2_TE_CNTL
//Uart Serial2(&sercom0, 40, 41, SERCOM_RX_PAD_1, UART_TX_TE_PAD_0_2); //RS485 Half Duplex 2pin ??


// Hand over the interrupts of the sercom port
void SERCOM0_0_Handler() //DRE:0
{
  Serial2.IrqHandler();
}
void SERCOM0_1_Handler() //TXC:1
{
  Serial2.IrqHandler();
}
void SERCOM0_2_Handler() //RXC:2
{
  Serial2.IrqHandler();
}
void SERCOM0_3_Handler() //RXS:3 CTSIC:4 RXBRK:5 ERROR:7
{
  Serial2.IrqHandler();
}
#endif //SERIAL2_EN

#ifdef SERIAL3_EN
/* TESTED for full duplex
 FeatherM4express Serial3 allocated SERCOM3 with custom variant.cpp
 SERCOM3 is undefinied in std variant.cpp on Feather M4 express
 Function FeatherM4Pin [PinDescriptionindex] cpuPin Port Sercom#
  Rx  D13 [13] cpuPin44/PA23 SERCOM3 Pad#1 - Shared LED BUILTIN
  Tx  D12 [12] cpuPin43/PA22 SERCOM3 Pad#0
  Te  D10 [10] cpuPin41/PA20 SERCOM3 Pad#2
*/ 
Uart Serial3(&sercom3, PIN_SERIAL3_RX, PIN_SERIAL3_TX, SERCOM_RX_PAD_1, UART_TX_PAD_2);
//Uart Serial3(&sercom3, PIN_SERIAL3_RX, PIN_SERIAL3_TX, SERCOM_RX_PAD_1, UART_TX_TE_PAD_0_2); //??RS485 3W

// Hand over the interrupts of the sercom port
void SERCOM3_0_Handler()
{
  Serial3.IrqHandler();
}
void SERCOM3_1_Handler()
{
  Serial3.IrqHandler();
}
void SERCOM3_2_Handler()
{
  Serial3.IrqHandler();
}
void SERCOM3_3_Handler()
{
  Serial3.IrqHandler();
}
#endif //SERIAL3_EN

#ifdef SERIAL4_EN
/* TESTED for Full duplex. Not tested for Half duplex 
 FeatherM4express - Set up UART using SERCOM4
 FeatherM4express Serial4 allocated SERCOM4 with custom variant.cpp
 SERCOM4 is undefinied invariant.cpp on Feather  M4 express,
 Function FeatherM4Pin [PinDescriptionindex] cpuPin Port Sercom#
  Rx   A3  [17] cpuPin13/PA05 SERCOM4 Pad#1
  Tx   A2  [16] cpuPin11/PB08 SERCOM4 Pad#0
  Te   D10 [10] cpupin15/PA06 SERCOM4 Pad#2
*/
Uart Serial4(&sercom4, PIN_SERIAL4_RX, PIN_SERIAL4_TX, SERCOM_RX_PAD_1, UART_TX_PAD_0); //Full duplex
//Uart Serial4(&sercom4, PIN_SERIAL4_RX, PIN_SERIAL4_TX, SERCOM_RX_PAD_1, PAD_SERIAL4_TX_TE); //RS485 Half Duplex 3pin ??
//Uart Serial4(&sercom4, PIN_SERIAL4_RX, PIN_SERIAL4_TX, SERCOM_RX_PAD_0, PAD_SERIAL4_TX_TE); //RS485 Half Duplex 2pin ??

// Hand over the interrupts of the sercom port
void SERCOM4_0_Handler()
{
  Serial4.IrqHandler();
}
void SERCOM4_1_Handler()
{
  Serial4.IrqHandler();
}
void SERCOM4_2_Handler()
{
  Serial4.IrqHandler();
}
void SERCOM4_3_Handler()
{
  Serial4.IrqHandler();
}
#endif //SERIAL4_EN

const uint32_t g_APinDescription_size=sizeof(g_APinDescription) ;
