/// MCP23x17 class
///
/// Parent class for SIO_io_23017/SIO_io_23S17 (Driver for SIO_io_23x17 I/O cards)
/// 1x MCP23017/23S17 (I2C/SPI I/O expander)
/// This parent class contains all common, application functions.
/// Register addressing and values are the same for either version; only the data transfer
/// is different. The former is implemented here, while the latter is in the
/// specialized end classes, which implement the comm functions for resp. I2C or SPI interfaces.

/// IO structure:
/// I/Os are read/written in byte banks (2 for each unit) which are numbered from 1 up.
/// Each unit has 2 input _and_ 2 output banks; which bits in these banks are significant
/// depends on how the IC is user-configured.
///
/// For further comments see specialized end classes

/// [TODO] WARNING - TO BE COMPLETED!!!
/// Check init values and setup (for reference: MCP23017 class from Adafruit and MCP23S17 Class for Arduino by Cort Buffington & Keith Neufeld
///

#include "MCP23x17.h"

MCP23x17::MCP23x17(void)
:MFPeripheral(0),MFIOBlock()
{}

MCP23x17::~MCP23x17(void)
{}


void MCP23x17::
init(byte addr, byte nUnits)
{
    _moduleCount  = nUnits;
    _address = addr & 0x07;
}

#define FILTER_OUTS(a, i)  ((a[i]&(~_DDR[i])))
#define FILTER_INS(a, i)   ((a[i]&(_DDR[i])))

void MCP23x17::
refresh(byte *ins, byte *outs, byte unit)
{
    if(!initialized()) return;
    byte          *newouts = outs;
    byte          idx = 0;
    unsigned int  d   = 0;
    unsigned int  nd  = 0;
#ifdef USE_BITSTORE
    //if(ins==0)  ins  = _storeNew;
    //if(outs==0) outs = _storeVal;
    if(ins==0)  ins  = _storeNew;
    if(outs==0) { outs = _storeVal; newouts = _storeNew; }
#endif
    for(byte u=0; u<_moduleCount; u++)
    {
        if(unit!=0xFF && u!=unit) continue;
        d  = 0;
        nd = 0;
        // sort bytes explicitly to make sure everything is in place
        idx = unit*2;
        if(outs) {
            // Mask values so as to affect only bits corresponding to Outputs
            d  =  FILTER_OUTS(outs, idx+1); //(outs[idx+1] & (~_DDR[idx+1]));
            d  =  (d<<8) + FILTER_OUTS(outs, idx); //(outs[idx] & (~_DDR[idx]));
            nd =  FILTER_OUTS(newouts, idx+1); //(newouts[idx+1] & (~_DDR[idx+1]));
            nd =  (nd<<8) + FILTER_OUTS(newouts, idx); //(newouts[idx] & (~_DDR[idx]));
            if(nd != d) {
                // Make sure to leave untouched all bits not used as outputs
                outs[idx]   = FILTER_OUTS(newouts, idx)|FILTER_INS(outs, idx); //(newouts[idx] & ~_DDR[idx])|(outs[idx] & _DDR[idx]);
                outs[idx+1] = FILTER_OUTS(newouts, idx+1)|FILTER_INS(outs, idx+1); //(newouts[idx+1] & ~_DDR[idx+1])|(outs[idx+1] & _DDR[idx+1]);
                // Mask values so as to affect only bits corresponding to Outputs
                d =  FILTER_OUTS(outs, idx+1);  //(outs[idx+1] & (~_DDR[idx+1]));
                d =  (d<<8) + FILTER_OUTS(outs, idx);  //(outs[idx] & (~_DDR[idx]));
                writeIOW(u, d);
            }
        }
        if(ins) {
            // Mask values so as to affect only bits corresponding to Inputs
            // Make sure to leave untouched all bits not used as inputs
            d = readIOW(u);
            ins[idx]   = FILTER_OUTS(ins, idx)  |(d & _DDR[idx]);   // (ins[idx] & ~_DDR[idx])|(d & _DDR[idx]);
            ins[idx+1] = FILTER_OUTS(ins, idx+1)|((d>>8) & _DDR[idx+1]);  // (ins[idx+1] & ~_DDR[idx+1])|((d>>8) & _DDR[idx+1]);
        }
        idx += 2;
    }
}

byte MCP23x17::
getIOInts(byte bank, byte *IOstatus)
{
    byte res = getBankInt(bank, 0);
    byte st = getIOStatusAtInt(bank);
    if(IOstatus) *IOstatus = st;
    return res;
}

void MCP23x17::
setupIntOut(byte bank, byte mode) {
    byte r;
    byte reg = MCP_IOCON;
    byte adr = UNITADR;
    r = readB(adr, reg);
    if(mode & 0x01) { reg |= 0x40; } else { reg &= ~0x40; }
    if(mode & 0x02) { reg |= 0x01; } else { reg &= ~0x01; }
    if(mode & 0x04) { reg |= 0x02; } else { reg &= ~0x02; }
    writeB(adr, reg, r);
}

void MCP23x17::
enablePinInt(byte bank, byte pinno, byte val, byte mode)
{
    byte r;
    byte reg;
    byte adr = UNITADR;
    byte m = (1<<(pinno&0x07));

    if(mode != 0xFF) {
        reg = ((bank&0x01) ? MCP_INTCAPB : MCP_INTCAPA);
        r = readB(adr, reg);
        writeB(adr, reg, ((mode & 0x02) ? r|m : r & (~m)));

        reg = ((bank&0x01) ? MCP_INTCONB : MCP_INTCONA);
        r = readB(adr, reg);
        writeB(adr, reg, ((mode & 0x01) ? r|m : r & (~m)));
    }
    reg = ((bank&0x01) ? MCP_GPINTENB : MCP_GPINTENA);
    r = readB(adr, reg);
    writeB(adr, reg, ((val) ? r|m : r & (~m)));
}

void MCP23x17::
enableBankInt(byte bank, byte vec, byte mode)
{
    //byte r;
    byte reg;
    byte adr = UNITADR;
    byte i = bank&0x01;

    if(mode != 0xFF) {
        reg = (MCP_INTCAPA + i);
        //r = readB(adr, reg);
        //writeB(adr, reg, ((mode & 0x02) ? r|vec : r & (~vec)));
        writeB(adr, reg, ((mode & 0x02) ? 0xFF : 0x00));

        reg = (MCP_INTCONA + i);
        //r = readB(adr, reg);
        //writeB(adr, reg, ((mode & 0x01) ? r|vec : r & (~vec)));
        writeB(adr, reg, ((mode & 0x01) ? 0xFF : 0x00));
    }
    reg = (MCP_GPINTENA + i);
    //r = readB(adr, reg);
    writeB(adr, reg, vec);
}

byte MCP23x17::
getPinInt(byte bank, byte pinno)
{
    byte m = (1<<(pinno&0x07));
    byte r = readB(UNITADR, (bank&0x01) ? MCP_INTFB : MCP_INTFA);
    return ((r & m) != 0);
}

byte MCP23x17::
getBankInt(byte bank, byte reset) {
    byte res = readB(UNITADR, MCP_INTFA+(bank&0x01));
    if(reset) getIOStatusAtInt(bank);
    return res;
}

// end MCP23x17.cpp
