// MFButton.cpp
//
// Copyright (C) 2013-2014

#include "MFButton.h"

// Static handler pointers and vars
buttonEvent  MFButton::_handler[2];
MFButton::MFButton(uint8_t pin, String name)
{   
  _pin  = pin;
  _name = name;
  //_last = millis();
  _state = 1;

  pinMode(_pin, INPUT);     // set pin to input
  digitalWrite(_pin, HIGH); // turn on pullup resistors
}

void MFButton::update()
{
    //long now = millis();
    //if (now-_last <= 10) return;
    byte newState = digitalRead(_pin);
    //_last = now;
    if (newState!=_state) {     
      _state = newState;
      trigger();      
    }
}

void MFButton::trigger()
{
      if (_state==LOW && _handler[btnOnPress]!= NULL) {
        (*_handler[btnOnPress])(btnOnPress, _pin, _name);
      }
      else if (_handler[btnOnRelease] != NULL)
        (*_handler[btnOnRelease])(btnOnRelease, _pin, _name);
}

void MFButton::attachHandler(byte eventId, buttonEvent newHandler)
{
  MFButton::_handler[eventId] = newHandler;
}