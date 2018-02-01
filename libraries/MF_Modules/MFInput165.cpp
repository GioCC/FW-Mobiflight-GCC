// MFInput165.cpp
//
// Copyright (C) 2018

#include "MFInput165.h"

MFInput165::MFInput165()
:MFPeripheral(3)
{}

void MFInput165::attach(int dataPin, int csPin, int clkPin, int moduleCount)
{
    // We should check that the pin nr. values are legal!

    DTA165 = dataPin;
    LAT165 = csPin;
    CLK165 = clkPin;

    pinMode(LAT165, OUTPUT);
    pinMode(CLK165, OUTPUT);
    pinMode(DTA165, INPUT_PULLUP);

    digitalWrite(CLK165, LOW);
    digitalWrite(LAT165, HIGH);

    _moduleCount = moduleCount;
    initialize(true);
}

void MFInput165::detach()
{
    pinMode(LAT165, INPUT_PULLUP);
    pinMode(CLK165, INPUT_PULLUP);
    pinMode(DTA165, INPUT_PULLUP);
    initialize(false);
}

void MFInput165::update(byte *dest)
{
    if(!initialized()) return;
#ifdef USE_BITSTORE
    //if(!dest && _store) dest = _store->bank(_base);
    if(!dest && _storeNew) dest = _storeNew->bank(_base);
#endif
    if(!dest) {
        register byte dtin;
        //Evaluate what additional delay value is more suited for stability
        digitalWrite(LAT165, LOW); delayMicroseconds(DELAY165_US);
        digitalWrite(LAT165, HIGH);
        // The IC closest to the root in a chain is the one in the lowest slot
        for(byte i=0; i<_moduleCount; i++){
            dtin = 0;
            for (byte k=0x80; k ; k>>=1) { // MSB FIRST
                digitalWrite(CLK165, HIGH); delayMicroseconds(DELAY165_US);
                digitalWrite(CLK165, LOW);
                if(digitalRead(DTA165)) { dtin |= k; }
            }
            *dest++ = dtin;
        }
    }
}

