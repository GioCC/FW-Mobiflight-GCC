// MFIO_MCP0.h
//
/// \mainpage MFIO_MCP0 - Digital Input module for MobiFlight Framework based on the MCP23017 I2C IO expander
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani


#ifndef MFIO_MCP0_H
#define MFIO_MCP0_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <Wire.h>
#include <SlowSoftI2CMaster.h>
#include <MFPeripheral.h>
#include <MCP23x17.h>

#define MCPSDA  _pin[0]
#define MCPSCL  _pin[1]

// Standard I2C pins
//#ifndef SDA_STD_PIN
// For Mega
//#define SDA_STD_PIN  20
//#define SCL_STD_PIN  21
// For UNO
//#define SDASTD  A4
//#define SCLSTD  A5
// For Micro
//#define SDASTD  2
//#define SCLSTD  3
//#endif

/////////////////////////////////////////////////////////////////////
/// \class MFIO_MCP0 MFIO_MCP0.h <MFIO_MCP0.h>
class MFIO_MCP0
: public MCP23x17
{
private:

    SlowSoftI2CMaster   *SWI2C;

    byte          readB(byte adr, byte reg);
    unsigned int  readW(byte adr, byte reg);
    void          writeB(byte adr, byte reg, byte val);
    void          writeW(byte adr, byte reg, unsigned int val);

public:

    MFIO_MCP0(void);
    void    attach(byte addr, byte SDAPin = 0xFF, byte SCLPin = 0xFF, byte nUnits=1);    // Any pin =0xFF means we are using HW I2C

    void    attach(byte *pm, char *name) { attach(pm[0], pm[1]); }    // name unused
    void    detach(void);

};
#endif  //MFIO_MCP0_H
