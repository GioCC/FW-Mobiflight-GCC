// MFIO_MCP0.cpp
//
// Copyright (C) 2018

#include "MFIO_MCP0.h"

MFIO_MCP0::
MFIO_MCP0(byte nUnits, byte addr)
: MCP23x17(nUnits, addr)
{
    //_initialized = false;
    _npins = 2;
}

void MFIO_MCP0::
attach(int SDAPin, int SCLPin)
{
    if(SCLPin && SDAPin) {
        if(SCLPin == SDAPin) return;
        // Not using standard I2C pins:
        // setup a non-std I2C SW library instead of <Wire>
        SWI2C = new SlowSoftI2CMaster(SDAPin, SCLPin, true);
        SWI2C->i2c_init();
    } else {
        SWI2C = NULL;
        // Either pin value =0 means we are using HW I2C
        //TODO Check that no I2C transfer is pending, if I2C is shared
        Wire.begin();
    }
    _initialized = true;
}

void MFIO_MCP0::
detach()
{
    _initialized = false;
}

// Generic register R/W functions (byte/word-wise)

byte MFIO_MCP0::
readB(byte adr, byte reg)
{
    byte val;
    if(!SWI2C) {
        // Using MCP_OPCODEW or MCP_OPCODER for <Wire> below  is unimportant - only part of them is used to form
        // the complete address to be passed to .beginTransmission(). R/W flags will then be added by .write()
        // or .read().
        Wire.beginTransmission((MCP_OPCODEW>>1)|adr);
        Wire.write((uint8_t) reg);
        Wire.endTransmission();
        Wire.requestFrom((MCP_OPCODER>>1)|adr, 1);
        val = Wire.read();
    } else {
        if (!SWI2C->i2c_start(MCP_OPCODEW|(adr<<1))) { /* I2C device busy! */ }
        SWI2C->i2c_write((uint8_t) reg);
        SWI2C->i2c_rep_start(MCP_OPCODER|(adr<<1));
        val = SWI2C->i2c_read(true);
        SWI2C->i2c_stop();
    }
    return val;
}

unsigned int MFIO_MCP0::
readW(byte adr, byte reg)
{
    unsigned int ba = 0;
    if(!SWI2C) {
        Wire.beginTransmission((MCP_OPCODEW>>1)|adr);
        Wire.write((uint8_t) reg);
        Wire.endTransmission();
        Wire.requestFrom((MCP_OPCODER>>1)|adr, 2);
        ba = Wire.read() & 0xFF;
        ba |= (uint16_t)(Wire.read()<<8);
    } else {
        if (!SWI2C->i2c_start(MCP_OPCODEW|adr)) { /* I2C device busy! */ }
        SWI2C->i2c_write((uint8_t) reg);
        SWI2C->i2c_rep_start(MCP_OPCODER|(adr<<1));
        ba = SWI2C->i2c_read(false);
        ba |= ((uint16_t)(SWI2C->i2c_read(true))<<8);
        SWI2C->i2c_stop();
    }
    return ba;
}

void MFIO_MCP0::
writeB(byte adr, byte reg, byte val)
{
    if(!SWI2C) {
        Wire.beginTransmission((MCP_OPCODEW>>1)|adr);
        Wire.write(reg);
        Wire.write(val);
        Wire.endTransmission();
    } else {
        if (!SWI2C->i2c_start(MCP_OPCODEW|(adr<<1))) { /* I2C device busy! */ }
        SWI2C->i2c_write(reg);
        SWI2C->i2c_write(val);
        SWI2C->i2c_stop();
    }
}

void MFIO_MCP0::
writeW(byte adr, byte reg, unsigned int val)
{
    if(!SWI2C) {
        Wire.beginTransmission((MCP_OPCODEW>>1)|adr);
        Wire.write(reg);
        Wire.write((uint8_t)(val&0xff));
        Wire.write((uint8_t)((val>>8)&0xff));
        Wire.endTransmission();
    } else {
        if (!SWI2C->i2c_start(MCP_OPCODEW|(adr<<1))) { /* I2C device busy! */ }
        SWI2C->i2c_write(reg);
        SWI2C->i2c_write((uint8_t)(val&0xff));
        SWI2C->i2c_write((uint8_t)((val>>8)&0xff));
        SWI2C->i2c_stop();
    }
}

// end MFIO_MCP0.cpp
