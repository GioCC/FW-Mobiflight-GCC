// MFButtonT.cpp
//
// Copyright (C) 2013-2014

#include "MFButtonT.h"

// Static handler pointers and vars
buttonEvent      MFButtonT::_handler[2];
bitStore<byte>  *MFButtonT::_InBits;
bitStore<byte>  *MFButtonT::_InBitsUpdate;
byte             MFButtonT::_MaxOnboardPin;
String           MFButtonT::nameStr("12345"); // Pre-allocate, assuring 5+1 chars width

void MFButtonT::attachHandler(byte eventId, buttonEvent newHandler)
{
    MFButtonT::_handler[eventId] = newHandler;
}

// setBitStore() requires <maxOBPin> in order to know whether to use digitalRead
// or get its value from the bitStore, according to own pin number
void MFButtonT::setBitStore(bitStore<byte> *status, bitStore<byte> *upd_status, byte maxOBPin)
{
    MFButtonT::_InBits         = status;
    MFButtonT::_InBitsUpdate   = upd_status;
    MFButtonT::_MaxOnboardPin  = maxOBPin;
}

MFButtonT::MFButtonT(uint8_t pin, String name)
{
  _pin  = pin;
  //_name = name;
  //_last = millis();
  //_state = 1;
  _InBits->set(_pin);
  if(_pin < _MaxOnboardPin) {
    pinMode(_pin, INPUT);     // set pin to input
    digitalWrite(_pin, HIGH); // turn on pullup resistors
  }
}

void MFButtonT::update()
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

void MFButtonT::trigger()
{
      char pt[5];
      char *pc = pt;
      *pc++ = 'F';
      if(_pin < 100) *pc++ = '0';
      if(_pin < 10)  *pc++ = '0';
      itoa(_pin, pc, 10);
      nameStr = pt;
      if (_InBits->get(_pin)==LOW && _handler[btnOnPress]!= NULL) {
        (*_handler[btnOnPress])(btnOnPress, _pin, nameStr);
      }
      else if (_handler[btnOnRelease] != NULL)
        (*_handler[btnOnRelease])(btnOnRelease, _pin, nameStr);
}

