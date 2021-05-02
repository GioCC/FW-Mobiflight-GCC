// MFIOBlock.h
//
/// \mainpage MF IOBlock base class for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani

// Base class with common members for MFxxxxx peripherals


#ifndef MFIOBLOCK_H_INCLUDED
#define MFIOBLOCK_H_INCLUDED

#include <Arduino.h>
#include <bitStore.h>

/////////////////////////////////////////////////////////////////////
/// \class MFIOBlock MFIOBlock.h <MFIOBlock.h>
class MFIOBlock
{
public:
//#ifdef USE_BITSTORE
    //MFIOBlock(void): _base(0), _moduleCount(0), _store(NULL) {}
    MFIOBlock(void): _moduleCount(0),_base(0),_storeVal(NULL),_storeNew(NULL){}
//#else
    //MFIOBlock(void): _moduleCount(0) {}
//#endif
    // Virtual destructor required: this is meant as a (polymorphic) base class, and derived-class objects
    // may possibly be destroyed through a "base class *" rather than a "derived class *".
    virtual ~MFIOBlock(void) {}

    virtual uint8_t getBaseSize(void);  // # of 8-bit banks per base unit
    virtual uint8_t getChainSize(void);
    virtual uint8_t getSize(void);

    // Return patterns of pin direction for specified bank
    virtual uint8_t getInputMap(byte bank) =0;
    virtual uint8_t getOutputMap(byte bank) =0;
    //virtual byte getPinDir(byte bank) =0;     // Bits: Input = 1, Output = 0

// #ifdef USE_BITSTORE
    //void bind(bitStore<byte> *store, byte slot) { _store = store; _base  = slot; }
    void bind(bitStore<uint8_t> *storeVal, bitStore<uint8_t> *storeNew, byte slot) { _storeVal = storeVal; _storeNew = storeNew; _base  = slot; }
    uint8_t getBase(void) { return _base; }
// #endif

protected:
    uint8_t            _moduleCount;
// #ifdef USE_BITSTORE
    uint8_t            _base;
    //bitStore<uint8_t>  *_store;
    bitStore<uint8_t>  *_storeVal;
    bitStore<uint8_t>  *_storeNew;

    uint8_t  *startVal(void)    { return _storeVal->bank(_base); }
    uint8_t  *startNew(void)    { return _storeNew->bank(_base); }
// #endif

private:
    //none
};

#endif // MFIOBLOCK_H_INCLUDED
