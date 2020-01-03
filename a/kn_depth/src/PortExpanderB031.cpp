/* PortExanderB031 - custom interace to a board 
   This should be folded into MCP23017 as really just relative to MCP23017?
    - some added value 

    Software License: BSD-3.
    Copyright (c) 2019, Neil Hancock     
  */
#include "PortExpanderB031.h"

PortExpanderB031::PortExpanderB031(uint8_t address, TwoWire& bus) : MCP23017( address, bus){}
PortExpanderB031::~PortExpanderB031() {}

size_t PortExpanderB031::init() {
    //MCP23017::init();
    size_t retVal;
    MS_DBG(F("-PortExpanderB031::init "));
    retVal = MCP23017::init();
    MS_DEEP_DBG(F("-PeInit init "), retVal);
    retVal |= MCP23017::portMode(MCP23017_PORT::A, (uint8_t)eMcpA_bm::eMcpA_direction);
    retVal |= MCP23017::portMode(MCP23017_PORT::B, (uint8_t)eMcpB_bm::eMcpB_direction);

    //MCP23017::interruptMode(MCP23017_INTMODE::SEPARATED); B031r1 together     
    //MCP23017::interrupt(MCP23017_PORT::B, FALLING);

    retVal |= MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, (uint8_t)eMcpA_bm::eMcpA_default);
    retVal |= MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, (uint8_t)eMcpB_bm::eMcpB_default );
    MCP23017::clearInterrupts();
    return retVal;
}

size_t PortExpanderB031::setBit(peB031_bit portNum, bool value) {
    size_t retVal;
    if (value) {
        retVal = setBit(portNum);
    } else {
        retVal = clrBit(portNum);
    }
    return retVal;
}
size_t PortExpanderB031::setBit(peB031_bit portNum) {
    uint8_t mcpBit=(uint8_t)portNum;
    size_t retVal;

    if (mcpBit > 7) {
        mcpBit -= 8;
        _portB |= (1<<mcpBit) ;
        retVal = MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("setPortB"),mcpBit,F("="),_portB);
    } else {
        _portA |= (1<<mcpBit);
        retVal = MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("setPortA"),mcpBit,F("="),_portA);
    }
    return retVal;
}
size_t PortExpanderB031::clrBit(peB031_bit portNum) {
    uint8_t mcpBit=(uint8_t)portNum;
    size_t retVal;

    if (mcpBit > 7) {
        mcpBit -= 8;
        _portB &= ~(1<<mcpBit) ;
        retVal = MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("clrPortB "),mcpBit,F("="),_portB);
    } else {
        _portA &= ~(1<<mcpBit);
        retVal = MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("clrPortA"),mcpBit,F("="),_portA);
    }
    return retVal;
}

size_t PortExpanderB031::pulseToggleBit(peB031_bit portNum,uint16_t delay_ms) {
    uint8_t mcpBit=(uint8_t)portNum;
    uint8_t mcpMask=0;
    size_t retVal;

    if (mcpBit > 7) {
        mcpBit -= 8;
        mcpMask = (1<<mcpBit) ;
        _portB ^= mcpMask;
        retVal = MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("pulsePortB"),mcpBit,F("="),_portB," mS=",delay_ms);
        delay(delay_ms);
        _portB ^= mcpMask;
        retVal |= MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("wrPortB "),_portB);        
    } else {
        mcpMask = (1<<mcpBit) ;
        _portA ^= mcpMask;
        retVal = MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("pulesPortA "),mcpBit,F("="),_portA," mS=",delay_ms," bit ",mcpMask);
        delay(delay_ms);
        _portA ^= mcpMask;
        retVal |= MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("wrPortA "),_portA);
    }
    return retVal;
}

size_t PortExpanderB031::digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
    size_t retVal;
    if (ulPin >= _ulPinMax) {
        retVal = digitalWrite(ulPin, ulVal); //Arduino wiring_digital.c
    } else {
        //pinDigitalWrite()
    }
    return retVal;
}

String PortExpanderB031::getPortStr(uint8_t mcpBit) 
{
    if (mcpBit > 7) {
        mcpBit -= 8;
        if (mcpBit <8) {
            return mcpBdesc[mcpBit];
        } else {
            return "Err";
        }
    } else {
        return mcpAdesc[mcpBit];
    }
}

uint8_t PortExpanderB031::rdMir(peB031_bit portNum) {
    uint8_t mcpBit=(uint8_t)portNum;
    uint8_t retVal=0;
    if (mcpBit > 7) {
        mcpBit -= 8;
        retVal = _portB & ~(1<<mcpBit) ;
        MS_DBG(F("mirPortB"),mcpBit,F("="),retVal);
    } else {
        retVal = _portA & ~(1<<mcpBit) ;
        MS_DBG(F("mirPortA"),mcpBit,F("="),retVal);
    }
    return retVal;
}    