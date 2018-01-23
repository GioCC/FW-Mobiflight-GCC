// MFSegments.h
//
/// \mainpage MF Button module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFSegments_h
#define MFSegments_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include "../LedControl/LedControl.h"
#include <MFPeripheral.h>

/////////////////////////////////////////////////////////////////////
/// \class MFSegments MFSegments.h <MFSegments.h>
class MFSegments
: public MFPeripheral
{
public:
    MFSegments();
    void display(byte module, char *string, byte points, byte mask, bool convertPoints = false);
    void attach(int dataPin, int csPin, int clkPin, int moduleCount, int brightness);
    void detach();
    void test();
    void powerSavingMode(bool state);
    void setBrightness(int module, int value);

    byte getPins(byte *dst) { dst[0]=_pin[0]; dst[1]=_pin[1]; dst[2]=_pin[2]; return _npins; }

private:
    //bool        _initialized;
    LedControl  *_ledControl;
    byte        _moduleCount;
    byte        _pin[3];
};
#endif
