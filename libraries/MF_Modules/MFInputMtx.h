// MFInputMtx.h
//
/// \mainpage MFInputMtx - Digital Matrix Input module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Giorgio Croci Candiani

/// This class manages an input matrix (max 8x8)

/// Row/Column pinout is specified as sequences of Arduino Pin numbers (all row pins adjacent, column pins too)
///
/// Hardware-wise, columns are used as outputs and rows are read as inputs;
/// Inactive cols are high-z, active cols are pulled low;
/// Rows are high with internal pull-ups (pulled low by cols through closed contacts)
///
/// Input vectors can be read directly from the array inputs[]; this is not packed, i.e. each element
/// corresponds to a column, and the lower bits up to the number of rows are significant while the
/// others are unused. E.g. with 3 rows, for any given input[n] only bits 0,1,2 are significant.
/// Other ways to read inputs are the methods In(row, col) (obvious) or In(bit_no), whereby
/// bit_no is a linear bit address in the total number of bits (e.g.: matrix 3r x 5c, bit_no goes from 0 to 14)
/// with the sequence c0r0, c0r1, ... c0rN, .... cMr0, ... cMrN

#ifndef MFINPUTMTX_H
#define MFINPUTMTX_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

// Calibrate pulse delay for stability
#define DELAYMTX_US   10

// Disable to prevent use of class bitStore:
#define USE_BITSTORE

#include <MFPeripheral.h>
#ifdef USE_BITSTORE
#include <bitStore.h>
#endif

/////////////////////////////////////////////////////////////////////
/// \class MFInputMtx MFInputMtx.h <MFInputMtx.h>
class MFInputMtx
: public MFPeripheral
{
public:
    MFInputMtx();
#ifdef USE_BITSTORE
    void bind(bitStore<byte> *store, byte slot);
#endif
    void attach(int dataPin, int csPin, int clkPin, int moduleCount);
    void scanNext(byte init = 0, byte *dst = NULL);
    void scanAll(byte *dst);

    void attach(byte *pm, char *name)   { attach(pm[0], pm[1], pm[2], pm[3]); }    // name unused
    void detach(void);
    void update(byte *send, byte *get)  { scanAll(get); }
    byte getPins(byte *dst);

    byte in(byte n)         { return (inputs[n/_nrows] & (0x01<<(n%_nrows))); }
    byte in(byte r, byte c) { return (inputs[c] & (0x01<<r)); }
    void init(void);
    void setDebounce(byte steps)   { _deb_steps = steps; }

    byte getSizeRows(void)  { return _nrows; }
    byte getSizeCols(void)  { return _ncols; }
    byte getSize(void)      { return _nrows*_ncols; }

    bool    changed;

    // Allow public access to inputs - if too permissive, make it private
#ifndef USE_BITSTORE
    byte    inputs[8];       // Validated inputs (each array element is a column)
#else
    byte    *inputs;
#endif

protected:
    byte    _row0;
    byte    _col0;
    byte    _nrows;
    byte    _ncols;

    byte    pins(byte n);

private:
#ifdef USE_BITSTORE
    bitStore<byte>  *_store;
    byte    _base;
#endif
    static const byte DEB_DEFAULT = 10;
    byte    _deb_steps;
    byte    _inPrev[8];   // Previous raw input status
    byte    _inDCnt[8];   // debounce counter for inputs
    byte    _currCol;

    void    fastModeSwitch(byte pin, byte val);
};
#endif  //MFInputMtx_H
