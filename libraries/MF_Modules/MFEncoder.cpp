// MFEncoder.cpp
//
// Copyright (C) 2013-2014

#include "MFEncoder.h"

// Static handler pointers
encoderEvent    MFEncoder::_handler[4];

MFEncoder::MFEncoder() : _encoder() {
  _initialized = false;
}

void MFEncoder::attach(uint8_t pin1, uint8_t pin2, String name)
{
  _pos   = 0;
  _name  = name;
  _pin1  = pin1;
  _pin2  = pin2;

  _encoder.initialize(pin1,pin2);
  _encoder.setMinMax(MF_ENC_MIN,MF_ENC_MAX);
  _encoder.setPosition(_pos);

  _initialized = true;
}

void MFEncoder::update()
{
  if (!_initialized) return;

  //_encoder.update();
  _encoder.tick();
  long pos = _encoder.getPosition();

  if (pos == _pos) {
    // nothing happened
    return;
  }

  long delta = pos - _pos;
  long dir = 1;
  if (delta<0) dir = -1;

  long absDelta = abs(delta);
  if (absDelta < 2) {
    // slow turn detected
    if (dir==1 && _handler[encLeft]!= NULL) {
        (*_handler[encLeft])(encLeft, _pin1, _name);
    } else if(_handler[encRight]!= NULL) {
        (*_handler[encRight])(encRight, _pin2, _name);
    }
  } else {
    // fast turn detected
    if (dir==1 && _handler[encLeftFast]!= NULL) {
        (*_handler[encLeftFast])(encLeftFast,  _pin1, _name);
    } else if(_handler[encRightFast]!= NULL) {
        (*_handler[encRightFast])(encRightFast, _pin2, _name);
    }
  }

  // clamp values
  if ( (dir > 0 && (pos + delta*2) > MF_ENC_MAX) || (dir < 0 && (pos - delta*2) < MF_ENC_MIN))
  {
    _encoder.setPosition(0);
    pos = 0;
  }
  _pos = pos;
}

void MFEncoder::attachHandler(byte eventId, encoderEvent newHandler)
{
  MFEncoder::_handler[eventId] = newHandler;
}
