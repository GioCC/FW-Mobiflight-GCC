// MFPeripheral.h
//
/// \mainpage MF Button module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

// Base class with common members for MFxxxxx peripherals


#ifndef MFPERIPHERAL_H_INCLUDED
#define MFPERIPHERAL_H_INCLUDED

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

/////////////////////////////////////////////////////////////////////
/// \class MFPeripheral MFPeripheral.h <MFPeripheral.h>
class MFPeripheral
{
public:
    MFPeripheral(uint8_t npins) : _npins(npins), _initialized(false) { };
    virtual byte NPins(void);
    virtual byte getPins(byte *dst) =0;

protected:
    byte    _npins;
    bool    _initialized;
private:
};

#endif // MFPERIPHERAL_H_INCLUDED
