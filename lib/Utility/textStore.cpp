/**
 *  textStore.cpp
 *
*/

#include "textStore.h"

// Add new text string (returns NULL if full)
char *  textStore::
add(const char *s)
{
    uint16_t  len = strlen(s)+1;  // include NUL!
    char *res = NULL;
    if(((_top-_store)+len) <= _maxlen) {
      strcpy(_top, s);
      res = _top;
      _top += len;
  }
  return res;
}

// Return pointer to n-th string (linear seek)
// n >= 1
char *  textStore::
getText(uint16_t n)
{
    char      *cp = _store;
    uint16_t  i = 0;
    if(n==0) return NULL;   // <n> starts from 1
    n--;
    while((cp < _top) && (i<n)) {
       i++;
       cp = strchr(cp, 0);
       cp++;
    }
    return (i<n ? NULL : cp);
}

#ifdef USE_STRINGS

// Add new text from String (returns NULL if full)
char *  textStore::
add(const String s)
{
    return add(s.c_str());
}

// Return n-th text as String (linear seek)
// n >= 1
String  textStore::
getString(uint16_t n)
{
    return String(getText(n));  // String(char *) gracefully takes NULL
}
#endif

// END textStore.cpp
