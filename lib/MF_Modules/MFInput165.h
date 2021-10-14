// MFInput165.h
//
/// \mainpage MFInput165 - Digital Input module for MobiFlight Framework based on the 74HC165
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani


#ifndef MFINPUT165_H
#define MFINPUT165_H

#include <Arduino.h>

// Calibrate pulse delay for stability
#define DELAY165_US   10

#include <MFPeripheral.h>
#include <MFIOBlock.h>

#define DTA165  _pin[0]
#define LAT165  _pin[1]
#define CLK165  _pin[2]

/////////////////////////////////////////////////////////////////////
/// \class MFInput165 MFInput165.h <MFInput165.h>
class MFInput165
: public MFPeripheral,
  public MFIOBlock
{
public:
    MFInput165();
    void update(byte *dest = NULL);
    void attach(int dataPin, int csPin, int clkPin, int moduleCount);

    void attach(byte *pm, char *name) override { attach(pm[0], pm[1], pm[2], pm[3]); }    // name unused
    void detach(void) override;
    void update(byte *send, byte *get) override { update(get); }

    byte getBaseSize(void)      { return 1; }   // # of 8-bit banks per base unit
    //byte getPinDir(byte bank)   { return 0xFF; }
    byte getInputMap(byte bank) { return 0xFF; };
    byte getOutputMap(byte bank){ return 0x00; };

protected:
    byte pins(byte n) override { return (n<3 ? _pin[n] : 0xFF); }

private:
    byte    _pin[3];
};
#endif  //MFINPUT165_H
