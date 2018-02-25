// MFIO_MCPS.cpp
//
// Copyright (C) 2018

#include "MFIO_MCPS.h"

MFIO_MCPS::MFIO_MCPS(void)
: MCP23x17()
{
    npins(4);
    // initialize(false);
}

//MFIO_MCPS::~MFIO_MCPS() {}

void MFIO_MCPS::
attach(byte addr,
       byte MOSIpin, byte MISOpin, byte csPin, byte clkPin,
       byte IOdir1, byte IOdir2) //, byte nUnits)
{
    ///TODO setup dir1/dir2!!!
    MCP23x17::init(addr, 1); //nUnits);

    // We should check that the pin nr. values are legal!
    MCPDTI = MOSIpin;
    MCPDTO = MISOpin;
    MCPSEL = csPin;
    MCPCLK = clkPin;

    pinMode(MCPSEL, OUTPUT); digitalWrite(MCPSEL, HIGH);
    pinMode(MCPCLK, OUTPUT); digitalWrite(MCPCLK, HIGH);
    pinMode(MCPDTO, OUTPUT); digitalWrite(MCPDTO, HIGH);
    pinMode(MCPDTI, INPUT_PULLUP);

    initialize(true);
}

void MFIO_MCPS::
detach()
{
    pinMode(MCPSEL, INPUT_PULLUP);
    pinMode(MCPCLK, INPUT_PULLUP);
    pinMode(MCPDTO, INPUT_PULLUP);
    pinMode(MCPDTI, INPUT_PULLUP);
    initialize(false);
}

void MFIO_MCPS::
_sendb(byte val)
{
    //pclr(SEL);
    for (byte k=0x80; k ; k>>=1) {
        // MSB FIRST
        digitalWrite(MCPDTO, ((val & k) ? 1 : 0));
        delayMicroseconds(DELAYMCPS_US);
        digitalWrite(MCPCLK, 1);
        delayMicroseconds(DELAYMCPS_US);
        digitalWrite(MCPCLK, 0);
    }
    //pset(SEL);
}

byte MFIO_MCPS::
_readb(void)
{
    register byte dtin = 0;
    //pclr(SEL);
    digitalWrite(MCPCLK, 0);
    for (byte k=0x80; k ; k>>=1) {
        // MSB FIRST
        digitalWrite(MCPCLK, 1);
        delayMicroseconds(DELAYMCPS_US);
        if(digitalRead(MCPDTI)) dtin |= k; //TODO Check if endianness is correct
        digitalWrite(MCPCLK, 0);
    }
    //pset(SEL);
    return dtin;
}

// Generic register R/W functions (byte/word-wise)

byte MFIO_MCPS::
readB(byte adr, byte reg)
{
    register byte rval;
    digitalWrite(MCPSEL, 0);
    _sendb(MCP_OPCODER|(adr<<1));
    _sendb(reg);
    rval = _readb();
    digitalWrite(MCPSEL, 1);
    return rval;
}

unsigned int MFIO_MCPS::
readW(byte adr, byte reg)
{
    register unsigned int rval;
    digitalWrite(MCPSEL, 0);
    _sendb(MCP_OPCODER|(adr<<1));
    _sendb(reg);
    rval = _readb();
    rval |= (_readb() << 8);
    digitalWrite(MCPSEL, 1);
    return rval;
}

void MFIO_MCPS::
writeB(byte adr, byte reg, byte val)
{
    digitalWrite(MCPSEL, 0);
    _sendb(MCP_OPCODEW|(adr<<1));
    _sendb(reg);
    _sendb(val);
    digitalWrite(MCPSEL, 1);
}

void MFIO_MCPS::
writeW(byte adr, byte reg, unsigned int val)
{
    digitalWrite(MCPSEL, 0);
    _sendb(MCP_OPCODEW|(adr<<1));
    _sendb(reg);
    _sendb(val&0xFF);
    _sendb((val>>8)&0xFF);
    digitalWrite(MCPSEL, 1);
}

// end MFIO_MCPS.cpp

