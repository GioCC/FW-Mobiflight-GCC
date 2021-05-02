/********************************************************************
*
* textStore.h
* Manages a heap-like repository of null-terminated strings (char[])
*
*
* Project:  -
* Hardware: -
* Author:   Giorgio CROCI CANDIANI g.crocic@gmail.com
* Date:     2018-01
*
********************************************************************/
// if interface with String objects is desired, define this:
#define USE_STRINGS

#ifndef TEXTSTORE_H
#define TEXTSTORE_H

//#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

#ifdef USE_STRINGS
#include <String.h>
#endif

class textStore
{
protected:

        char        *_store;
        char        *_top;
        uint16_t    _maxlen;

public:
        textStore(char *store, uint16_t capacity)
        : _store(store), _top(store), _maxlen(capacity) { }

        char    *add(const char *s);        // Add new text string (returns NULL if full)
        char    *getText(uint16_t n);       // Return pointer to n-th string (linear seek)

        uint16_t occupation(void) { return (uint16_t)(_top-_store);};

#ifdef USE_STRINGS
        char    *add(const String s);       // Add new text from String (returns NULL if full)
        String  getString(uint16_t n);      // Return n-th text as String (linear seek)
#endif
};

#endif // TEXTSTORE_H

