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

MCP23x17::MCP23x17(byte nUnits, byte addr)
:MFPeripheral(0)
#ifdef USE_BITSTORE
, _store(NULL)
#endif
{
    _nUnits  = nUnits;
    _address = addr & 0x07;
}

#ifdef USE_BITSTORE
void MFIO_MCP0::
bind(bitStore<byte> *store, byte slot)
{
    _store = store;
    _base  = slot;
}
#endif

void MCP23x17::
update(byte *ins = NULL, byte *outs = NULL)
{
    byte idx = 0;
    if(ins==0)  ins  = _store;
    if(outs==0) outs = _store;
    for(_currUnit=0; _currUnit<_nUnits; _currUnit++)
    {
        unsigned int d = 0;
        // sort bytes explicitly to make sure everything is in place
        if(outs) {
            d =  outs[idx] 
            d += (outs[idx+1]<<8);
        }
        writeIOW(d);
        d = readIOW();
        if(ins) {
            ins[idx]   = (d & 0xFF);
            ins[idx+1] = ((d>>8) & 0xFF);
        }
        idx += 2;
    }
    _currUnit = 0;
}

void MCP23x17::
enablePinInt(byte pinno, byte val, byte mode)
{
    byte r;
    byte reg;
    byte m = _pinMask(pinno);

    if(mode != 0xFF) {
        reg = ((pinno>7) ? MCP_INTCAPA : MCP_INTCAPB);
        r = readB(reg);
        writeB(reg, ((mode & 0x02) ? r|m : r & (~m)));

        reg = ((pinno>7) ? MCP_INTCONA : MCP_INTCONB);
        r = readB(reg);
        writeB(reg, ((mode & 0x01) ? r|m : r & (~m)));
    }
    reg = ((pinno>7) ? MCP_GPINTENA : MCP_GPINTENB);
    r = readB(reg);
    writeB(reg, ((val) ? r|m : r & (~m)));
}

void MCP23x17::
enableIOInts(byte bank, byte vec, byte mode)
{
    byte r;
    byte reg;

    for(byte i=0; i<2; i++) {
        if(i==(bank-1) || bank == 0) {
            if(mode != 0xFF) {
                reg = (MCP_INTCAPA + i);
                //r = readB(reg);
                //writeB(reg, ((mode & 0x02) ? r|vec : r & (~vec)));
                writeB(reg, ((mode & 0x02) ? 0xFF : 0x00));

                reg = (MCP_INTCONA + i);
                //r = readB(reg);
                //writeB(reg, ((mode & 0x01) ? r|vec : r & (~vec)));
                writeB(reg, ((mode & 0x01) ? 0xFF : 0x00));
            }
            reg = (MCP_GPINTENA + i);
            //r = readB(reg);
            writeB(reg, vec);
        }
    }
}

byte MCP23x17::
pinInt(byte pinno)
{
    byte r = readB((pinno>7) ? MCP_INTFA : MCP_INTFB);
    return ((r & _pinMask(pinno)) != 0);
}


void MCP23x17::
setupIntOut(byte mode) {
    byte r;
    byte reg;
    reg = MCP_IOCON;
    r = readB(reg);
    if(mode & 0x01) { reg |= 0x40; } else { reg &= ~0x40; }
    if(mode & 0x02) { reg |= 0x01; } else { reg &= ~0x01; }
    if(mode & 0x04) { reg |= 0x02; } else { reg &= ~0x02; }
    writeB(reg, r);
}

byte MCP23x17::
getInts(byte bank, byte reset) {
   _currUnit = bank>>1; 
   byte res = readB(MCP_INTFA + ((bank-1)&0x01));
   if(reset) getIntIOs(bank);
   return res;
}
