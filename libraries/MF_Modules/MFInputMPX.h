// MFInputMPX.h
//
/// \mainpage MFInputMPX - Digital Multiplex Input module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani (g.crocic@gmail.com)
// Copyright (C) 2018 Giorgio Croci Candiani

/// This class manages a set of (up to 16) direct inputs multiplexed by a device like a 74HC4067
/// An external selector function must be supplied (see comments below)


#ifndef MFINPUTMPX_H
#define MFINPUTMPX_H

#include <Arduino.h>

// Calibrate pulse delay for stability
#define DELAYMPX_US   10

// Disable to prevent use of class bitStore:
#define USE_BITSTORE

#include <MFPeripheral.h>
#include <MFIOBlock.h>

typedef  void (*t_selector_fn)(byte);

/////////////////////////////////////////////////////////////////////
/// \class MFInputMPX MFInputMPX.h <MFInputMPX.h>
class MFInputMPX
: public MFPeripheral,
  public MFIOBlock
{
public:
    MFInputMPX(void);
    MFInputMPX(t_selector_fn);

    void attach(int inPin);
    void setSelector(t_selector_fn sel);
    void scan(byte *dst);

    // MFPeripheral virtuals:
    void attach(byte *pm, char *name)   { attach(pm[0]); }
    void detach(void);
    void update(byte *send, byte *get)  { scan(get); }
    byte getPins(byte *dst);

    // MFIOBlock virtuals:
    byte getBaseSize(void)  { return 2; }                   // # of 8-bit banks per base unit
    byte getChainSize(void) { return (_moduleCount = 1); }  // 1 unit (non-cascadable)
    byte getInputMap(byte bank) { return 0xFF; };
    byte getOutputMap(byte bank){ return 0x00; };

    bool changed;

    byte in(byte n)         { return (inputs[(n>>3)&0x01] & (0x01<<(n&0x07))); }
    void init(void);

    // Allow public access to inputs - if too permissive, make it private
#ifndef USE_BITSTORE
    byte    inputs[2];       // Validated inputs
#else
    byte    *inputs;
#endif

protected:
    byte    _inPin;

    byte    pins(byte n);   // MFPeripheral virtual

private:
    byte    _inPrev[2];     // Previous raw input status

    // Notes:
    // - The selector function is common to ALL MFInputMPX displays;
    //   this can be changed by not making this member static (and removing the option of
    //   not specifying it in the init function)
    // - This function selects the MPX position given as argument; after the deed is done,
    //   an 0xFF index value ("deselect") will be sent by the caller.
    //   If this is not used, this function is expected to ignore it.
    static t_selector_fn    _selector;

};
#endif  //MFInputMtx_H
