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

#include <MF_LedControl.h>
#include <MFPeripheral.h>

/////////////////////////////////////////////////////////////////////
/// \class MFSegments MFSegments.h <MFSegments.h>
class MFSegments
: public MFPeripheral
{
public:
    MFSegments();
    void attach(int dataPin, int csPin, int clkPin, int moduleCount, int brightness);
    void display(byte module, char *string, byte points, byte mask, bool convertPoints = false);

    void attach(byte *pm, char *name)  { attach(pm[0], pm[1], pm[2], pm[3], pm[4]); }    // name unused
    void detach(void);
    void update(byte *send, byte *get) {}

    void test(void);
    void powerSavingMode(bool state);

    void setBrightness(int module, int value);

protected:
    byte pins(byte n)    { return (n<3 ? _pin[n] : 0xFF); }

private:
    MF_LedControl *_ledControl;
    byte          *_digitBuf;
    byte          _moduleCount;
    byte          _pin[3];
};
#endif
