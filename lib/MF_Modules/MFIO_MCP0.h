// MFIO_MCP0.h
//
/// \mainpage MFIO_MCP0 - Digital Input module for MobiFlight Framework based on the MCP23017 I2C IO expander
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani

/// WARNING The parent class could manage groups of "multiple" units (just like independent units, but
/// with addresses starting from the one specified);
/// however, in Mobiflight an MCP IOBlock is always based on a _single_ MCP.

#ifndef MFIO_MCP0_H
#define MFIO_MCP0_H

#include <Arduino.h>

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

    // Virtuals from MCP23x17
    byte          readB(byte adr, byte reg) override;
    unsigned int  readW(byte adr, byte reg) override;
    void          writeB(byte adr, byte reg, byte val) override;
    void          writeW(byte adr, byte reg, unsigned int val) override;

public:

    MFIO_MCP0(void);
    //~MFIO_MCP0(void);

    void    attach(byte addr, byte SDAPin, byte SCLPin, byte IOdir1, byte IOdir2); // Any pin =0xFF means we are using HW I2C

    // Virtuals from MCP23x17::MFPeripheral
    void    attach(byte *pm, char *name) override { attach(pm[0], pm[1], pm[2], pm[3], pm[4]); }    // name unused
    void    detach(void) override;

    // Virtuals from MCP23x17::MFIOBlock
    byte    getInputMap(byte bank) { return MCP23x17::getIMap(bank); };
    byte    getOutputMap(byte bank){ return MCP23x17::getOMap(bank); };
    byte    getBaseSize(void)      { return MCP23x17::getBSize(); }   // # of 8-bit banks per base unit

};
#endif  //MFIO_MCP0_H
