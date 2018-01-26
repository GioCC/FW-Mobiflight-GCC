// MFPeripheral.cpp
//

#include "MFPeripheral.h"

byte MFPeripheral::NPins(void)
{
    return _npins;
}

byte MFPeripheral::getPins(byte *dst)
{
    if(dst){ for(byte i=0; i<_npins; i++) dst[i]=pins(i); }
    return _npins;
}

// end MFPeripheral.cpp
