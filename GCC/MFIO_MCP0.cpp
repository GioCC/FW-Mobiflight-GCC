// MFIO_MCP0.cpp
//
// Copyright (C) 2018

#include "MFIO_MCP0.h"

MFIO_MCP0::MFIO_MCP0(byte nUnits, byte addr)
: MCP23x17(nUnits, addr)
{
    //_initialized = false;
    _pins = 2;
}

void MFIO_MCP0::attach(int SDAPin, int SCLPin)
{
    // We should check that the pin nr. values are legal!

    /*
    if(  (scl_pin!=STD_PIN && scl_pin!=SCL)
      || (sda_pin!=STD_PIN && sda_pin!=SDA) ) {
      // [TODO] not using standard I2C pins:
      // should setup a non-std I2C SW library here! (instead of <Wire>)
        SCLMCP = SCLPin;
        SDAMCP = SDAPin;
        pinMode(SCLPin, INPUT_PULLUP);
        pinMode(SDAPin, INPUT_PULLUP);
        digitalWrite(SCLPin, HIGH);
        digitalWrite(SDAPin, HIGH);

    } else {
        Wire.begin();
    }
    */

    Wire.begin();
    _initialized = true;
}

void MFIO_MCP0::detach()
{
    _initialized = false;
}

/ Generic register R/W functions (byte/word-wise)

byte SIO_io_23017::
readB(byte reg)
{
    // from Adafruit_MCP23017:
    Wire.beginTransmission((MCP_OPCODER>>1)|(_address+_currUnit));
    Wire.write((uint8_t) reg);
    Wire.endTransmission();
    Wire.requestFrom((MCP_OPCODER>>1)|(_address+_currUnit), 1);
    return Wire.read();
}

unsigned int SIO_io_23017::
readW(byte reg)
{
    // from Adafruit_MCP23017:
    unsigned int ba = 0;
    Wire.beginTransmission((MCP_OPCODER>>1)|(_address+_currUnit));
    Wire.write((uint8_t) reg);
    Wire.endTransmission();
    Wire.requestFrom((MCP_OPCODER>>1)|(_address+_currUnit), 2);
    ba = (Wire.read() & 0xFF);
    ba |= ((uint16_t)(Wire.read())<<8);
    return ba;
}

void SIO_io_23017::
writeB(byte reg, byte val)
{
    // from Adafruit_MCP23017:
    Wire.beginTransmission((MCP_OPCODER>>1)|(_address+_currUnit));
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)val);
    Wire.endTransmission();
}

void SIO_io_23017::
writeW(byte reg, unsigned int val)
{
    // from Adafruit_MCP23017:
    Wire.beginTransmission((MCP_OPCODER>>1)|(_address+_currUnit));
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)(val&0xff));
    Wire.write((uint8_t)((val>>8)&0xff));
    Wire.endTransmission();
}



