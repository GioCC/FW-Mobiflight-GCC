/********************************************************************
*
* bitStore.h
* Manages a repository of flags/bits, accessible both bitwise or bank-
* (ie byte-)wise.
*
* Banks are always 8-bit; Tadr specifies the type for the bit index
* (ie the reachable total size in bits for the store).
*
* Project:  -
* Hardware: -
* Author:   Giorgio CROCI CANDIANI g.crocic@gmail.com
* Date:     2018-01
*
********************************************************************/

#ifndef BITSTORE_H
#define BITSTORE_H
#include <Arduino.h>

#define roundUp(n)     (((n)+7)>>3)
#define idxB(n)        ((n)>>3)
#define idxP(n)        ((n)&0x07)
#define idxM(n)        (1<<idxP(n))

template<typename Tadr>
class bitStore
{
protected:

        uint8_t *_store;
        Tadr    BITSIZE;
        Tadr    BYTESIZE;   // Precalculated and stored for access speed

public:
        bitStore(uint8_t *store, Tadr bsize): _store(store), BITSIZE(bsize) { BYTESIZE = sizeBytes(bsize);}

        //uint8_t     *base(void)                     { return _store; }

        // which is the same of the total size in byte required to contain <bitsize> bits
        static Tadr sizeBytes(Tadr bitsize)         { return roundUp(bitsize); }

        void        clr(void)                       { for(Tadr i=0; i<BYTESIZE; i++) _store[i]=0; }

        // Bit access methods
        // Addresses for bits are [0...BITSIZE-1]
        uint8_t     get(Tadr adr)                   { return (adr<BITSIZE ? ((_store[idxB(adr)] & idxM(adr)) ? 1 : 0) : 0); }
        void        clr(Tadr adr)                   { if(adr<BITSIZE) _store[idxB(adr)] &= ~idxM(adr); }
        void        set(Tadr adr)                   { if(adr<BITSIZE) _store[idxB(adr)] |= idxM(adr); }
        void        put(Tadr adr, uint8_t val)      { if(adr<BITSIZE) { if(val) set(adr); else clr(adr); }; }

        // Bank access methods

        // Addresses for banks are [0...(BITSIZE/8)-1] (division rounded to upper integer)
        int8_t      getB(Tadr Badr)                 { return (Badr<BYTESIZE ? _store[Badr] : 0); }
        void        setB(Tadr Badr, int8_t Bval)    { if(Badr<BYTESIZE) { _store[Badr] |= Bval; }; }
        void        clrB(Tadr Badr, int8_t Bval)    { if(Badr<BYTESIZE) { _store[Badr] &= ~Bval; }; }
        void        putB(Tadr Badr, int8_t Bval)    { if(Badr<BYTESIZE) { _store[Badr] = Bval; }; }
        // Return index of bank containing bit <adr>
        static Tadr bankIdxOfBit(Tadr adr)          { return roundUp(adr); }

        // Direct bank access
        // Return pointer to bank containing bit <adr>
        uint8_t     *bankOfBit(Tadr adr)            { return (adr<BITSIZE ? &(_store[idxB(adr)]) : NULL); }
        // Return pointer to n-th bank containing bit <adr>
        uint8_t     *bank(uint8_t Badr)             { return &(_store[Badr]); }

};

#endif // BITSTORE_H

