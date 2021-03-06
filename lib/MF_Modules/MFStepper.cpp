// MFStepper.cpp
//
// Copyright (C) 2013-2014

#include "MFStepper.h"
#include "MFButtonT.h"

uint8_t MFStepper_stepperCount = 0;
MFStepper* MFStepper_steppers[10];

void addStepper(MFStepper *stepper) {
  MFStepper_steppers[MFStepper_stepperCount] = stepper;
  MFStepper_stepperCount ++;
}

void HandlerOnRelease(byte eventId, uint8_t pin, const char *name) {
  if ( eventId != btnPress ) return;
  for (int i=0; i < MFStepper_stepperCount; i++) {
      if (MFStepper_steppers[i]->getButton()->_pin == pin) {
        MFStepper_steppers[i]->setZeroInReset();
        break;
      }
  }
}

MFStepper::MFStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t btnPin5)
: MFPeripheral(4),
  _stepper(AccelStepper::FULL4WIRE, pin4, pin2, pin1, pin3)
{
    _pin[0] = pin1; _pin[1] = pin2; _pin[2] = pin3; _pin[3] = pin4;
    _resetting = false;
    addStepper(this);
    _button.attach(btnPin5);
    // _button.attachHandler(btnPress, HandlerOnRelease);
    // _button.attachHandler(btnRelease, HandlerOnRelease);
    _button.attachHandler(HandlerOnRelease);
}

void MFStepper::moveTo(long absolute)
{
    _resetting = false;
    if (_targetPos != absolute)
    {
      _targetPos = absolute;
      _stepper.moveTo(absolute);
    }
}

void MFStepper::setZero()
{
    _stepper.setCurrentPosition(0);
}

void MFStepper::setZeroInReset()
{
    if (_resetting) {
      _stepper.setCurrentPosition(0);
      _resetting = false;
    }
}

void MFStepper::update(void)
{
    _stepper.run();
    _button.update();
}

void MFStepper::reset()
{
    // we are not a auto reset stepper if this pin is 0
    if (_button._pin==0) return;

    // if we are already resetting ignore next reset command
    if (_resetting) return;

    // flag that we are resetting
    _resetting = true;

    // tell stepper to move counter clockwise for a long while
    _stepper.moveTo(-100000);
}

void MFStepper::setMaxSpeed(float speed) {
    _stepper.setMaxSpeed(speed);
}

void MFStepper::setAcceleration(float acceleration){
    _stepper.setAcceleration(acceleration);
}

MFButtonT * MFStepper::getButton() {
    return &_button;
}

