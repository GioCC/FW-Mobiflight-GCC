// MFButton.cpp
//
// Copyright (C) 2013-2014

#include "MFButton.h"

// Static handler pointers and vars
buttonEvent      MFButton::_handler[2];
bitStore<byte>  *MFButton::_InBits;
bitStore<byte>  *MFButton::_InBitsUpdate;
byte             MFButton::_MaxOnboardPin;

void MFButton::attachHandler(byte eventId, buttonEvent newHandler)
{
    MFButton::_handler[eventId] = newHandler;
}

// setBitStore() requires <maxOBPin> in order to know whether to use digitalRead
// or get its value from the bitStore, according to own pin number
void MFButton::setBitStore(bitStore<byte> *status, bitStore<byte> *upd_status, byte maxOBPin)
{
    MFButton::_InBits         = status;
    MFButton::_InBitsUpdate   = upd_status;
    MFButton::_MaxOnboardPin  = maxOBPin;
}

MFButton::MFButton(uint8_t pin, String name)
{
  _pin  = pin;
  _name = name;
  //_last = millis();
  //_state = 1;
  _InBits->set(_pin);
  if(_pin < _MaxOnboardPin) {
    pinMode(_pin, INPUT);     // set pin to input
    digitalWrite(_pin, HIGH); // turn on pullup resistors
  }
}

void MFButton::update()
{
    //long now = millis();
    //if (now-_last <= 10) return;
    byte newState = ((_pin<_MaxOnboardPin) ? digitalRead(_pin) : _InBitsUpdate->get(_pin));
    //_last = now;
    if (newState!=_InBits->get(_pin)) {
      _InBits->put(_pin, newState);
      trigger();
    }
}

void MFButton::trigger()
{
      if (_InBits->get(_pin)==LOW && _handler[btnOnPress]!= NULL) {
        (*_handler[btnOnPress])(btnOnPress, _pin, _name);
      }
      else if (_handler[btnOnRelease] != NULL)
        (*_handler[btnOnRelease])(btnOnRelease, _pin, _name);
}

