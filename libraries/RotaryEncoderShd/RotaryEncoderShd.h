// -----
// RotaryEncoderShd.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
//
// Derived from class:
// RotaryEncoder - Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
//
// This modified class uses a static shared TPS object in order to save memory (lots of it)
// when many encoders are used.
// It has the limitation that it must be assumed that the encoders are only activated one at a time.
// Requires an EXTENDED TickPerSecond class, having an additional update() method that takes a 'UnitNo'
// argument (unitNo), in order to reset counters when the TPS is switched.
// -----
// 18.01.2014 Original class created by Matthias Hertel
// 18.01.2018 New class derived by Giorgio Croci Candiani -
// -----

#ifndef ROTARY_ENC_SHD_H
#define ROTARY_ENC_SHD_H

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

class RotaryEncoderShd
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

private:
  Button  _pin1, _pin2; // Arduino pins used for the encoder.

  int8_t  _oldState;

  int     _position;     // Internal position (4 times _positionExt)
  int     _positionExt;  // External position

  long    _minValue;
  long    _maxValue;

  uint8_t _id;

  // Define a shared TicksPerSecond object for all RotaryEncoders
  // This means that only one encoder at a time can be activated!
  static TicksPerSecond   _tps;

  long calcSpeed(void);   // calcSpeed made private (if the TPS is shared, result won't be correct)

};

#endif // ROTARY_ENC_SHD_H

// End
