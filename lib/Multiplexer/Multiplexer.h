/*
 *    Multiplexer.h - A simple multiplexer driver stub for Arduino
 *    2018-04 by Giorgio Croci Candiani
 */

#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include <Arduino.h>
//#include <avr/pgmspace.h>

//typedef  void (*t_selector_fn)(byte);

class Multiplexer {
private :

    //static t_selector_fn    selector;
    byte firstPin;
    void fastModeSwitch(byte pin, byte val);

public:

    // Create a new controller
    Multiplexer(void)
    :firstPin(0xFF) {};

    // Create a new controller, specify output pin
    Multiplexer(byte first_pin);

    byte setPins(byte first_pin = 0xFF);
    void select(byte index);
};
#endif	//MULTIPLEXER_H



