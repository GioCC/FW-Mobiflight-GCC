// MFServo.cpp
//
// Copyright (C) 2013-2014

#include "MFServo.h"

void MFServo::moveTo(int absolute)
{
	int newValue = map(absolute, _mapRange[0], _mapRange[1], _mapRange[2], _mapRange[3]);
    if (_targetPos != newValue)
    {
			_targetPos = newValue;
			if (!initialized()) {
			  _servo.attach(_pin);
			 initialize(true);
			}
    }
}

void MFServo::update() {
    // after reaching final position
    // detach the servo to prevent continuous noise
    if (_currentPos == _targetPos) {
        // servo.detach();
        return;
    }
    if (!initialized()) return;
    //if ((millis()-_lastUpdate) < 5) return;

    if (_currentPos > _targetPos) _currentPos--;
    else _currentPos++;

    _lastUpdate = millis();
    _servo.write(_currentPos);
}

void MFServo::detach() {
  if (initialized()) {
    _servo.detach();
    initialize(false);
  }
}

void MFServo::attach(uint8_t pin, bool enable)
{
	// initialize(false);
	_targetPos = 0;
	_currentPos = 0;
	setExternalRange(0,180);
	setInternalRange(0,180);
	_pin = pin;
	_lastUpdate = millis();
}

MFServo::MFServo()
: MFPeripheral(1), _servo()
{}

MFServo::MFServo(uint8_t pin, bool enable) : MFPeripheral(1), _servo()
{
	attach(pin, enable);
}

void MFServo::setExternalRange(int min, int max)
{
	_mapRange[0] = min;
	_mapRange[1] = max;
}

void MFServo::setInternalRange(int min, int max)
{
	_mapRange[2] = min;
	_mapRange[3] = max;
}
