// MFLCDDisplay.h
//
/// \mainpage MFLCDDisplay module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (info@mobiflight.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFLCDDisplay_h
#define MFLCDDisplay_h

#include <Arduino.h>

//#include "../NewliquidCrystal/LiquidCrystal_I2C.h"
#include "../LiquidCrystal-I2C/LiquidCrystal_I2C.h"
#include <MFPeripheral.h>


/////////////////////////////////////////////////////////////////////
/// \class MFLCDDisplay MFLCDDisplay.h <MFLCDDisplay.h>
class MFLCDDisplay
: public MFPeripheral
{
public:
    MFLCDDisplay();
    void display(char *string);
    void attach(byte address, byte cols, byte lines);

    void attach(byte *pm, char *name) override   { attach(pm[0], pm[1], pm[2]); }    // name unused
    void detach(void) override;
    void update(byte *send, byte *get) override  {}
    byte getPins(byte *dst) override             { /* copy pin nos into dst */ return npins(); }   ///TODO Should return the I2C pins for consistency

    void test(void);
    void powerSavingMode(bool state);

protected:
    byte pins(byte n) { return 0xFF; }  ///TODO Should return the I2C pins for consistency

private:
    LiquidCrystal_I2C  *_lcdDisplay;
    byte    _address;
    byte	_cols;
    byte	_lines;
};
#endif
