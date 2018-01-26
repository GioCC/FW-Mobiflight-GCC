// MFLCDDisplay.h
//
/// \mainpage MFLCDDisplay module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (info@mobiflight.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFLCDDisplay_h
#define MFLCDDisplay_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

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

    void attach(int *params, char *name) {} //TODO generic attach()
    void detach(void);
    void update(byte *send, byte *get)  {}
    byte getPins(byte *dst)             { return _npins; }   ///TODO Should return the I2C pins for consistency

    void test(void);
    void powerSavingMode(bool state);

protected:
    byte pins(byte n) { return 0xFF; }  ///TODO Should return the I2C pins for consistency

private:
    LiquidCrystal_I2C  *_lcdDisplay;
    //bool    _initialized;
    byte    _address;
    byte	_cols;
    byte	_lines;
};
#endif
