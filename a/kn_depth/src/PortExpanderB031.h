/*
 *PortExpanderB031.h
 *This file is for the B031 Port Expander access functions.
 * see peB031_bit for definitions
*/
#pragma once
#ifdef MS_PortExpanderB031E_DEBUG
#define MS_DEBUGGING_STD "PortExpanderB031"
#endif

// Dependencies
#include "ModSensorDebugger.h"
#undef MS_PortExpanderB031_STD

#include "MCP23017.h" //Port 16bit Expander
enum class peB031_bit :uint8_t 
{
    eMcp_SwV3out_bit, //00
    eMcp_SwVbatout_bit,  //01
    eMcp_SwVrs485out_bit,//02
    eMcp_swV1wout_bit   ,//03
    eMcp_SwVsdiout_bit  ,//04
    eMcp_XbeeOnSleepNin_bit,//05
    eMcp_XbeeSleeRqout_bit ,//06
    eMcp_XbeeResetNout_bit ,//07
    eMcp_CustOut1out_bit ,  //08
    eMcp_CustOut2out_bit ,//09
    eMcp_MuxAdcAout_bit  ,//10
    eMcp_MuxAdcBout_bit  ,//11
    eMcp_MuxAdcCout_bit  ,//12
    eMcp_MuxAdcEnout_bit ,//13
    eMcp_PB06_bit       ,//14
    eMcp_eInk_RSTout_bit , //15
};
enum class eMcpA_bm :uint8_t  //bit mask
{
    eMcpA_SwV3_bmout       = 0x01,//(1<<(uint8_t)peB031_bit::eMcp_SwV3out_bit),
    eMcpA_SwVbat_bmout     = 0x02,
    eMcpA_SwVrs485_bmout   = 0x04,
    eMcpA_swV1w_bmout      = 0x08,
    eMcpA_SwVsdi_bmout     = 0x10,
    eMcpA_XbeeOnSleepN_bmin= 0x20,
    eMcpA_XbeeSleeRq_bmout = 0x40,
    eMcpA_XbeeResetN_bmout = 0x80,

    eMcpA_direction = 0b00100000,
    eMcpA_default =(eMcpA_SwV3_bmout|eMcpA_XbeeSleeRq_bmout|eMcpA_XbeeResetN_bmout),

};//eMcpA_bm
enum class eMcpB_bm :uint8_t  
{
    eMcpB_CustOut1_bmout = 0x01,
    eMcpB_CustOut2_bmout = 0x02,
    eMcpB_MuxAdcA_bmout  = 0x04,
    eMcpB_MuxAdcB_bmout  = 0x08,
    eMcpB_MuxAdcC_bmout  = 0x10,
    eMcpB_MuxAdcEn_bmout = 0x20,
    eMcpB_PB06         = 0x40,
    eMcpB_eInk_RST_bmout = 0x80,

    eMcpB_direction = 0x00,
 
    eMcpB_default = (eMcpB_eInk_RST_bmout),
};//eMcpB_bm
//class portFriend;  forward ref

class PortExpanderB031 : public MCP23017
{
    //friend class portFriend;

public:
	PortExpanderB031(uint8_t address, TwoWire& bus = Wire);
	~PortExpanderB031();
    void init();
    void configure(uint32_t ulPinMax);
    void setBit(peB031_bit portNum); 
    void clrBit(peB031_bit portNum); 
    void toggleBit(peB031_bit portNum,uint16_t delay_ms=2);
    void digitalWrite( uint32_t ulPin, uint32_t ulVal ); //mimic wiring_digital
    uint8_t _portA=(uint8_t)eMcpA_bm::eMcpA_default;
    uint8_t _portB=(uint8_t)eMcpB_bm::eMcpB_default;
    uint8_t _ulPinMax=0;
    //void writeRegister(MCP23017_REGISTER reg, uint8_t value);
};

//end PortPortExpanderB031