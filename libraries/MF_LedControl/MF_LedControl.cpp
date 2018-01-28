/*
 *    MF_LedControl.cpp - A MAX7219 library for Mobiflight Project (by Sebastian Moebius)
 *    Adapted (2018-01 by Giorgio Croci Candiani) from:
 *    LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    This permission notice shall be included in all copies or
 *    substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

// Difference from original LedControl: (mainly in order to spare memory)
// - removed internal status[] buffer (must be supplied externally, now called digits[])
// - tx buffer made static for the class
// - Now all write methods have a 'noTX' argument (except for methods
//   referencing the whole display, like ClearDisplay).
//   If noTX==true, writing only affects the buffer (and a later call
//   to transmit() is REQUIRED to confirm and effect the transmission of data).
//   If noTX==false, transmission occurs immediately (like before).
// - a few optimizations.

#include "MF_LedControl.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

byte MF_LedControl::spidata[16];


MF_LedControl::MF_LedControl(byte dataPin, byte clkPin, byte csPin, byte numUnits)
{
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    if(numUnits<=0 || numUnits>8 ) numUnits=8;
    numDevices=numUnits;
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);
    digitalWrite(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    // for(int i=0;i<64;i++) status[i]=0x00; // removed 2018-01 by GCC
    for(int i=0;i<numDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
        clearDisplay(i);
        //we go into shutdown-mode on startup
        shutdown(i,true);
    }
}

byte MF_LedControl::getDeviceCount()
{
    return numDevices;
}

void MF_LedControl::shutdown(byte addr, bool b)
{
    if(addr<0 || addr>=numDevices)
	return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void MF_LedControl::setScanLimit(byte addr, byte limit) {
    if(addr<0 || addr>=numDevices)
        return;
    if(limit>=0 || limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void MF_LedControl::setIntensity(byte addr, byte intensity) {
    if(addr<0 || addr>=numDevices)
        return;
    if(intensity>=0 || intensity<16)
        spiTransfer(addr, OP_INTENSITY,intensity);

}

void MF_LedControl::clearDisplay(byte addr) {
    byte offset;

    if(addr<0 || addr>=numDevices)
        return;
    offset=addr*8;
    for(byte i=0;i<8;i++) {
        if(digits) digits[offset+i]=0;
        spiTransfer(addr, i+1, 0); //digits[offset+i]);
    }
}

void MF_LedControl::setLed(byte addr, byte row, byte column, bool state, bool noTX)
{
    byte offset;
    byte val=0x00;

    if(!digits) return;
    if(addr<0 || addr>=numDevices)
        return;
    if(row<0 || row>7 || column<0 || column>7)
        return;
    offset=addr*8;
    val=B10000000 >> column;
    if(state)
        digits[offset+row]=digits[offset+row]|val;
    else {
        val=~val;
        digits[offset+row]=digits[offset+row]&val;
    }
    if(!noTX) spiTransfer(addr, row+1,digits[offset+row]);
}

void MF_LedControl::setRow(byte addr, byte row, byte value, bool noTX)
{
    byte offset;
    if(addr<0 || addr>=numDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    if(digits) digits[offset+row]=value;
    if(!noTX) spiTransfer(addr, row+1, value); //digits[offset+row]);
}

void MF_LedControl::setColumn(byte addr, byte col, byte value, bool noTX) {
    byte val;

    if(addr<0 || addr>=numDevices)
        return;
    if(col<0 || col>7)
        return;
    for(byte row=0;row<8;row++) {
        val=value >> (7-row);
        val=val & 0x01;
        setLed(addr,row,col,val,noTX);
    }
}

void MF_LedControl::setDigit(byte addr, byte digit, byte value, bool dp, bool noTX)
{
    byte offset;
    byte v;

    if(addr<0 || addr>=numDevices)
        return;
    if(digit<0 || digit>7 || value>15)
        return;
    offset=addr*8;
    v=charTable[value];
    if(dp)
        v|=B10000000;
    if(digits) digits[offset+digit]=v;
    if(!noTX) spiTransfer(addr, digit+1,v);

}

void MF_LedControl::setChar(byte addr, byte digit, char value, bool dp, bool noTX)
{
    byte offset;
    byte index,v;

    if(addr<0 || addr>=numDevices)
        return;
    if(digit<0 || digit>7)
        return;
    offset=addr*8;
    index=(byte)value;
    if(index >127) {
        //nothing define we use the space char
        value=32;
    }
    v=pgm_read_byte_near(charTable+value);
    if(dp)
        v|=B10000000;
    if(digits) digits[offset+digit]=v;
    if(!noTX) spiTransfer(addr, digit+1,v);
}

void MF_LedControl::spiTransfer(byte addr, volatile byte opcode, volatile byte data)
{
    //Create an array with the data to shift out
    byte offset=addr*2;
    byte maxbytes=numDevices*2;

    for(byte i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line
    digitalWrite(SPI_CS,LOW);
    //Now shift out the data
    for(byte i=maxbytes;i>0;i--)
        shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
    //latch the data onto the display
    digitalWrite(SPI_CS,HIGH);
}

void MF_LedControl::transmit(void)
{
    if(!digits) return;

    for(byte d=0; d<8; d++) {
        //Create an array with the data to shift out
        for(byte u=0; u<numDevices*2; u++) {
            spidata[u+1]= d+1;                //opcode;
            spidata[u]  = digits[(u<<3)+d];  //data;
        }
        digitalWrite(SPI_CS,LOW);
        for(byte i=numDevices*2;i>0;i--) {
            shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
        }
        digitalWrite(SPI_CS,HIGH);
    }
}
