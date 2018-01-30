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
#include <MFIOBlock.h>

#define DTA595  _pin[0]
#define LAT595  _pin[1]
#define CLK595  _pin[2]

/////////////////////////////////////////////////////////////////////
/// \class MFOutput595 MFOutput595.h <MFOutput595.h>
class MFOutput595
: public MFPeripheral,
  public MFIOBlock
{
public:
    MFOutput595();
    void attach(int dataPin, int csPin, int clkPin, int moduleCount);
    void send(byte *pattern = NULL);

    void attach(byte *pm, char *name) { attach(pm[0], pm[1], pm[2], pm[3]); }    // name unused
    void detach(void);
    void update(byte *snd, byte *get) { send(snd); }
    void test(void);
    void powerSavingMode(bool state) {}   // Not currently implemented

    byte getBaseSize(void)      { return 1; }   // # of 8-bit banks per base unit
    //byte getPinDir(byte bank)   { return 0x00; }
    byte getInputMap(byte bank) { return 0x00; };
    byte getOutputMap(byte bank){ return 0xFF; };

protected:
    byte pins(byte n)    { return (n<3 ? _pin[n] : 0xFF); }

private:
    byte        _pin[3];
};
#endif  //MFOUTPUT595_H
