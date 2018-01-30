// MFSegments.cpp
//
// Copyright (C) 2013-2014

#include "MFSegments.h"

MFSegments::MFSegments()
:MFPeripheral(3)
{
  // initialize(false);
}

void MFSegments::attach(byte dataPin, byte csPin, byte clkPin, byte moduleCount, byte brightness)
{
  initialize(true);
  _pin[0] = dataPin; _pin[1] = csPin; _pin[2] = clkPin;
  MF_LedControl::init(dataPin, clkPin, csPin, moduleCount);
  _moduleCount = moduleCount;
  if(_digitBuf) delete _digitBuf;
  _digitBuf = new byte[moduleCount*8];
  MF_LedControl::setBuffer(_digitBuf);
  for (byte i=0; i!=_moduleCount; ++i) {
    setBrightness(i, brightness);
    MF_LedControl::shutdown(i,false);
    MF_LedControl::clearDisplay(i);
  }
}

void MFSegments::detach()
{
  if (!initialized()) return;
  delete _digitBuf; _digitBuf = NULL;
  initialize(false);
}

void MFSegments::powerSavingMode(bool state)
{
  for (byte i=0; i!=_moduleCount; ++i) {
    MF_LedControl::shutdown(i, state);
  }
}

void MFSegments::display(byte module, char *string, byte points, byte mask, bool convertPoints)
{
  if (!initialized()) return;

  String str = String(string);
  byte digit = 8;
  byte pos = 0;
  for(int i=0; i!=8; i++){
    digit--;
    if(((1<<digit) & mask) == 0) continue;
    //bool hasPoint = pos+1<str.length() && (str.charAt(pos+1)=='.' || str.charAt(pos+1)==',');
    MF_LedControl::setChar(module,digit,str.charAt(pos),((1<<digit) & points), true);
    //if (hasPoint) pos++;
    pos++;
  }
  MF_LedControl::transmit();
}

void MFSegments::setBrightness(byte module, byte value)
{
  if (!initialized()) return;

  //if (module < MF_LedControl::getDeviceCount()) // no need to check, this is done by the lib itself
  //MF_LedControl::setIntensity(module, value);
  MF_LedControl::setIntensity(module, value);
}

void MFSegments::test() {
  if (!initialized()) return;
  byte _delay = 10;
  byte module = 0;
  byte digit = 0;

  for (digit=0; digit<8; digit++)
  {
    for (module=0; module!=_moduleCount; ++module) {
      MF_LedControl::setDigit(module,digit,8,1,true);
      MF_LedControl::transmit();
    }
    delay(_delay);
  }

  for (digit=0; digit<8; digit++)
  {
    for (module=0; module!=_moduleCount; ++module) {
      MF_LedControl::setChar(module,7-digit,'-',false,true);
    }
    MF_LedControl::transmit();
    delay(_delay);
  }

  for (digit=0; digit<8; digit++)
  {
    for (module=0; module!=_moduleCount; ++module) {
      MF_LedControl::setChar(module,7-digit,' ',false,true);
    }
    MF_LedControl::transmit();
    delay(_delay);
  }
}
