// MFInputMtx.cpp
//
// Copyright (C) 2018

#include "MFInputMtx.h"

MFInputMtx::MFInputMtx()
:MFPeripheral(0)
#ifdef USE_BITSTORE
, _store(NULL)
#endif
{
  // initialize(false);
}

void MFInputMtx::fastModeSwitch(byte pin, byte val)
{
    byte msk = digitalPinToBitMask(pin);
    volatile byte *out;
    out = portModeRegister(digitalPinToPort(pin));
    if (val) { *out |= msk; } else { *out &= ~msk; }
}

byte MFInputMtx::getPins(byte *dst)   // overrides MPPeripheral::
{
    if(!initialized()) return 0;
    if(dst){
        byte i;
        for(i=0; i<_nrows; i++) dst[i] = _row0 + i;
        for(i=0; i<_ncols; i++) dst[_nrows+i] = _col0 + i;
    }
    return npins();
}

byte MFInputMtx::pins(byte n)
{
    return (n<_nrows ? _row0+n : (n<_nrows+_ncols ? _nrows+_col0+n : 0xFF));
}


void MFInputMtx::init(void)
{
    uint16_t msk = 0;
    if(initialized()) {
        for(byte i=_row0; i<_nrows; i++) { pinMode(i, INPUT_PULLUP); }
        for(byte i=_col0; i<_ncols; i++) {
            pinMode(i, INPUT);
            digitalWrite(i, LOW);
        }
    }
    for(byte i=0; i<8; i++) { _inDCnt[i] = _deb_steps; }
}


void MFInputMtx::attach(int Row0, int NRows, int Col0, int NCols)
{
    // We should also check that the pin nr. values are legal!
    initialize(true);
    if(NRows>8 || NCols>8 || NRows<2 || NCols<2) initialize(false);
    // No overlapping ranges!
    if(Row0<=Col0 && (Row0+NRows > Col0)) initialize(false);
    if(Col0<=Row0 && (Col0+NCols > Row0)) initialize(false);
    if(initialized()) {
        _row0   = Row0;
        _col0   = Col0;
        _nrows  = NRows;
        _ncols  = NCols;
        npins(_nrows + _ncols);
        changed = 0;
        _deb_steps = DEB_DEFAULT;
        init();
    }
}

void MFInputMtx::detach()
{
    npins(0);
    _nrows = _ncols = 0;
    _row0  = _col0  = 0xFF;
    initialize(false);
}

#ifdef USE_BITSTORE
void MFInputMtx::bind(bitStore<byte> *store, byte slot)
{
    _store = store;
    _base  = slot;
    inputs = _store->bank(_base);
}
#endif

void MFInputMtx::scanAll(byte *dst)
{
    if(!initialized()) return;
    for(byte c=0; c<_ncols; c++) {
        scanNext(c, dst);
    }
}

// Matrix switch sampling section
void MFInputMtx::scanNext(byte init, byte *dst)
{
    byte ivec = 0;

    if(!initialized()) return;
    if(!inputs && !dst)return;

    if(init) { _currCol = 0;}

    // Build input vector for current column
    fastModeSwitch(_col0+_currCol, HIGH);   // Set as O/P (goes LOW)
    delayMicroseconds(5);   // Don't rush or we're going to miss something
    for(byte r=_nrows; r>0; r--) {
        ivec <<= 1;
        ivec |= (digitalRead(_row0+r-1) ? 0x00 : 0x01);    // Negative logic
    }
    fastModeSwitch(_col0+_currCol, LOW);    // reset to Hi-Z

    /// Debounce input vector
    if (ivec == _inPrev[_currCol]) {
        if (_inDCnt[_currCol] > 0) {
            if (--_inDCnt[_currCol] == 0) {
                if(dst) {
                    dst[_currCol] = ivec;
                } else {
                    inputs[_currCol] = ivec;
                }
                changed = true;     // the caller will reset this, if interested
            }
        }
    } else {
        _inPrev[_currCol] = ivec;
        _inDCnt[_currCol] = _deb_steps;
    }
    if(++_currCol >= _ncols) _currCol=0;
}


