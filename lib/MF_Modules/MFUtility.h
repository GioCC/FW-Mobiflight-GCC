#ifndef MFUTILITY_H
#define MFUTILITY_H

#include <stdlib.h>
#include <Arduino.h>

#include <avr/pgmspace.h>

void    fast_itoa(char *dst, uint8_t val);
uint8_t fast_atoi(char *src);

#endif  //MFUTILITY_H
