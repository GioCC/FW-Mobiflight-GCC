// MFUtility.cpp
//

#include "MFUtility.h"

//const uint8_t TEN_CONST[11] = {0,10,20,30,40,50,60,70,80,90,100};
const uint8_t TEN_CONST[11] PROGMEM = {0,10,20,30,40,50,60,70,80,90,100};

// Following function quickly converts a byte value into a char array (null-terminated).
// The char string will be exactly 3 chars long, with leading 0's.
void fast_itoa(char *dst, byte val) {
    //char pt[5];
    //char *pc = pt;
    //*pc++ = 'B';
    if(val > 200) {
        *dst++ = '2'; val -= 200;
    } else if(val > 100) {
        *dst++ = '1'; val -= 100;
    } else {
        *dst++ = '0';
    }

    uint8_t b = 5;
    uint8_t step = 3;
    do {
        //if(val >= TEN_CONST[b]) {
        if(val >= pgm_read_byte_near(TEN_CONST+b)) {
            //if(val < TEN_CONST[b+1]) {
            if(val < pgm_read_byte_near(TEN_CONST+b+1)) {
                step = 0;
                break;
            } else {
                b += step; step -= 1;
            }
        } else {
            b -= step; step -= 1;
        }
    } while(step);

    *dst++ = '0' + b;
    val -= TEN_CONST[b];
    *dst++ = '0' + val;
    *dst++ = '\0';
}

// Following function quickly converts a char array (null-terminated) into a byte value.
// The char string must be 1 to 3 chars long; the represented value must be between 0 and 255.
// Leading 0's are OK. If the string is longer that 3 chars, first 3 chars are considered.
// No leading '+' or '-' are allowed.
// Examples of valid strings are '4', '98', '131', '254', '025', '07', '005', '014efgh'
// Anything outside the specs will cause undetermined behaviour.
uint8_t fast_atoi(char *src)
{
    char *t = src;
    uint8_t a = 0;
    uint8_t d = 0;
    uint8_t res = 0;
    if(*t) t++; if(*t) t++; if(*t) t++;
    if(t==src) return 0;
    // Units
    res = (*(--t))-'0';
    if(t==src) return res;
    // Tens
    a = 10;
    for(d = (*(--t))-'0', a = 10; d; a<<=1, d>>=1) if(d & 0x01) res += a;
    if(t==src) return res;
    t--;
    // Hundreds
    if(*t == '2') res+=200; else if(*t == '1') res+=100;
    return res;
}
