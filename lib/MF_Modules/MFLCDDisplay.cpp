// MFSegments.cpp
//
// Copyright (C) 2013-2014

#include "MFLCDDisplay.h"

MFLCDDisplay::MFLCDDisplay()
: MFPeripheral(2)
{
  // initialize(false);
}

void MFLCDDisplay::display(char *string)
{
  if (!initialized()) return;
  char readBuffer[21] = "";
  for(byte l=0;l!=_lines;l++) {
    _lcdDisplay->setCursor(0, l);
    memcpy(readBuffer, string+_cols*l, _cols);
    _lcdDisplay->print(readBuffer);
  }
}

void MFLCDDisplay::attach(byte address, byte cols, byte lines)
{
  _address = address;
  _cols = cols;
  _lines = lines;
  _lcdDisplay = new LiquidCrystal_I2C( (uint8_t)address, (uint8_t)cols, (uint8_t)lines );
  initialize(true);
  _lcdDisplay->begin();
  _lcdDisplay->backlight();
  test();
}

void MFLCDDisplay::detach()
{
  if (!initialized()) return;
  //delete _lcdDisplay;
  initialize(false);
}

void MFLCDDisplay::powerSavingMode(bool state)
{
  if (state)
    _lcdDisplay->noBacklight();
  else
    _lcdDisplay->backlight();
}

void MFLCDDisplay::test() {
  if (!initialized()) return;

  _lcdDisplay->setCursor(0, (_lines/2)-1);
  for(byte c=0;c!=((_cols-10)/2);c++) {
    _lcdDisplay->print(" ");
  }
  _lcdDisplay->print(F("Mobiflight"));
  for(byte c=0;c!=((_cols-10)/2);c++) {
    _lcdDisplay->print(F(" "));
  }
  _lcdDisplay->setCursor(0, (_lines/2));
  for(byte c=0;c!=((_cols-6)/2);c++) {
    _lcdDisplay->print(F(" "));
  }
  _lcdDisplay->print(F("Rocks!"));
  for(byte c=0;c!=((_cols-6)/2);c++) {
    _lcdDisplay->print(F(" "));
  }
  _lcdDisplay->setCursor(0, 0);
}
