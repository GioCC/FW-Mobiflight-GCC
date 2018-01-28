// MFPeripheral.cpp
//

#include "MFPeripheral.h"

byte MFPeripheral::pinCount(void)
{
    return (_npins&0x7F);
}

byte MFPeripheral::getPins(byte *dst)
{
    byte n = _npins&0x7F;
    if(dst){ for(byte i=0; i<n; i++) dst[i]=pins(i); }
    return n;
}

// end MFPeripheral.cpp
