// MFUtility.cpp
//

#include "MFUtility.h"

//const uint8_t TEN_CONST[11] = {0,10,20,30,40,50,60,70,80,90,100};
const uint8_t TEN_CONST[11] PROGMEM = {0,10,20,30,40,50,60,70,80,90,100};

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
