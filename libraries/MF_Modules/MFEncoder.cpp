// MFEncoder.cpp
//
// Copyright (C) 2013-2014

#include "MFEncoder.h"

// Static handler pointers
encoderEvent    MFEncoder::_handler;
//String          MFEncoder::nameStr("12345"); // Pre-allocate, assuring 5+1 chars width

MFEncoder::MFEncoder()
: MFPeripheral(2), _encoder()
{
  // initialize(false);
}

void MFEncoder::attach(byte pin1, byte pin2, byte encno) //, String name)
{
  char ns[4];
  _pos   = 0;
  //_pin1  = pin1;
  //_pin2  = pin2;

  //itoa(encno, ns, 10);
  fast_itoa(ns, encno);   // encno is trusted to be <100!
  _name[0] = ns[1];
  _name[1] = ns[2];

  _encoder.initialize(pin1,pin2);
  _encoder.setMinMax(MF_ENC_MIN,MF_ENC_MAX);
  _encoder.setPosition(_pos);

  initialize(true);
}

void MFEncoder::update()
{
  if (!initialized()) return;

  //_encoder.update();
  _encoder.tick();
  long pos = _encoder.getPosition();

  if (pos == _pos) {
    // nothing happened
    return;
  }

  // Build name to pass back
  char nm[4];
  nm[0] = 'E'; nm[1] = _name[0]; nm[2] = _name[1]; nm[3] = 0;

  long delta = pos - _pos;
  long dir = 1;
  if (delta<0) dir = -1;

  long absDelta = abs(delta);
  if (absDelta < 2) {
    // slow turn detected
    /*
    if (dir==1 && _handler[encLeft]!= NULL) {
        (*_handler[encLeft])(encLeft, _pin1, _name);
    } else if(_handler[encRight]!= NULL) {
        (*_handler[encRight])(encRight, _pin2, _name);
    }
    */
    if (_handler) { (*_handler)((dir==1 ? encLeft : encRight), (dir==1 ? _encoder.pin(1) : _encoder.pin(2)), nm); }
  } else {
    // fast turn detected
    /*
    if (dir==1 && _handler[encLeftFast]!= NULL) {
        (*_handler[encLeftFast])(encLeftFast,  _pin1, _name);
    } else if(_handler[encRightFast]!= NULL) {
        (*_handler[encRightFast])(encRightFast, _pin2, _name);
    }
    */
    if (_handler) { (*_handler)((dir==1 ? encLeftFast : encRightFast), (dir==1 ? _encoder.pin(1) : _encoder.pin(2)), nm); }
  }

  // clamp values
  if ( (dir > 0 && (pos + delta*2) > MF_ENC_MAX) || (dir < 0 && (pos - delta*2) < MF_ENC_MIN))
  {
    _encoder.setPosition(0);
    pos = 0;
  }
  _pos = pos;
}

//void MFEncoder::attachHandler(byte eventId, encoderEvent newHandler)
void MFEncoder::attachHandler(encoderEvent newHandler)
{
  //MFEncoder::_handler[eventId] = newHandler;
  MFEncoder::_handler = newHandler;
}
