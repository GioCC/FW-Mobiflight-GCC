// MFButtonT.h
//
/// MF Button module for MobiFlight Framework
/// Modified by GCC
// MFButtonT does not keep the name stored (it isn't used for anything else than
// qualifying an event message) and uses the pin no. in its place (eg "P005")
// In order to maintain compatibility with existing handlers, text is passed as a string;
// however, repeated dynamic allocation is prevented by reusing a class static String object.

#ifndef MFButtonT_h
#define MFButtonT_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <bitStore.h>
#include <MFUtility.h>

extern "C"
{
  // callback functions always follow the signature: void cmd(void);
  typedef void (*buttonEvent) (byte, uint8_t, const char *); //String);
};

enum
{
  btnOnPress,
  btnOnRelease,
};

/////////////////////////////////////////////////////////////////////
/// \class MFButtonT MFButtonT.h <MFButtonT.h>
class MFButtonT
{
public:
    static void setBitStore(bitStore<byte> *status, bitStore<byte> *upd_status, byte maxOBPin);
    static void attachHandler(buttonEvent newHandler);

    uint8_t       _pin;
    //String        _name;

    MFButtonT(void) {}
    void attach(uint8_t pin = 1);
    byte getPin(void)                { return _pin; }
    void trigger(void);
    void update(void);

    // Expose the same interface style as MFPeripheral, but without inheriting it (too much overhead and no real need)
    byte NPins(void)                    { return 1; }
    void attach(byte *pm, char *name)   { attach(pm[0]); }
    void detach(void)                   {}
    void update(byte *send, byte *get)  { update(); }
    byte getPins(byte *dst)             { dst[0]=_pin; return 1; }
    //void test(void) {};
protected:
    byte pins(byte n)    { return (n=0 ? _pin : 0xFF); }

private:
    static buttonEvent      _handler;
    static bitStore<byte>   *_InBits;
    static bitStore<byte>   *_InBitsUpdate;
    static byte             _MaxOnboardPin;
};
#endif
