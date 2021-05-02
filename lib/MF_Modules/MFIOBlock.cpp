// MFIOBlock.cpp
//
/// \mainpage MF IOBlock base class for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Giorgio Croci Candiani

#include <MFIOBlock.h>

uint8_t 
MFIOBlock::getBaseSize(void)  
    { return 1; }   // # of 8-bit banks per base unit

uint8_t 
MFIOBlock::getChainSize(void)
    { return _moduleCount; }

uint8_t 
MFIOBlock::getSize(void)      
    { return _moduleCount*getBaseSize(); }
