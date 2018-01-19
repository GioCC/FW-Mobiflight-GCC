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

extern "C"
{
  // callback functions always follow the signature: void cmd(void);
  typedef void (*buttonEvent) (byte, uint8_t, const String);
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
    static void attachHandler(byte eventId, buttonEvent newHandler);
    static void setBitStore(bitStore<byte> *status, bitStore<byte> *upd_status, byte maxOBPin);

    uint8_t       _pin;
    //String        _name;

    MFButtonT(uint8_t pin = 1, String name = "Button");
    void update();
    void trigger();
private:
    static buttonEvent      _handler[2];
    static bitStore<byte>   *_InBits;
    static bitStore<byte>   *_InBitsUpdate;
    static byte             _MaxOnboardPin;

    //bool          _state; // now superceded by lookup into _bits
    //long          _last;

    static String nameStr;
};
#endif
