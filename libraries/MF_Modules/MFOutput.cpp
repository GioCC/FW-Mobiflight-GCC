// MFOutput.cpp
//
// Copyright (C) 2013-2014

#include "MFOutput.h"

bitStore<byte>  *MFOutput::_OutBits;
byte            MFOutput::_MaxOnboardPin;

// setBitStore() requires <maxOBPin> in order to know whether to use digitalWrite
// or write the value to the bitStore, according to own pin number
void MFOutput::setBitStore(bitStore<byte> *storage, byte maxOBPin)
{
    MFOutput::_OutBits        = storage;
    MFOutput::_MaxOnboardPin  = maxOBPin;
}

void MFOutput::attach(uint8_t pin)
{
  _pin  = pin;
  //_state = false;
  _OutBits->clr(_pin);
  if(_pin < _MaxOnboardPin) { pinMode(_pin, OUTPUT); }
  set(0);
}

void MFOutput::set(bool state)
{
  //_state = state;
  _OutBits->put(_pin, state);
  if(_pin < _MaxOnboardPin) {
      digitalWrite(_pin, state ? HIGH : LOW);
  }
}

void MFOutput::powerSavingMode(bool active)
{
  if (_pin < _MaxOnboardPin) {
    digitalWrite(_pin, (active ? LOW : _OutBits->get(_pin)));
  }
}

