// MFIOBlock.h
//
/// \mainpage MF IOBlock base class for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani

// Base class with common members for MFxxxxx peripherals


#ifndef MFIOBLOCK_H_INCLUDED
#define MFIOBLOCK_H_INCLUDED

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef USE_BITSTORE
#include <bitStore.h>
#endif // USE_BITSTORE

/////////////////////////////////////////////////////////////////////
/// \class MFIOBlock MFIOBlock.h <MFIOBlock.h>
class MFIOBlock
{
public:
#ifdef USE_BITSTORE
    //MFIOBlock(void): _base(0), _moduleCount(0), _store(NULL) {}
    MFIOBlock(void): _moduleCount(0),_base(0),_storeVal(NULL),_storeNew(NULL){}
#else
    MFIOBlock(void): _moduleCount(0) {}
#endif
    // Virtual destructor required: this is meant as a (polymorphic) base class, and derived-class objects
    // may possibly be destroyed through a "base class *" rather than a "derived class *".
    virtual ~MFIOBlock(void) {}

    virtual byte getBaseSize(void)  { return 1; }   // # of 8-bit banks per base unit
    virtual byte getChainSize(void) { return _moduleCount; }
    virtual byte getSize(void)      { return _moduleCount*getBaseSize(); }

    // Return patterns of pin direction for specified bank
    virtual byte getInputMap(byte bank) =0;
    virtual byte getOutputMap(byte bank) =0;
    //virtual byte getPinDir(byte bank) =0;     // Bits: Input = 1, Output = 0

#ifdef USE_BITSTORE
    //void bind(bitStore<byte> *store, byte slot) { _store = store; _base  = slot; }
    void bind(bitStore<byte> *storeVal, bitStore<byte> *storeNew, byte slot) { _storeVal = storeVal; _storeNew = storeNew; _base  = slot; }
    byte getBase(void) { return _base; }
#endif

protected:
    byte            _moduleCount;
#ifdef USE_BITSTORE
    byte            _base;
    //bitStore<byte>  *_store;
    bitStore<byte>  *_storeVal;
    bitStore<byte>  *_storeNew;
#endif

private:
    //none
};

#endif // MFIOBLOCK_H_INCLUDED
