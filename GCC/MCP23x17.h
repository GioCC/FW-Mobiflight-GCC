/// MCP23x17 class
///
/// Parent class for MFIO_MCP0/MFIO_MCP0 (Driver for SIO_io_23x17 I/O cards)
/// 1x MCP23017/23S17 (I2C/SPI I/O expander)
/// This parent class contains all common, application functions.
/// Register addressing and values are the same for either version; only the data transfer
/// is different. The former is implemented here, while the latter is in the
/// specialized end classes, which implement the comm functions for resp. I2C or SPI interfaces.

/// IO structure:
/// I/Os are read/written in byte banks (2 for each unit) which are numbered from 1 up.
/// Each unit has 2 input _and_ 2 output banks; which bits in these banks are significant
/// depends on how the IC is user-configured.
/// For further comments see specialized end classes

/// [TODO] WARNING - TO BE COMPLETED!!!
/// Check init values and setup (for reference: MCP23017 class from Adafruit and MCP23S17 Class for Arduino by Cort Buffington & Keith Neufeld
///

#ifndef _MCP23X17_H
#define _MCP23X17_H
#include "Arduino.h"

#define MAX_BANKS   (MAX_CHAINED_UNITS*BANKS_PER_UNIT)

// #Defines for user settings

#define INTPIN_SEP    0x00
#define INTPIN_JOIN   0x01
#define INTPIN_ACTLOW 0x00
#define INTPIN_ACTHI  0x02
#define INTPIN_HILO   0x00
#define INTPIN_ODRN   0x04

#define INTMODE_CHG   0x00
#define INTMODE_CMP   0x01
#define INTMODE_RISE  0x00
#define INTMODE_FALL  0x02

#define INTMODE_NOCHG 0xFF

// #Defines courtesy MCP23S17 Class for Arduino by Cort Buffington & Keith Neufeld

#define MCP_OPCODEW       (0b01000000)  // Opcode for MCP23S17 with LSB (bit0) set to write (0), address OR'd in later, bits 1-3
#define MCP_OPCODER       (0b01000001)  // Opcode for MCP23S17 with LSB (bit0) set to read (1), address OR'd in later, bits 1-3

#define MCP_IODIRA    (0x00)      // MCP23x17 I/O Direction Register
#define MCP_IODIRB    (0x01)      // 1 = Input (default), 0 = Output

#define MCP_IPOLA     (0x02)      // MCP23x17 Input Polarity Register
#define MCP_IPOLB     (0x03)      // 0 = Normal (default)(low reads as 0), 1 = Inverted (low reads as 1)

#define MCP_GPINTENA  (0x04)      // MCP23x17 Interrupt on Change Pin Assignements
#define MCP_GPINTENB  (0x05)      // 0 = No Interrupt on Change (default), 1 = Interrupt on Change

#define MCP_DEFVALA   (0x06)      // MCP23x17 Default Compare Register for Interrupt on Change
#define MCP_DEFVALB   (0x07)      // Opposite of what is here will trigger an interrupt (default = 0)

#define MCP_INTCONA   (0x08)      // MCP23x17 Interrupt on Change Control Register
#define MCP_INTCONB   (0x09)      // 1 = pin is compared to DEFVAL, 0 = pin is compared to previous state (default)

#define MCP_IOCON     (0x0A)      // MCP23x17 Configuration Register
//                    (0x0B)      //     Also Configuration Register

#define MCP_GPPUA     (0x0C)      // MCP23x17 Weak Pull-Up Resistor Register
#define MCP_GPPUB     (0x0D)      // INPUT ONLY: 0 = No Internal 100k Pull-Up (default) 1 = Internal 100k Pull-Up

#define MCP_INTFA     (0x0E)      // MCP23x17 Interrupt Flag Register
#define MCP_INTFB     (0x0F)      // READ ONLY: 1 = This Pin Triggered the Interrupt

#define MCP_INTCAPA   (0x10)      // MCP23x17 Interrupt Captured Value for Port Register
#define MCP_INTCAPB   (0x11)      // READ ONLY: State of the Pin at the Time the Interrupt Occurred

#define MCP_GPIOA     (0x12)      // MCP23x17 GPIO Port Register
#define MCP_GPIOB     (0x13)      // Value on the Port - Writing Sets Bits in the Output Latch

#define MCP_OLATA     (0x14)      // MCP23x17 Output Latch Register
#define MCP_OLATB     (0x15)      // 1 = Latch High, 0 = Latch Low (default) Reading Returns Latch State, Not Port Value!

