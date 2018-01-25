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
#include <MFPeripheral.h>
#include <MCP23x17.h>


#define SDAMCP  _pin[0]
#define SCLMCP  _pin[1]


/////////////////////////////////////////////////////////////////////
/// \class MFIO_MCP0 MFIO_MCP0.h <MFIO_MCP0.h>
class MFIO_MCP0
: public MCP23x17
{
private:

    byte          readB(byte reg);
    unsigned int  readW(byte reg);
    void          writeB(byte reg, byte val);
    void          writeW(byte reg, unsigned int val);

public:

    MFIO_MCP0(byte nUnits, byte addr);
    void attach(int SDAPin, int SCLPin);
    void detach(void);

};
#endif  //MFIO_MCP0_H
