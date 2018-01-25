// MFOutput595.h
//
/// \mainpage MFInput595 - Digital Output module for MobiFlight Framework based on the 74HC595
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani

#ifndef MFOUTPUT595_H
#define MFOUTPUT595_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

// Calibrate pulse delay for stability
#define DELAY595_US   10

// Disable to prevent use of class bitStore:
#define USE_BITSTORE

#include <MFPeripheral.h>
#ifdef USE_BITSTORE
#include <bitStore.h>
#endif

#define DTA595  _pin[0]
#define LAT595  _pin[1]
#define CLK595  _pin[2]

/////////////////////////////////////////////////////////////////////
/// \class MFOutput595 MFOutput595.h <MFOutput595.h>
class MFOutput595
: public MFPeripheral
{
public:
    MFOutput595();
    void attach(int dataPin, int csPin, int clkPin, int moduleCount);
    void detach();
#ifdef USE_BITSTORE
    void bind(bitStore<byte> *store, byte slot);
#endif
    void send(byte *pattern = NULL);
    void test();
    void powerSavingMode(bool state) {}   // Not currently implemented

    byte getPins(byte *dst) { dst[0]=_pin[0]; dst[1]=_pin[1]; dst[2]=_pin[2]; return _npins; }
    byte getSize(void)      { return _moduleCount; }

private:
    byte        _moduleCount;
    byte        _pin[3];

#ifdef USE_BITSTORE
    bitStore<byte>  *_store;
    byte            _base;
#endif
};
#endif  //MFOUTPUT595_H