class MCP23x17
: public MFPeripheral 
{
private:
    // Units for SPI (MCPS) are not chained: just like for the I2C version,
    // they share the same pins, and subsequent units have subsequent addersses.
    static const uint8_t    MAX_UNITS = 4;
    static const uint8_t    BANKS_PER_UNIT = 2;
    static const byte       num_units = 1;

    byte    _buf[MAX_UNITS*BANKS_PER_UNIT];
    byte    _DDR[MAX_UNITS*BANKS_PER_UNIT];

    /// Device-specific functions:
    /// Generic register R/W functions (byte/word-wise)
    /// These are pure virtual, must be defined by specialized classes.

    virtual byte          readB(byte reg) =0;
    virtual unsigned int  readW(byte reg) =0;
    virtual void          writeB(byte reg, byte val) =0;
    virtual void          writeW(byte reg, unsigned int val) =0;
    
protected:

#ifdef USE_BITSTORE
    bitStore<byte>  *_store;
    byte            _base;
#endif

    byte    _nUnits;
    byte    _address;
    byte    _currUnit;

    byte    _pin[4];
        
public:

    MCP23x17(byte nUnits=1, byte addr=0);
    //~MCP23x17();

#ifdef USE_BITSTORE
    void    bind(bitStore<byte> *store, byte slot);
#endif
    byte    getPins(byte *dst) { for(byte i=0; i<_npins; i++) dst[i] = pins()[i]; return _npins; }
    byte    getSize(void)      { return _moduleCount; }
    void    test();
    void    powerSavingMode(bool state) {}   // Not currently implemented

    // I/O setup functions (byte-wise)
    void    setIODirs(byte bank, byte val);     { _currUnit = bank>>1; writeB(MCP_IODIRA   + ((bank-1)&0x01), val); }
    void    setIOInts(byte bank, byte val);     { _currUnit = bank>>1; writeB(MCP_GPINTENA + ((bank-1)&0x01), val); }
    void    setPullups(byte bank, byte val);    { _currUnit = bank>>1; writeB(MCP_GPPUA    + ((bank-1)&0x01), val); }
    byte    getIOInts(byte bank)                { return getInts(bank); }

    // I/O oriented direct R/W functions (byte/word-wise)
    byte          readIOB(byte bank)            { _currUnit = bank>>1; return readB(MCP_GPIOA + ((bank-1)&0x01)); }
    unsigned int  readIOW(void)                 { _currUnit = bank>>1; return readW(MCP_GPIOA); }
    void          writeIOB(byte bank, byte val) { _currUnit = bank>>1; writeB(MCP_GPIOA + ((bank-1)&0x01), val); }
    void          writeIOW(unsigned int val)    { _currUnit = bank>>1; writeW(MCP_GPIOA, val); }

    void    update(byte *ins = NULL, byte *outs = NULL);

    /// Setup Int outputs
    /// Configure the INT outputs. The configuration is common to both port A and B.
    /// mode|0x01 -> Join:        0[default]=each port has its pin, 1=INTA and INTB pins are internally OR'ed
    /// mode|0x02 -> Polarity:    0[default]=Active low, 1=Active high
    /// mode|0x04 -> Open drain:  0[default]=Push-pull, 1=Open drain (overrides polarity)
    /// Use constants: INTPIN_SEP/_JOIN, INTPIN_ACTLOW/_ACTHI, INTPIN_HILO/_ODRN
    void setupIntOut(byte mode);

    /// Set IRQ enable and mode for single pin
    /// val is on/off
    /// mode|0x01 -> Trigger    0[default]=both edges, 1=rising or falling edge only according to Direction
    /// mode|0x02 -> Direction  0[default]=Int on rising edge, 1=Int on falling edge
    /// mode=0xFF (default) means mode is not changed (change only enable status)
    /// Use constants: INTMODE_CHG/_CMP, INTMODE_RISE/_FALL, INTMODE_NOCHG
    void enablePinInt(byte pinno, byte val, byte mode = INTMODE_NOCHG);

    /// Set IRQ enable for whole bank.
    /// <bank> = 1..2, 0 for both.
    /// <mode> is defined as for the single pin function, and - if not 0xFF - it is applied
    /// to ALL pins equally (regardless if their enable is on or off).
    /// If some pins require different modes, the single pin function must be used.
    void enableIOInts(byte bank, byte vec, byte mode = INTMODE_NOCHG);

    /// Return IRQ flag status for single pin. Does NOT reset flag.
    byte pinInt(byte pinno);

    /// Return IRQ flag vectors.
    /// If reset=1 (default), resets INT flags, otherwise leaves them unaltered.
    byte getInts(byte bank, byte reset=1);

    /// Return IO vector at last IRQ. DOES reset flags (actually, it _must_ be called in order to reset flags).
    byte getIntIOs(byte bank)   { return readB(MCP_INTCAPA + ((bank-1)&0x01)); }

};

#endif

