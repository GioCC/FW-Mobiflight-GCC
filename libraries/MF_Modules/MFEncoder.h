// MFEncoder.h
//
/// \mainpage MF Button module for MobiFlight Framework
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
#include <RotaryEncoderShd.h>
#include <MFPeripheral.h>

extern "C"
{
  typedef void (*encoderEvent) (byte, uint8_t, const String);
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
    static void attachHandler(byte eventId, encoderEvent newHandler);
    MFEncoder();
    void attach(uint8_t pin1, uint8_t pin2, String name = "Encoder");
    void update();

    byte getPins(byte *dst) { dst[0] = _pin1; dst[1] = _pin2; return _npins; }

private:
    //bool                      _initialized;
    static encoderEvent       _handler[4];
    uint8_t                   _pin1;
    uint8_t                   _pin2;
    RotaryEncoderShd          _encoder;
    String                    _name;
    long                      _pos;
};
#endif
