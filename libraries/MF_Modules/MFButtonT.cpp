// MFButtonT.cpp
//
// Copyright (C) 2013-2014

#include "MFButtonT.h"

// Static handler pointers and vars
buttonEvent      MFButtonT::_handler;
bitStore<byte>  *MFButtonT::_InBitsVal;
bitStore<byte>  *MFButtonT::_InBitsNew;
byte             MFButtonT::_MaxOnboardPin;
//String           MFButtonT::_nameStr("12345"); // Pre-allocate, assuring 5+1 chars width

void MFButtonT::attach(uint8_t pin)
{
  _pin  = pin;
  _InBitsVal->set(_pin);
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
void MFButtonT::setBitStore(bitStore<byte> *val_status, bitStore<byte> *new_status, byte maxOBPin)
{
    MFButtonT::_InBitsVal      = val_status;
    MFButtonT::_InBitsNew      = new_status;
    MFButtonT::_MaxOnboardPin  = maxOBPin;
}

void MFButtonT::update()
{
    //long now = millis();
    //if (now-_last <= 10) return;
    byte newState = ((_pin<_MaxOnboardPin) ? digitalRead(_pin) : _InBitsNew->get(_pin));
    //_last = now;
    if (newState!=_InBitsVal->get(_pin)) {
      _InBitsVal->put(_pin, newState);
      trigger();
    }
}

void MFButtonT::trigger()
{
      char pt[5];
      *pt = 'B';
      fast_itoa(&pt[1], _pin);
      //_nameStr = pt;

      if (_handler) {(*_handler)((_InBitsVal->get(_pin)==LOW ? btnPress : btnRelease), _pin, pt); } //_nameStr); }

      /*
      if (_InBits->get(_pin)==LOW && _handler[btnPress]!= NULL) {
        (*_handler[btnPress])(btnPress, _pin, _nameStr);
      }
      else if (_handler[btnRelease] != NULL)
        (*_handler[btnRelease])(btnRelease, _pin, _nameStr);
      */
}

