// MFPeripheral.h
//
/// \mainpage MF Peripheral base class for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani

// Base class with common members for MFxxxxx peripherals


#ifndef MFPERIPHERAL_H_INCLUDED
#define MFPERIPHERAL_H_INCLUDED

#include <Arduino.h>

/////////////////////////////////////////////////////////////////////
/// \class MFPeripheral MFPeripheral.h <MFPeripheral.h>
class MFPeripheral
{
public:
    explicit MFPeripheral(uint8_t npins) : _npins(npins&0x7F) { };
    // Virtual destructor required: this is meant as a (polymorphic) base class, and derived-class objects
    // may possibly be destroyed through a "base class *" rather than a "derived class *".
    virtual ~MFPeripheral(void) {}

    virtual byte pinCount(void);
    virtual byte getPins(byte *dst);

    virtual void attach(byte *params, char *name) =0;
    virtual void detach(void) {}
    virtual void update(byte *send, byte *get) {}
    virtual void test(void) {}
    virtual byte pins(byte i) =0;       // i = 0..n
    byte mpxAdr(byte n = 0xFF) { if(n!=0xFF) MPX_adr = n; return MPX_adr;}

protected:
    byte initialized(void) { return((_npins&0x80)!=0); }
    void initialize(byte on) { if(on){_npins|=0x80;} else {_npins&=0x7F;} }

    byte npins(byte n = 0xFF) { if(n!=0xFF) {_npins=(_npins&0x80)+(n&0x7F);} return (_npins&0x7F); }

private:
    byte    _npins;     // bit 7 is the 'initialized' flag
    //bool    _initialized;
    byte    MPX_adr;
};

#endif // MFPERIPHERAL_H_INCLUDED
