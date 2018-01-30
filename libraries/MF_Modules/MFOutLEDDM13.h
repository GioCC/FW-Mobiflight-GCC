// MFOutLEDDM13.h
//
/// \mainpage MFInput595 - Digital Output module for MobiFlight Framework based on the 74HC595
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani

#ifndef MFOutLEDDM13_H
#define MFOutLEDDM13_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

// Calibrate pulse delay for stability
#define DELAYDM13_US   10

// Disable to prevent use of class bitStore:
#define USE_BITSTORE

#include <MFPeripheral.h>
#include <MFIOBlock.h>

#define DTADM13  _pin[0]
#define LATDM13  _pin[1]
#define CLKDM13  _pin[2]
#define OENDM13  _pin[3]

/////////////////////////////////////////////////////////////////////
/// \class MFOutLEDDM13 MFOutLEDDM13.h <MFOutLEDDM13.h>
class MFOutLEDDM13
: public MFPeripheral,
  public MFIOBlock
{
public:
    MFOutLEDDM13();
    void attach(int dataPin, int csPin, int clkPin, int oenPin, int moduleCount);
    void send(byte *pattern = NULL);

    void attach(byte *pm, char *name) { attach(pm[0], pm[1], pm[2], pm[3], pm[4]); }    // name unused
    void detach(void);
    void update(byte *snd, byte *get) { send(snd); }

    void test(void);
    void powerSavingMode(bool state) { if(OENDM13!=0xFF) digitalWrite(OENDM13, state); }

    byte getBaseSize(void)      { return 2; }   // # of 8-bit banks per base unit
    //byte getPinDir(byte bank)   { return 0x00; }
    byte getInputMap(byte bank) { return 0x00; };
    byte getOutputMap(byte bank){ return 0xFF; };

protected:
    byte pins(byte n)    { return (n<4 ? _pin[n] : 0xFF); }

private:
    byte _pin[4];

};
#endif  //MFOutLEDDM13_H
