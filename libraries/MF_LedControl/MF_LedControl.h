/*
 *    MF_LedControl.cpp - A MAX7219 library for Mobiflight Project (by Sebastian Moebius)
 *    Adapted (2018-01 by Giorgio Croci Candiani) from:
 *    LedControl.h - A library for controling Leds with a MAX7219/MAX7221
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

#ifndef MFLEDCONTROL_H
#define MFLEDCONTROL_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <avr/pgmspace.h>

/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static byte charTable[] PROGMEM = {
    // Segments: .abcdefg (according to MAX72xx datasheet)
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B01110111,B00011111,B01001110,B00111101,B01001111,B01000111,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B10000000,B00000001,B10000000,B00000000,
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B01110111,B00011111,B01001110,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B01000110,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B00001111,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};

class MF_LedControl {
 private :
    /* The array for shifting the data to the devices */
    static byte spidata[16];
    /* Send out a single command to the device */
    void spiTransfer(byte addr, byte opcode, byte data);

    /* We keep track of the led-status for all 8 devices in this array */
    //byte status[64];  // removed 2018-01 by GCC
    byte *digits;
    /* Data is shifted out of this pin*/
    byte SPI_MOSI;
    /* The clock is signaled on this pin */
    byte SPI_CLK;
    /* This one is driven LOW for chip selectzion */
    byte SPI_CS;
    /* The maximum number of devices we use */
    byte numDevices;

 public:
    /*
     * Create a new controller
     * Neutral constructor - used for preallocation, will be setup by init()
     */
    MF_LedControl(void)
    :numDevices(0) {};

    /*
     * Create a new controller
     * Params :
     * dataPin		pin on the Arduino where data gets shifted out
     * clockPin		pin for the clock
     * csPin		pin for selecting the device
     * numDevices	maximum number of devices that can be controled
     */
    MF_LedControl(byte dataPin, byte clkPin, byte csPin, byte numDevices=1);

    /*
     * Init a new controller (when neutral constructor was used)
     * Params :
     * see constructor
     */
    void init(byte dataPin, byte clkPin, byte csPin, byte numDevices=1);

    /*
     * Sets the reference to the external digit buffer (size can be custom-tailored).
     * The size must be at least 8*numDevices
     */
    void setBuffer(byte *buf)
      { digits = buf; }

    /*
     * Gets the number of devices attached to this LedControl.
     * Returns :
     * byte	the number of devices on this LedControl
     */
    byte getDeviceCount();

    /*
     * Set the shutdown (power saving) mode for the device
     * Params :
     * addr	The address of the display to control
     * status	If true the device goes into power-down mode. Set to false
     *		for normal operation.
     */
    void shutdown(byte addr, bool status);

    /*
     * Set the number of digits (or rows) to be displayed.
     * See datasheet for sideeffects of the scanlimit on the brightness
     * of the display.
     * Params :
     * addr	address of the display to control
     * limit	number of digits to be displayed (1..8)
     */
    void setScanLimit(byte addr, byte limit);

    /*
     * Set the brightness of the display.
     * Params:
     * addr		the address of the display to control
     * intensity	the brightness of the display. (0..15)
     */
    void setIntensity(byte addr, byte intensity);

    /*
     * Switch all Leds on the display off.
     * Params:
     * addr	address of the display to control
     */
    void clearDisplay(byte addr);

    /*
     * Transmits the whole buffer content.
     * Allows to perform several changes on the buffer and transmit
     * only once they're done.
     */
    void transmit(void);

    /*
     * Set the status of a single Led.
     * Params :
     * addr	address of the display
     * row	the row of the Led (0..7)
     * col	the column of the Led (0..7)
     * state	If true the led is switched on,
     *		if false it is switched off
     */
    void setLed(byte addr, byte row, byte col, bool state, bool noTX=false);

    /*
     * Set all 8 Led's in a row to a new state
     * Params:
     * addr	address of the display
     * row	row which is to be set (0..7)
     * value	each bit set to 1 will light up the
     *		corresponding Led.
     */
    void setRow(byte addr, byte row, byte value, bool noTX=false);

    /*
     * Set all 8 Led's in a column to a new state
     * Params:
     * addr	address of the display
     * col	  column which is to be set (0..7)
     * value	each bit set to 1 will light up the
     *        corresponding Led.
     */
    void setColumn(byte addr, byte col, byte value, bool noTX=false);

    /*
     * Display a hexadecimal digit on a 7-Segment Display
     * Params:
     * addr	address of the display
     * digit	the position of the digit on the display (0..7)
     * value	the value to be displayed. (0x00..0x0F)
     * dp	sets the decimal point.
     */
    void setDigit(byte addr, byte digit, byte value, bool dp, bool noTX=false);

    /*
     * Display a character on a 7-Segment display.
     * There are only a few characters that make sense here :
     *	'0','1','2','3','4','5','6','7','8','9','0',
     *  'A','b','c','d','E','F','H','L','P',
     *  '.','-','_',' '
     * Params:
     * addr	address of the display
     * digit	the position of the character on the display (0..7)
     * value	the character to be displayed.
     * dp	sets the decimal point.
     */
    void setChar(byte addr, byte digit, char value, bool dp, bool noTX=false);
};

#endif	//MFLEDCONTROL_H



