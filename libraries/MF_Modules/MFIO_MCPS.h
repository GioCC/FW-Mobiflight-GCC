// MFIO_MCPS.h
//
/// \mainpage MFIO_MCPS - Digital Input module for MobiFlight Framework based on the MCP23S17 SPI IO expander
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani

/// Driver for MCP23S17 I/O card -
/// 1x MCP23S17 (SPI I/O expander)
/// Multiple units can be connected in parallel (SCK, DI, DO lines), provided that
/// the _CS line is only active (low) for ONE unit at a time.
/// This can be achieved by instantiating more than one object, specifying the
/// same SCK, DI, DO line numbers but a different SEL(=_CS) line for each of them.
/// Another option is using the same _CS line but using the address jumpers on the IC.
/// I/Os are read/written in byte banks (2 for each unit) which are numbered from 1 up.
/// Each unit has 2 input _and_ 2 output banks; which bits in these banks are significant
/// depends on how the IC is user-configured.
///

#ifndef MFIO_MCPS_H
#define MFIO_MCPS_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

// Calibrate pulse delay for stability
#define DELAYMCPS_US   2

#include <MFPeripheral.h>
#include <MCP23x17.h>

#define MCPDTI  _pin[0]
#define MCPDTO  _pin[1]
#define MCPSEL  _pin[2]
#define MCPCLK  _pin[3]

/////////////////////////////////////////////////////////////////////
/// \class MFIO_MCPS MFIO_MCPS.h <MFIO_MCPS.h>
class MFIO_MCPS
: public MCP23x17
{
private:

    void          _sendb(byte val);   // for slaved use, does not touch _cs
    byte          _readb(void);       // for slaved use, does not touch _cs

    byte          readB(byte adr, byte reg);
    unsigned int  readW(byte adr, byte reg);
    void          writeB(byte adr, byte reg, byte val);
    void          writeW(byte adr, byte reg, unsigned int val);

public:
    MFIO_MCPS(void);
    void    attach(byte addr, byte dataInPin, byte dataOutPin, byte csPin, byte clkPin, byte nUnits=1);

    void    attach(byte *pm, char *name) { attach(pm[0], pm[1], pm[2], pm[3], pm[4], pm[5]); }    // name unused
    void    detach();

};
#endif  //MFIO_MCPS_H
