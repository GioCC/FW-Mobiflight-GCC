// MFStepper.h
//
/// \mainpage MF Stepper module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFStepper_h
#define MFStepper_h

#include <stdlib.h>

#include <Arduino.h>

#include "../AccelStepper/AccelStepper.h"
#include "MFButtonT.h"
#include <MFPeripheral.h>

/////////////////////////////////////////////////////////////////////
/// \class MFStepper MFStepper.h <MFServo.h>
class MFStepper
: public MFPeripheral
{
public:
    MFStepper(uint8_t pin1 = 1, uint8_t pin2 = 2, uint8_t pin3 = 3, uint8_t pin4 = 4, uint8_t btnPin1 = 0);
    void update(void);
    ///TODO Currently, attach(....) does nothing - the constructor compels to pass the pin numbers
    /// because this is required by base-class AccelStepper, which also has no method for changing
    /// the pins assignment later (which could at least allow to pass dummies in the constructor).
    /// A modification to AccelStepper would be required in order to have a working MFStepper::attach(....).
    void attach(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4) {};

    void attach(byte *pm, char *name)   { attach(pm[0], pm[1], pm[2], pm[3]); }    // name unused
    void update(byte *send, byte *get)  { update(); }

    void reset();
    void moveTo(long absolute);
    void setMaxSpeed(float speed);
    void setAcceleration(float acceleration);
    void setZeroInReset();
    void setZero();
    MFButtonT *getButton();


protected:
    byte pins(byte n)    { return (n<4 ? _pin[n] : 0xFF); }

private:
    bool          _resetting;
    AccelStepper  _stepper;
    MFButtonT     _button;
    long          _targetPos;
    byte          _pin[4];
};
#endif
