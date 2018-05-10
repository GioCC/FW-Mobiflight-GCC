/********************************************************************
*
* MobiFlight Project - v2
*
* Panel controller with Arduino for FSX/P3D flight simulators (also X-Plane through XPUIPC)
*
* MobiFlight is a project by Sebastian MOEBIUS
* Version 2 has been improved and expanded by Giorgio CROCI CANDIANI
*
* Project:  MobiFlight v2
* Hardware: Arduino Mega (/ Uno / Nano)
* Author:   Giorgio CROCI CANDIANI g.crocic@gmail.com
* Date:     2018-01
*
* Most of the code for the MobiFlight core in the original '.ino' file
* has now been split to .cpp modules and a .h file in order to achieve
* a more manageable size, and also to allow the project to be built as
* a normal module with a "conventional" IDE (not the Arduino one)
* for easier debugging.
*
* >>> STILL requires to be built with an Arduino-aware toolchain!!!
*
********************************************************************/


#include <Arduino.h>
#include "mobiflight.h"

// 1.0.1 : Nicer firmware update, more outputs (20)
// 1.1.0 : Encoder support, more outputs (30)
// 1.2.0 : More outputs (40), more inputs (40), more led segments (4), more encoders (20), steppers (10), servos (10)
// 1.3.0 : Generate New Serial
// 1.4.0 : Servo + Stepper support
// 1.4.1 : Reduce velocity
// 1.5.0 : Improve servo behaviour
// 1.6.0 : Set name
// 1.6.1 : Reduce servo noise
// 1.7.0 : New Arduino IDE, new AVR, Uno Support
// 1.7.1 : More UNO stability
// 1.7.2 : "???"
// 1.7.3 : Servo behaviour improved, fixed stepper bug #178, increased number of buttons per module (MEGA)
// 1.8.0 : added support for LCDs
// 1.9.0 : Support for rotary encoders with different detent configurations
// 2.0.0 : [WIP] GCC mods
// Setup function
void setup()
{
  Serial.begin(115200);
  attachCommandCallbacks();
  OnResetBoard();
  cmdMessenger.printLfCr();
#ifdef COMPUTE_OBJ_SIZES
  // Just required for debug, so the compiler doesn't dismiss OBJ_SIZE because unused
  if(OBJ_SIZE[0])
    cmdMessenger.printLfCr();
#endif
}

// Loop function
void loop()
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
  updatePowerSaving();

  // if config has been reset
  // and still is not activated
  // do not perform updates
  // to prevent mangling input for config (shared buffers)
  if (!configActivated) return;

  readButtons();
  readEncoder();

  // segments do not need update
  updateSteppers();
  updateServos();
  updateIOBlocks();
}

