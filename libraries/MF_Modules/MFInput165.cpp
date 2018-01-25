// MFInput165.cpp
//
// Copyright (C) 2018

#include "MFInput165.h"

MFInput165::MFInput165()
:MFPeripheral(3)
#ifdef USE_BITSTORE
, _store(NULL)
#endif
{
  //_initialized = false;
}

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
    _initialized = true;
}

void MFInput165::detach()
{
    pinMode(LAT165, INPUT_PULLUP);
    pinMode(CLK165, INPUT_PULLUP);
    pinMode(DTA165, INPUT_PULLUP);
    _initialized = false;
}

#ifdef USE_BITSTORE
void MFInput165::bind(bitStore<byte> *store, byte slot)
{
    _store = store;
    _base  = slot;
}
#endif

void MFInput165::read(byte *dest)
{
    if(!_initialized) return;
#ifdef USE_BITSTORE
    if(!dest && _store) dest = _store->bank(_base);
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

