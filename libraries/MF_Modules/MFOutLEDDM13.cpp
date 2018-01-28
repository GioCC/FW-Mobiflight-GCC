// MFOutLEDDM13.cpp
//
// Copyright (C) 2018

#include "MFOutLEDDM13.h"

MFOutLEDDM13::MFOutLEDDM13()
:MFPeripheral(3)
#ifdef USE_BITSTORE
, _store(NULL)
#endif
{
  // initialize(false);
}

void MFOutLEDDM13::attach(int dataPin, int csPin, int clkPin, int oenPin, int moduleCount)
{
    // We should check that the pin nr. values are legal!

    DTADM13 = dataPin;
    LATDM13 = csPin;
    CLKDM13 = clkPin;
    OENDM13 = (oenPin>0 ? oenPin : 0xFF);

    pinMode(DTADM13, OUTPUT); digitalWrite(DTADM13, LOW);
    pinMode(LATDM13, OUTPUT); digitalWrite(LATDM13, LOW);
    pinMode(CLKDM13, OUTPUT); digitalWrite(CLKDM13, HIGH);
    if(oenPin > 0) { pinMode(OENDM13, OUTPUT); digitalWrite(OENDM13, LOW); }

    _moduleCount = moduleCount;
    initialize(true);
}

void MFOutLEDDM13::detach(void)
{
    pinMode(LATDM13, INPUT_PULLUP);
    pinMode(CLKDM13, INPUT_PULLUP);
    pinMode(DTADM13, INPUT_PULLUP);
    if(OENDM13!=0xFF) pinMode(OENDM13, INPUT_PULLUP);
    initialize(false);
}

#ifdef USE_BITSTORE
void MFOutLEDDM13::bind(bitStore<byte> *store, byte slot)
{
    _store = store;
    _base  = slot;
}
#endif

void MFOutLEDDM13::send(byte *pattern)
{
    if(!initialized()) return;
#ifdef USE_BITSTORE
    if(!pattern && _store) pattern = _store->bank(_base);
#endif
    if(pattern) {
        register byte dto;
        for(byte i=_moduleCount*2; i;){
            dto = pattern[--i];
            for (byte k=0x80; k ; k>>=1) { // MSB FIRST
                if(dto & k) {
                    digitalWrite(DTADM13, HIGH);
                } else {
                    digitalWrite(DTADM13, LOW);
                }
                delayMicroseconds(DELAYDM13_US);
                digitalWrite(CLKDM13, HIGH); delayMicroseconds(DELAYDM13_US);
                digitalWrite(CLKDM13, LOW);
            }
        }
        digitalWrite(LATDM13, HIGH); delayMicroseconds(DELAYDM13_US);
        digitalWrite(LATDM13, LOW);
    }
}

void MFOutLEDDM13::test(void)
{
    if (!initialized()) return;

    byte _delay = 10;
    byte buf[_moduleCount*2];

    for (byte i=0; i<_moduleCount*2; i++) {
        buf[i] = 0;
    }
    for (byte i=0; i<_moduleCount*2; i++) {
        for (byte dout=0x01; dout; dout<<=1) {
            buf[i] = dout;
            send(buf);
            delay(_delay);
        }
        buf[i] = 0;
    }
    send(buf);
}
