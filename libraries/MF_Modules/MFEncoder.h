// MFEncoder.h
//
/// \mainpage MF Encoder module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFEncoder_h
#define MFEncoder_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

///#include "../Button/Button.h"
///#include "../TicksPerSecond/TicksPerSecond.h"
///#include "../RotaryEncoderAcelleration/RotaryEncoderAcelleration.h"
///#include <RotaryEncoder.h>
#include <MFUtility.h>
#include <RotaryEncoderShd.h>
#include <MFPeripheral.h>

extern "C"
{
  typedef void (*encoderEvent) (byte, uint8_t, const char *); //String);
};

#define MF_ENC_MIN -2147483647
#define MF_ENC_MAX 2147483647

enum
{
  encLeft,
  encLeftFast,
  encRight,
  encRightFast
};

/////////////////////////////////////////////////////////////////////
/// \class MFEncoder MFEncoder.h <MFEncoder.h>
class MFEncoder
: public MFPeripheral
{
public:
    MFEncoder(void);
    //static void attachHandler(byte eventId, encoderEvent newHandler);
    static void attachHandler(encoderEvent newHandler);
    void update(void);
    void attach(byte pin1, byte pin2, byte encno); //String name = "Encoder");

    void attach(byte *pm, char *name)   { attach(pm[0], pm[1], pm[2]); } //name); }
    void detach(void) {};
    void update(byte *send, byte *get)  { update(); }
    byte getPins(byte *dst)             { dst[0] = _encoder.pin(1); dst[1] = _encoder.pin(2); return npins(); }

protected:
    byte    pins(byte n) { return _encoder.pin(n+1); }

private:
    static encoderEvent       _handler; //[4];
    //uint8_t                   _pin1;  // contained in _encoder
    //uint8_t                   _pin2;  // contained in _encoder
    RotaryEncoderShd          _encoder;
    //String                    _name;
    char                      _name[2];

    long                      _pos;
};
#endif
