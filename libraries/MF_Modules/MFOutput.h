// MFOutput.h
//
/// \mainpage MF Output module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFOutput_h
#define MFOutput_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <bitStore.h>

/////////////////////////////////////////////////////////////////////
/// \class MFOutput MFOutput.h <MFOutput.h>
class MFOutput
{
public:
    static void setBitStore(bitStore<byte> *val_status, bitStore<byte> *new_status, byte maxOBPin);

    MFOutput(void) {}
    void attach(uint8_t pin);
    void set(bool state);
    void powerSavingMode(bool state);
    byte getPin(void) { return _pin; }

    // Expose the same interface style as MFPeripheral, but without inheriting it (too much overhead and no real need)
    // in case it is found fitting to process MFOutput in the same way as MFPeripherals.
    //byte NPins(void)                    { return 1; }
    //void attach(byte *pm, char *name)   { attach(pm[0]); }
    //void detach(void)                   {}
    //void update(byte *send, byte *get)  { set(*send); }
    //byte getPins(byte *dst)             { dst[0]=_pin; return 1; }
    //void test(void) {};
protected:
    byte pins(byte n)    { return (n=0 ? _pin : 0xFF); }

private:
    static bitStore<byte>   *_OutBitsVal;
    static bitStore<byte>   *_OutBitsNew;
    static byte             _MaxOnboardPin;
    uint8_t                 _pin;
    //bool          _state; // now superceded by lookup into _bits
};
#endif
