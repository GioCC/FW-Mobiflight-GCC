// -----
// RotaryEncoder.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// -----

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"
#include <TicksPerSecond.h>
#include <Button.h>


#define LATCHSTATE 3
/**
 * Minimum rotary encoder tick per second to start acceleration.
 * If the speed of ticking is below this value no acceleration
 * is considered, i.e. ticking is by 1.
 */
#define MIN_TPS 5

/**
 * Maximum rotary encoder tick per second when accelerating.
 * If the speed of ticking is above this value then acceleration
 * is considered at full speed.
 */
#define MAX_TPS 15

/**
 * The number of ticks that a rotary encoder should make at full speed
 * to go from minValue to maxValue. If rotary encoder has 20 ticks for
 * a 360 degrees rotation then 5 rotations at full speed will be needed
 * to go from minValue to maxValue.
 */
#define TICKS_AT_MAX_SPEED_FOR_FULL_SPAN 30

class RotaryEncoder
{
public:
  // ----- Constructor -----
  void initialize(int pin1, int pin2);

  void setMinMax(long min, long max);

  // retrieve the current position
  int  getPosition();

  // adjust the current position
  void setPosition(int newPosition);

  // call this function every some milliseconds or by using an interrupt for handling state changes of the rotary encoder.
  void tick(void);

  long calcSpeed(void);

private:
  Button _pin1, _pin2; // Arduino pins used for the encoder.

  int8_t _oldState;

  int _position;     // Internal position (4 times _positionExt)
  int _positionExt;  // External position

  long _minValue;
  long _maxValue;

  TicksPerSecond tps;
};

#endif

// End
