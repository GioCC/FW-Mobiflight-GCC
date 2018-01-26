/// Ss_FastArduino library
/// from McMajan Library pack
/// http://www.mcmajan.com/mcmajanwpr/?page_id=1885

// Arduino IDE preprocessor bug prevent - insert me on top of your arduino-code
#if 1

#define USE_FASTARDUINO

#ifndef _FastArduino_h
#define _FastArduino_h
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// following declarations just supply a prototype for type checking
// actual code is implemented by the macros below
//void FdigitalWrite(unsigned char, unsigned char );
//int  FdigitalRead(unsigned char);
//void FshiftOut(unsigned char, unsigned char, unsigned char, unsigned char );
//void FshiftOutMSB(unsigned char, unsigned char, unsigned char );
void HPulse(unsigned char);

#ifdef USE_FASTARDUINO
//#warning USE_FASTARDUINO is active - defining corresponding symbols.

void f_FastdigitalWrite(unsigned char , unsigned char );
int  f_FastdigitalRead(unsigned char);
void f_FastshiftOut(const unsigned char, const unsigned char,const unsigned char,const unsigned char );
void f_FastdigitalPulse(unsigned char , unsigned char);

//#define FdigitalRead(a) f_FastdigitalRead(a)
//#define FdigitalWrite(a,b) f_FastdigitalWrite(a,b)
//#define FshiftOut(a,b,d,v)  f_FastshiftOut(a,b,d,v)
#define digitalRead(a)      f_FastdigitalRead(a)
#define digitalWrite(a,b)   f_FastdigitalWrite(a,b)
#define shiftOut(a,b,d,v)   f_FastshiftOut(a,b,d,v)
#define digitalPulse(a,b)   f_FastdigitalPulse(a,b)

#else

//#define FdigitalWrite(a,b) digitalWrite(a,b)
//#define FshiftOut(a,b,d,v) shiftOut(a,b,d,v)
//#warning USE_FASTARDUINO NOT active - Using default library functions.

#endif
#endif
#endif

