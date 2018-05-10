/*
 *    Multiplexer.cpp - A simple multiplexer driver stub for Arduino
 *    2018-04 by Giorgio Croci Candiani
 */

#include "Multiplexer.h"

Multiplexer::Multiplexer(byte pin0)
{
    setPins(pin0);
}

byte Multiplexer::setPins(byte pin0)
{
    if(pin0 >= 64) return firstPin;       // rough check; should adapt this to actual board used

    // check if any pins were already set
    if(firstPin < 64) {
        for(byte i=firstPin; i<firstPin+4; i++) {
            pinMode(i,INPUT_PULLUP);
            digitalWrite(i,LOW);
        }
    }
    firstPin = pin0;
    /// MPX users 4 consecutive pins, starting from the one specified.
    /// Lowest-numbered pin is the LSBit of mpx adr
    for(byte i=firstPin; i<firstPin+4; i++) {
        pinMode(i,OUTPUT);
        digitalWrite(i,LOW);
    }
    return firstPin;
}

void Multiplexer::fastModeSwitch(byte pin, byte val)
{
    byte msk = digitalPinToBitMask(pin);
    volatile byte *out;
    out = portModeRegister(digitalPinToPort(pin));
    if (val) { *out |= msk; } else { *out &= ~msk; }
}

void Multiplexer::select(byte index)
{
    if(index>=0x0F || firstPin>=64) return;
    byte i = firstPin;
    //digitalWrite(i,index&0x01); i++;    // keep increment outside, in case digitalWrite() is replaced by a macro!
    //digitalWrite(i,index&0x02); i++;
    //digitalWrite(i,index&0x04); i++;
    //digitalWrite(i,index&0x08);
    fastModeSwitch(i,index&0x01); i++;
    fastModeSwitch(i,index&0x02); i++;
    fastModeSwitch(i,index&0x04); i++;
    fastModeSwitch(i,index&0x08);
}

// End MULTIPLEXER.cpp
