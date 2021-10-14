/********************************************************************
*
* MF_registration.h
*
* Declarations for MF_registration.cpp
*
* Project:  MobiFlight v2
* Hardware: Arduino Mega (/ Uno / Nano)
* Author:   Giorgio CROCI CANDIANI g.crocic@gmail.com
* Date:     2018-11
*
********************************************************************/

#ifndef MF_REGISTRATION_H
#define MF_REGISTRATION_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "mobiflight.h"

/// Define a structure to store pin registration data - fixed size for now

// For pin overlay:
// a flag in <pinRegister.Ins> marks that the pin is registered as INPUT
// a flag in <pinRegister.Outs> marks that the pin is registered as OUTPUT
// a flag in <pinRegister.Shared> (implies reg in either <pinsRegIn> or <pinsRegOut>) marks that
//   the pin can be additionally claimed by another client trying to register a sharable
//   input/output pin
// Currently, only Onboard pins can be shared; this is meant to allow for sharing of HW driving lines
// for certain interfaces (e.g. software I2C).
// Conceptually, Virtual pins could also be shared, but it must be defined exactly how,
// what for and to what extent.
#define roundUp(n) (((n)+7)>>3)

#define BUFSZ (roundUp(MAX_LINES))

class pinRegistry {

private:
        static byte    prBufI [BUFSZ];
        static byte    prBufO [BUFSZ];
        static byte    prBufR [NUM_ONB_PINS];
public:
        bitStore<byte>  Ins;
        bitStore<byte>  Outs;
        bitStore<byte>  Sharable;
        pinRegistry() { 
                Ins.init (&pinRegistry::prBufI[0], BUFSZ); 
                Outs.init(&pinRegistry::prBufO[0], BUFSZ); 
                Sharable.init(&pinRegistry::prBufR[0], NUM_ONB_PINS); 
        }
};

//bool isPinRegisteredForType(byte pin, byte type);
//void registerPin(byte pin, byte type);
byte isPinRegdAsIn(byte pin);
byte isPinRegdAsOut(byte pin);
byte isPinSharable(byte pin);
byte isPinRegistered(byte pin);
//void registerPin(byte pin);
byte registerPin(byte pin, byte isInput, byte sharable, byte checkonly=0);
void clearRegisteredPins(void);
void clearRegisteredPins(byte type);
byte checkVirtualPins(byte base, byte nBanks);
byte registerVirtualPins(MFIOBlock *obj, byte checkOnly);
//byte unregisterVirtualPins(MFIOBlock *obj);
template<class T> byte registerPeripheral(T *vec[], byte &nElem, byte nMax, byte *argList, byte nPins, char *Name);
template<class T> void unregPeripheral(T *vec[], byte &n);
template<class T> byte registerIOB(byte *argList, byte nPins, byte base, byte nBlocks, char *Name);
void unregIOBlocks(void);

void AddOutput(byte pin = 1);
void ClearOutputs(void);
void AddButton(byte pin = 1);
void ClearButtons(void);
void AddEncoder(byte pin1 = 1, byte pin2 = 2, byte type = 0, const char *name = "Encoder");
void ClearEncoders(void);
void AddLedSegment(byte dataPin, byte csPin, byte clkPin, byte numDevices, byte brightness);
void ClearLedSegments(void);
void AddStepper(byte pin1, byte pin2, byte pin3, byte pin4, byte btnPin1);
void ClearSteppers(void);
void AddServo(byte pin);
void ClearServos(void);
void AddLcdDisplay (byte address = 0x24, byte cols = 16, byte lines = 2, const char *name = "LCD");
void ClearLcdDisplays(void);
void ClearIOBlocks(void);

void AddInput165(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices);
void AddOutput595(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices);
void AddOutLEDDM13(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices);
void AddIOMCP0(byte SDAPin, byte SCLPin, byte addr, byte IOdir1, byte IOdir2, byte base);
void AddIOMCPS(byte MOSIPin, byte MISOPin, byte csPin, byte clkPin, byte addr, byte IOdir1, byte IOdir2, byte base);
void AddInputMtx(byte Row0, byte NRows, byte Col0, byte NCols, byte base);
void AddInputMPX(byte inPin, byte firstSelPin, byte base);

#endif // MF_REGISTRATION_H
