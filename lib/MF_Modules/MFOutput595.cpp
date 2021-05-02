// MFOutput595.cpp
//
// Copyright (C) 2018

#include "MFOutput595.h"

MFOutput595::MFOutput595()
:MFPeripheral(3)
{}

void MFOutput595::attach(int dataPin, int csPin, int clkPin, int moduleCount)
{
    // We should check that the pin nr. values are legal!

    DTA595 = dataPin;
    LAT595 = csPin;
    CLK595 = clkPin;

    pinMode(LAT595, OUTPUT);
    pinMode(CLK595, OUTPUT);
    pinMode(DTA595, OUTPUT);

    digitalWrite(LAT595, HIGH);
    digitalWrite(CLK595, HIGH);
    digitalWrite(DTA595, LOW);

    _moduleCount = moduleCount;
    initialize(true);
}

void MFOutput595::detach()
{
    pinMode(LAT595, INPUT_PULLUP);
    pinMode(CLK595, INPUT_PULLUP);
    pinMode(DTA595, INPUT_PULLUP);
    initialize(false);
}

void MFOutput595::send(byte *pattern)
{
    if(!initialized()) return;
    byte *newpattern = pattern;
// #ifdef USE_BITSTORE
    if(!pattern && _storeVal) {
        pattern    = startVal();
        newpattern = startNew();
    }
// #endif
    if(pattern && newpattern) {
        register byte dto;
        for(byte i=_moduleCount; i;){
            dto = newpattern[--i];
            if(dto != pattern[i]) {
                // Transmit only if something changed
                pattern[i] = dto;
                for (byte k=0x80; k ; k>>=1) { // MSB FIRST
                    if(dto & k) {
                        digitalWrite(DTA595, HIGH);
                    } else {
                        digitalWrite(DTA595, LOW);
                    }
                    delayMicroseconds(DELAY595_US);
                    digitalWrite(CLK595, HIGH); delayMicroseconds(DELAY595_US);
                    digitalWrite(CLK595, LOW);
                }
            }
        }
        digitalWrite(LAT595, LOW); delayMicroseconds(DELAY595_US);
        digitalWrite(LAT595, HIGH);
    }
}

void MFOutput595::test(void)
{
    if (!initialized()) return;

    byte _delay = 10;
    byte module = 0;
    byte dout = 0;
    byte buf[_moduleCount];

    for (module=0; module<_moduleCount; module++) {
        buf[module] = 0;
    }
    for (module=0; module<_moduleCount; module++) {
        for (dout=0x01; dout; dout<<=1) {
            buf[module] = dout;
            send(buf);
            delay(_delay);
        }
        buf[module] = 0;
    }
    send(buf);
}
