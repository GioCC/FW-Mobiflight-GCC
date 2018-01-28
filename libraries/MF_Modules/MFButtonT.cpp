// MFButtonT.cpp
//
// Copyright (C) 2013-2014

#include "MFButtonT.h"

// Static handler pointers and vars
buttonEvent      MFButtonT::_handler;
bitStore<byte>  *MFButtonT::_InBits;
bitStore<byte>  *MFButtonT::_InBitsUpdate;
byte             MFButtonT::_MaxOnboardPin;
//String           MFButtonT::_nameStr("12345"); // Pre-allocate, assuring 5+1 chars width

void MFButtonT::attach(uint8_t pin)
{
  _pin  = pin;
  _InBits->set(_pin);
  if(_pin < _MaxOnboardPin) {
    pinMode(_pin, INPUT);     // set pin to input
    digitalWrite(_pin, HIGH); // turn on pullup resistors
  }
}

void MFButtonT::attachHandler(/*byte eventId, */ buttonEvent newHandler)
{
    //MFButtonT::_handler[eventId] = newHandler;
    MFButtonT::_handler = newHandler;
}

// setBitStore() requires <maxOBPin> in order to know whether to use digitalRead
// or get its value from the bitStore, according to own pin number
void MFButtonT::setBitStore(bitStore<byte> *status, bitStore<byte> *upd_status, byte maxOBPin)
{
    MFButtonT::_InBits         = status;
    MFButtonT::_InBitsUpdate   = upd_status;
    MFButtonT::_MaxOnboardPin  = maxOBPin;
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
      *pt = 'B';
      fast_itoa(&pt[1], _pin);
      //_nameStr = pt;

      if (_handler) {(*_handler)((_InBits->get(_pin)==LOW ? btnOnPress : btnOnRelease), _pin, pt); } //_nameStr); }

      /*
      if (_InBits->get(_pin)==LOW && _handler[btnOnPress]!= NULL) {
        (*_handler[btnOnPress])(btnOnPress, _pin, _nameStr);
      }
      else if (_handler[btnOnRelease] != NULL)
        (*_handler[btnOnRelease])(btnOnRelease, _pin, _nameStr);
      */
}

