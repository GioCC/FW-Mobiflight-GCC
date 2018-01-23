// MFButton.h
//
/// \mainpage MF Button module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFButton_h
#define MFButton_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <bitStore.h>

extern "C"
{
  // callback functions always follow the signature: void cmd(void);
  typedef void (*buttonEvent) (byte, uint8_t, const String);
};

enum
{
  btnOnPress,
  btnOnRelease,
};

/////////////////////////////////////////////////////////////////////
/// \class MFButton MFButton.h <MFButton.h>
class MFButton
{
public:
    static void attachHandler(byte eventId, buttonEvent newHandler);
    static void setBitStore(bitStore<byte> *status, bitStore<byte> *upd_status, byte maxOBPin);

    MFButton(uint8_t pin = 1, String name = "Button");
    void update();
    void trigger();
    String        _name;

    byte getPin(void) { return _pin; }

private:
    uint8_t                 _pin;
    static buttonEvent      _handler[2];
    static bitStore<byte>   *_InBits;
    static bitStore<byte>   *_InBitsUpdate;
    static byte             _MaxOnboardPin;

    //bool          _state; // now superceded by lookup into _bits
    //long          _last;
};
#endif
