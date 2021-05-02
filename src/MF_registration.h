/********************************************************************
*
* MF_registration.h
*
* Declarations for MF_registration.cpp
*
* Project:  MobiFlight v2
* Hardware: Arduino Mega (/ Uno / Nano)
* Author:   Giorgio CROCI CANDIANI g.crocic@gmail.com
* Date:     2018-11
*
********************************************************************/

#ifndef MF_REGISTRATION_H
#define MF_REGISTRATION_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "mobiflight.h"

/// Define a structure to store pin registration data - fixed size for now

// For pin overlay:
// a flag in <pinRegister.Ins> marks that the pin is registered as INPUT
// a flag in <pinRegister.Outs> marks that the pin is registered as OUTPUT
// a flag in <pinRegister.Shared> (implies reg in either <pinsRegIn> or <pinsRegOut>) marks that
//   the pin can be additionally claimed by another client trying to register a sharable
//   input/output pin
// Currently, only Onboard pins can be shared; this is meant to allow for sharing of HW driving lines
// for certain interfaces (e.g. software I2C).
// Conceptually, Virtual pins could also be shared, but it must be defined exactly how,
// what for and to what extent.

#define BUFSZ (roundUp(MAX_LINES))

class pinRegistry {

private:
        byte    prBufI [BUFSZ];
        byte    prBufO [BUFSZ];
        byte    prBufR [NUM_ONB_PINS];
public:
        bitStore<byte>  Ins (prBufI, BUFSZ);
        bitStore<byte>  Outs(prBufO, BUFSZ);
        bitStore<byte>  Sharable(prBufR, NUM_ONB_PINS);
        pinRegistry() {}
};


#endif // MF_REGISTRATION_H
