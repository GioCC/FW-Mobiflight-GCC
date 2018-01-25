// MFIO_MCPS.cpp
//
// Copyright (C) 2018

#include "MFIO_MCPS.h"

MFIO_MCPS::MFIO_MCPS(byte nUnits, byte addr)
: MCP23x17(nUnits, addr)
{
    _pins = 4;
    //_initialized = false;
}

void MFIO_MCPS::attach(int dataInPin, int dataOutPin, int csPin, int clkPin, int moduleCount)
{
    // We should check that the pin nr. values are legal!

    DTIMCP = dataInPin;
    DTOMCP = dataOutPin;
    SELMCP = csPin;
    CLKMCP = clkPin;

    pinMode(SELMCP, OUTPUT);
    pinMode(CLKMCP, OUTPUT);
    pinMode(DTOMCP, OUTPUT);
    pinMode(DTIMCP, INPUT_PULLUP);

    digitalWrite(CLKMCP, HIGH);
    digitalWrite(SELMCP, HIGH);
    digitalWrite(DTOMCP, HIGH);

    _moduleCount = moduleCount;
    _initialized = true;
}

void MFIO_MCPS::detach()
{
    pinMode(SELMCP, INPUT_PULLUP);
    pinMode(CLKMCP, INPUT_PULLUP);
    pinMode(DTOMCP, INPUT_PULLUP);
    pinMode(DTIMCP, INPUT_PULLUP);
    _initialized = false;
}

void SIO_io_23S17::
_sendb(byte val)
{
    //pclr(SEL);
    for (byte k=0x80; k ; k>>=1) {
        // MSB FIRST
        if(val & k) {pset(DTO);} else {pclr(DTO);}
        pset(CLK);
        pclr(CLK);
    }
    //pset(SEL);
}

byte SIO_io_23S17::
_readb(void)
{
    register byte dtin = 0;
    //pclr(SEL);
    pclr(CLK);
    for (byte k=0x80; k ; k>>=1) {
        // MSB FIRST
        pset(CLK);
        if(pget(DTI)) dtin |= k; // [TODO] check if endianness is correct!
        pclr(CLK);
    }
    //pset(SEL);
    return dtin;
}

// Generic register R/W functions (byte/word-wise)

byte SIO_io_23S17::
readB(byte reg)
{
    register byte rval;
    pclr(SEL);
    _sendb(MCP_OPCODER|((_address+_currUnit)<<1));
    _sendb(reg);
    rval = _readb();
    pset(SEL);
    return rval;
}

unsigned int SIO_io_23S17::
readW(byte reg)
{
    register unsigned int rval;
    pclr(SEL);
    _sendb(MCP_OPCODER|((_address+_currUnit)<<1));
    _sendb(reg);
    rval = _readb();
    rval |= (_readb() << 8);
    pset(SEL);
    return rval;
}

void SIO_io_23S17::
writeB(byte reg, byte val)
{
    pclr(SEL);
    _sendb(MCP_OPCODEW|((_address+_currUnit)<<1));
    _sendb(reg);
    _sendb(val);
    pset(SEL);
}

void SIO_io_23S17::
writeW(byte reg, unsigned int val)
{
    pclr(SEL);
    _sendb(MCP_OPCODEW|((_address+_currUnit)<<1));
    _sendb(reg);
    _sendb(val&0xFF);
    _sendb((val>>8)&0xFF);
    pset(SEL);
}


