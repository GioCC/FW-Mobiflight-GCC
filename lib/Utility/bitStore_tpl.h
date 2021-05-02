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

template<typename Tadr, Tadr BITSIZE> class bitStore;

template<typename Tadr, Tadr BITSIZE>
class bitStore
{
private:
        Tadr roundUp(Tadr n)     { return (((n)+7)>>3); }     
        Tadr idxB(Tadr n)        { return ((n)>>3); }
        Tadr idxP(Tadr n)        { return ((n)&0x07); }
        Tadr idxM(Tadr n)        { return (1<<idxP(n)); }

protected:

        uint8_t _store[roundUp(BITSIZE)];

public:
        bitStore() {}


        //uint8_t     *base(void)                     { return _store; }
        // Return pointer to bank containing bit <adr>
        uint8_t     *pbank(Tadr adr)                { return (adr<BITSIZE ? &(_store[idxB(adr)]) : NULL); }
        // Return index of bank containing bit <adr>
        Tadr        bitBank(Tadr adr)               { return roundUp(adr); }

        // Bit access methods
        // Addresses for bits are [0...BITSIZE-1]
        uint8_t     get(Tadr adr)                   { return (adr<BITSIZE ? ((_store[idxB(adr)] & idxM(adr)) ? 1 : 0) : 0); }
        void        clr(Tadr adr)                   { if(adr<BITSIZE) _store[idxB(adr)] &= ~idxM(adr); }
        void        set(Tadr adr)                   { if(adr<BITSIZE) _store[idxB(adr)] |= idxM(adr); }
        void        put(Tadr adr, uint8_t val)      { if(adr<BITSIZE) { if(val) set(adr); else clr(adr); }; }

        // Bank access methods
        // Addresses for banks are [0...(BITSIZE/8)-1] (division rounded to upper integer)
        int8_t      getB(Tadr Badr)                 { return (Badr<roundUp(BITSIZE) ? _store[Badr] : 0); }
        void        setB(Tadr Badr, int8_t Bval)    { if(Badr<roundUp(BITSIZE)) { _store[Badr] |= Bval; }; }
        void        clrB(Tadr Badr, int8_t Bval)    { if(Badr<roundUp(BITSIZE)) { _store[Badr] &= ~Bval; }; }
        void        putB(Tadr Badr, int8_t Bval)    { if(Badr<roundUp(BITSIZE)) { _store[Badr] = Bval; }; }

};

#endif // BITSTORE_H

