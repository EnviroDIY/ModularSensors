/* PortExanderB031 - custom interace to a board */
#include "PortExpanderB031.h"

PortExpanderB031::PortExpanderB031(uint8_t address, TwoWire& bus) : MCP23017( address, bus){}
PortExpanderB031::~PortExpanderB031() {}

void PortExpanderB031::init() {
    MCP23017::init();
    MS_DBG(F("-PortExpanderB031 init"));
    MCP23017::init();
    MCP23017::portMode(MCP23017_PORT::A, (uint8_t)eMcpA_bm::eMcpA_direction);
    MCP23017::portMode(MCP23017_PORT::B, (uint8_t)eMcpB_bm::eMcpB_direction);

    //MCP23017::interruptMode(MCP23017_INTMODE::SEPARATED); B031r1 together     
    //MCP23017::interrupt(MCP23017_PORT::B, FALLING);

    MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, (uint8_t)eMcpA_bm::eMcpA_default);
    MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, (uint8_t)eMcpB_bm::eMcpB_default );
    MCP23017::clearInterrupts();
}

void PortExpanderB031::setBit(peB031_bit portNum, bool value) {
    if (value) {
        setBit(portNum);
    } else {
        clrBit(portNum);
    }
}
void PortExpanderB031::setBit(peB031_bit portNum) {
    uint8_t mcpBit=(uint8_t)portNum;

    if (mcpBit > 7) {
        mcpBit -= 8;
        _portB |= (1<<mcpBit) ;
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("SPB "),_portB);
    } else {
        _portA |= (1<<mcpBit);
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("SPA "),_portA);
    }
}
void PortExpanderB031::clrBit(peB031_bit portNum) {
    uint8_t mcpBit=(uint8_t)portNum;

    if (mcpBit > 7) {
        mcpBit -= 8;
        _portB &= ~(1<<mcpBit) ;
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("CPB "),_portB);
    } else {
        _portA &= ~(1<<mcpBit);
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("CPA "),_portA);
    }
}

void PortExpanderB031::pulseToggleBit(peB031_bit portNum,uint16_t delay_ms) {
    uint8_t mcpBit=(uint8_t)portNum;
    uint8_t mcpMask=0;
    if (mcpBit > 7) {
        mcpBit -= 8;
        mcpMask = (1<<mcpBit) ;
        _portB ^= mcpMask;
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("XPB "),_portB," mS=",delay_ms);
        delay(delay_ms);
        _portB ^= mcpMask;
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOB, _portB);
        MS_DBG(F("XPB "),_portB);        
    } else {
        mcpMask = (1<<mcpBit) ;
        _portA ^= mcpMask;
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("XPA "),_portA," mS=",delay_ms," bit ",mcpMask);
        delay(delay_ms);
        _portA ^= mcpMask;
        MCP23017::writeRegister(MCP23017_REGISTER::GPIOA, _portA);
        MS_DBG(F("XPA "),_portA);
    }
}

void PortExpanderB031::digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
    if (ulPin >= _ulPinMax) {
        digitalWrite(ulPin, ulVal); //Arduino wiring_digital.c
    } else {
        //pinDigitalWrite()
    }
}
