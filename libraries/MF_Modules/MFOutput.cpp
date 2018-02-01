// MFOutput.cpp
//
// Copyright (C) 2013-2014

#include "MFOutput.h"

bitStore<byte>  *MFOutput::_OutBitsVal;
bitStore<byte>  *MFOutput::_OutBitsNew;
byte             MFOutput::_MaxOnboardPin;

// setBitStore() requires <maxOBPin> in order to know whether to use digitalWrite
// or write the value to the bitStore, according to own pin number
void MFOutput::setBitStore(bitStore<byte> *val_status, bitStore<byte> *new_status, byte maxOBPin)
{
    MFOutput::_OutBitsVal     = val_status;
    MFOutput::_OutBitsNew     = new_status;
    MFOutput::_MaxOnboardPin  = maxOBPin;
}

void MFOutput::attach(uint8_t pin)
{
  _pin  = pin;
  //_state = false;
  _OutBitsNew->clr(_pin);
  _OutBitsVal->clr(_pin);
  if(_pin < _MaxOnboardPin) { pinMode(_pin, OUTPUT); }
  set(0);
}

void MFOutput::set(bool state)
{
  //_state = state;
  _OutBitsNew->put(_pin, state);
  if(_pin < _MaxOnboardPin) {
      digitalWrite(_pin, state ? HIGH : LOW);
      _OutBitsVal->put(_pin, state);
  }
}

void MFOutput::powerSavingMode(bool active)
{
  if (_pin < _MaxOnboardPin) {
    digitalWrite(_pin, (active ? LOW : _OutBitsVal->get(_pin)));
  }
}

