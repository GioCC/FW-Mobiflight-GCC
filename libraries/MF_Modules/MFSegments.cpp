// MFSegments.cpp
//
// Copyright (C) 2013-2014

#include "MFSegments.h"

MFSegments::MFSegments()
:MFPeripheral(3)
{
  // initialize(false);
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
    _ledControl->setChar(module,digit,str.charAt(pos),((1<<digit) & points), true);
    //if (hasPoint) pos++;
    pos++;
  }
  _ledControl->transmit();
}

void MFSegments::setBrightness(int module, int value)
{
  if (!initialized()) return;

  //if (module < _ledControl->getDeviceCount()) // no need to check, this is done by the lib itself
  //_ledControl->setIntensity(module, value);
  _ledControl->setIntensity(module, value);
}

void MFSegments::attach(int dataPin, int csPin, int clkPin, int moduleCount, int brightness)
{
  initialize(true);
  _pin[0] = dataPin; _pin[1] = csPin; _pin[2] = clkPin;
  _ledControl = new MF_LedControl(dataPin, clkPin, csPin, moduleCount);
  _moduleCount = moduleCount;
  _digitBuf = new byte[moduleCount];
  _ledControl->setBuffer(_digitBuf);
  for (int i=0; i!=_moduleCount; ++i) {
    setBrightness(i, brightness);
    _ledControl->shutdown(i,false);
    _ledControl->clearDisplay(i);
  }
}

void MFSegments::detach()
{
  if (!initialized()) return;
  delete _ledControl; _ledControl = NULL;
  delete _digitBuf;   _digitBuf = NULL;
  initialize(false);
}

void MFSegments::powerSavingMode(bool state)
{
  for (byte i=0; i!=_moduleCount; ++i) {
    _ledControl->shutdown(i, state);
  }
}

void MFSegments::test() {
  if (!initialized()) return;
  byte _delay = 10;
  byte module = 0;
  byte digit = 0;

  for (digit=0; digit<8; digit++)
  {
    for (module=0; module!=_moduleCount; ++module) {
      _ledControl->setDigit(module,digit,8,1,true);
      _ledControl->transmit();
    }
    delay(_delay);
  }

  for (digit=0; digit<8; digit++)
  {
    for (module=0; module!=_moduleCount; ++module) {
      _ledControl->setChar(module,7-digit,'-',false,true);
    }
    _ledControl->transmit();
    delay(_delay);
  }

  for (digit=0; digit<8; digit++)
  {
    for (module=0; module!=_moduleCount; ++module) {
      _ledControl->setChar(module,7-digit,' ',false,true);
    }
    _ledControl->transmit();
    delay(_delay);
  }
}
