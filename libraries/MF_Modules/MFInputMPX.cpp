// MFInputMPX.cpp
//
// Copyright (C) 2018

#include "MFInputMPX.h"

t_selector_fn  MFInputMPX::_selector;

MFInputMPX::MFInputMPX()
:MFPeripheral(0)
{ _moduleCount = 1; }

MFInputMPX::MFInputMPX(t_selector_fn sel)
:MFPeripheral(0)
{ _moduleCount = 1; _selector = sel;}


void
MFInputMPX::setSelector(t_selector_fn sel)
{
    _selector = sel;
}

byte
MFInputMPX::getPins(byte *dst)   // overrides MPPeripheral::
{
    if(!initialized()) return 0;
    if(dst) dst[0] = _inPin;
    return 1;
}

byte
MFInputMPX::pins(byte n)
{
    return (n==1 ? _inPin : 0xFF);
}

void
MFInputMPX::init(void)
{
    if(initialized()) {
        pinMode(_inPin, INPUT_PULLUP);
    }
}

void
MFInputMPX::attach(int inPin)
{
    // We should also check that the pin nr. values are legal!
    initialize(true);
    if(initialized()) {
        _inPin = inPin;
        npins(1);
        changed = 0;
        init();
    }
}

void
MFInputMPX::detach()
{
    npins(0);
    _inPin = 0xFF;
    initialize(false);
}

void
MFInputMPX::scan(byte *dst)
{
    byte ivec[2] = {0, 0};
    byte iidx;

    if(!initialized())  return;
#ifdef USE_BITSTORE
    //inputs = _store->bank(_base);
    inputs = (_storeNew ? _storeNew->bank(_base) : NULL);
#endif
    if(!inputs && !dst) return;

    for(byte s=16; s>0; s--) {
        MFInputMPX::_selector(s-1);
        iidx = ((s-1)>>3)&0x01;
        ivec[iidx] <<= 1;
        ivec[iidx] |= (digitalRead(_inPin) ? 0x00 : 0x01);    // Negative logic!
    }
    for(iidx=0; iidx<2; iidx++) {
        if(dst)     { dst[iidx] = ivec[iidx]; }
        if(inputs)  { inputs[iidx] = ivec[iidx]; }
    }
}


