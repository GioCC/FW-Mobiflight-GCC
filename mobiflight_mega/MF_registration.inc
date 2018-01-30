/// ===================
/// MF_registration.inc
///
/// macro inclusion file for mobiflight_mega_ino

bool isPinRegistered(byte pin)
{
  return (pin < MAX_LINES ? pinsRegistered.get(pin) : true);
}

//bool isPinRegisteredForType(byte pin, byte type) {
//  return pinsRegistered[pin] == type;
//}

void registerPin(byte pin)
{
  if(pin < MAX_LINES) pinsRegistered.set(pin);
}

void clearRegisteredPins()
{
    pinsRegistered.clr();
}

//void clearRegisteredPins(byte type) {
//  for(int i=0; i<MODULE_MAX_PINS;++i)
//    if (pinsRegistered[i] == type)
//      pinsRegistered[i] = kTypeNotSet;
//}
void registerVirtualPins(MFPeripheral *obj, byte base)
{
    ///TODO: should be able to read the list of VIRTUAL pins based on:
    /// - the obj's base address
    /// - the obj's unit size (banks)
    /// - the number of units in the module
    /// I/O DDR configuration is luckily irrelevant.
    /// The list of vpins should be also _checked in advance_ before even allocating
    /// the driver pins, to make sure there's no overlay!
    for(byte i=0; i<obj->Size(); i++)

}

void clearRegisteredPins(byte type)
{
  byte p[5];
  byte ne = 0;
  byte np;
  MFPeripheral *mfp = NULL;
  MFPeripheral **mfpp = NULL;

  switch(type) {
    case kTypeButton:
        for(byte i=0; i<buttonsRegistered; i++) pinsRegistered.clr(buttons[i].getPin());
        break;
    case kTypeOutput:
        for(byte i=0; i<outputsRegistered; i++) pinsRegistered.clr(outputs[i].getPin());
        break;
    case kTypeEncoder:
        ne = encodersRegistered;
        mfpp = (MFPeripheral **)encoders;
        break;
    case kTypeLedSegment:
        ne = ledSegmentsRegistered;
        mfpp = (MFPeripheral **)ledSegments;
        break;
    case kTypeServo:
        ne = servosRegistered;
        mfpp = (MFPeripheral **)servos;
        break;
    case kTypeStepper:
        ne = steppersRegistered;
        mfpp = (MFPeripheral **)steppers;
        break;
    case kTypeLcdDisplayI2C:  // this does nothing;
    break;
    // Clearing any of the peripheral types clears all peripherals!
    case kTypeInputMtx:
    case kTypeInput165:
    case kTypeOutput595:
    case kTypeOutLEDDM13:
    case kTypeOutLED5940:
    case kTypeInOutMCPS:
    case kTypeInOutMCP0:
        ne = IOBlocksRegistered;
        /// mfp = IOblocks;     //TODO
    break;
    default:
    break;
  }
  for(byte i=0; i<ne; i++) {
      np = 0;
      if(mfp) {
          np = mfp[i].pinCount();
          mfp[i].getPins(p);
      }
      if(mfpp) {
          np = mfpp[i]->pinCount();
          mfpp[i]->getPins(p);
      }
      for(byte k=0; k<np; k++) pinsRegistered.clr(p[k]);
  }
}

///************************************************************************************************
/// Registration macro

// Object array version:
#define Mregister(x, xNRegd, MAX_x, pinList, nPins, Name) \
{ \
    byte j;\
    errReg = 0; \
    if (xNRegd < MAX_x) {\
        for(j=0; j<nPins; j++) if(isPinRegistered(pinList[j])) { \
          errReg = 2; \
          break; \
        }\
        if(j == nPins) {\
            x[xNRegd].attach(pinList, Name); \
            for(j=0; j<nPins; j++) registerPin(pinList[j]); \
            xNRegd++; \
        }\
    } else {\
      errReg = 1; \
    } \
}

#define Munreg(x, xNRegd) \
{ \
    for (byte j=0; j<xNRegd; j++) { \
        x[xNRegd].detach(); \
    } \
    xNRegd = 0; \
}
/*
// custom section following: e.g.
if(!errReg) {
  if(xNRegd == 1) {
      MFButtonT::attachHandler(btnOnRelease, handlerOnRelease);
      MFButtonT::attachHandler(btnOnPress, handlerOnRelease);
  }
}
// end custom section
*/

// Object POINTER array version:
// (see http://forum.arduino.cc/index.php?topic=376860.msg2598440#msg2598440)

#define MregisterPtr(x, xNRegd, MAX_x, pinList, nPins, Name) \
{ \
    byte j;\
    errReg = 0; \
    if (xNRegd < MAX_x) {\
        for(j=0; j<nPins; j++) if(isPinRegistered(pinList[j])) { \
          errReg = 2; \
          break; \
        }\
        if(j == nPins) {\
            x[xNRegd] = new typeof(*x[0]); \
            x[xNRegd]->attach(pinList, Name); \
            for(j=0; j<nPins; j++) registerPin(pinList[j]); \
            xNRegd++; \
        }\
    } else {\
      errReg = 1; \
    } \
}

#define MunregPtr(x, xNRegd) \
{ \
    for (byte j=0; j<xNRegd; j++) { \
        x[xNRegd]->detach(); \
        delete x[xNRegd]; x[xNRegd] = NULL;\
    } \
    xNRegd = 0; \
}

// IOBlock (pointer) version:
// (see http://forum.arduino.cc/index.php?topic=376860.msg2598440#msg2598440)

#define MregisterIOB(x, pinList, nPins, base, Name) \
{ \
    byte j;\
    errReg = 0; \
    if (IOBlocksRegistered<MAX_IOBLOCKS) {\
        for(j=0; j<nPins; j++) if(isPinRegistered(pinList[j])) { \
          errReg = 2; \
          break; \
        }\
        if(j == nPins) {\
            x *obj = new x; \
            IOBlocks[IOBlocksRegistered] = (MFPeripheral *)obj; \
            IOBlocks[IOBlocksRegistered]->attach(pinList, Name); \
            for(j=0; j<nPins; j++) registerPin(pinList[j]); \
            IOBlocksRegistered++; \
            registerVirtualPins((MFPeripheral *)obj, base);\
        }\
    } else {\
      errReg = 1; \
    } \
}

#define MunregPtr(x, xNRegd) \
{ \
    for (byte j=0; j<xNRegd; j++) { \
        x[xNRegd]->detach(); \
        delete x[xNRegd]; x[xNRegd] = NULL;\
    } \
    xNRegd = 0; \
}

// Error msg management:

//TODO Check if error msgs can be collated with one of these macros
// called like: Merrors(Device) or: Merrors("Device")
// (both compile, but how do they behave?)

#define Merrors(x) \
  { \
    char *cp; \
    if(errReg==1) cp=PSTR("##x## limit exceeded"); \
    if(errReg==2) cp=PSTR("##x## pin conflict"); \
    if(errReg==3) cp=PSTR("##x## pin(s) not onboard"); \
    if(!errReg)   cp=PSTR("Added ##x##"); \
    cmdMessenger.sendCmd(kStatus,cp); \
  }

/*
#define Merrors(x) \
  { \
    if(errReg==1) cmdMessenger.sendCmd(kStatus,PSTR("##x## limit exceeded")); \
    if(errReg==2) cmdMessenger.sendCmd(kStatus,PSTR("##x## pin conflict")); \
    if(errReg==3) cmdMessenger.sendCmd(kStatus,PSTR("##x## pin(s) not onboard")); \
    if(!errReg)   cmdMessenger.sendCmd(kStatus,PSTR("Added ##x##")); \
  }
*/
///************************************************************************************************

///========///
/// OUTPUT ///
///========///

void AddOutput(uint8_t pin) //, String name)
{
  byte errReg = 0;
  if (outputsRegistered == MAX_OUTPUTS) errReg=1;
  if (isPinRegistered(pin)) errReg=2;

  if(!errReg) {
    //outputs[outputsRegistered] = MFOutput(pin);
    outputs[outputsRegistered].attach(pin);
    registerPin(pin); //, kTypeOutput);
    outputsRegistered++;
  }

#ifdef DEBUG
  char *cp;
  if(errReg==1) cp=PSTR("Output limit exceeded");
  if(errReg==2) cp=PSTR("Output pin conflict");
  if(!errReg)   cp=PSTR("Added Output");
  cmdMessenger.sendCmd(kStatus,cp);
  //cmdMessenger.sendCmd(kStatus, PSTR("Added output.")); // " + name);
#endif
}

void ClearOutputs()
{
  clearRegisteredPins(kTypeOutput);
  outputsRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared outputs"));
#endif
}

///=========///
/// BUTTONS ///
///=========///
void AddButton(uint8_t pin) //, String name)
{
  byte errReg = 0;
  if (buttonsRegistered == MAX_BUTTONS) errReg=1;
  if (isPinRegistered(pin))             errReg=2;

  if(!errReg) {
    //buttons[buttonsRegistered] = MFButtonT(pin, name);
    buttons[buttonsRegistered].attach(pin);
    if(buttonsRegistered == 0) {
        MFButtonT::attachHandler(handlerOnButton);
    }
    registerPin(pin); //, kTypeButton);
    buttonsRegistered++;
  }
#ifdef DEBUG
  char *cp;
  if(errReg==1) cp=PSTR("Button limit exceeded");
  if(errReg==2) cp=PSTR("Button pin conflict");
  if(!errReg)   cp=PSTR("Added Button");
  cmdMessenger.sendCmd(kStatus,cp);
  //cmdMessenger.sendCmd(kStatus, PSTR("Added output.")); // " + name);
#endif
}

void ClearButtons()
{
  clearRegisteredPins(kTypeButton);
  buttonsRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared buttons"));
#endif
}

///==========///
/// ENCODERS ///
///==========///
void AddEncoder(byte pin1, byte pin2, char *Name)
{
  byte errReg = 0;
  if (!isOnboard(pin1) || !isOnboard(pin2)) errReg=3;

  if(!errReg) {
    byte  argList[3];
    argList[0] = pin1;
    argList[1] = pin2;
    argList[2] = encodersRegistered+1;
    MregisterPtr(encoders, encodersRegistered, MAX_ENCODERS, argList, 2, Name); //Name will be ignored anyway
  #ifdef DEBUG
      if(errReg==1) cmdMessenger.sendCmd(kStatus,PSTR("Encoder limit exceeded"));
      if(errReg==2) cmdMessenger.sendCmd(kStatus,PSTR("Encoder pin conflict"));
  #endif
    if(!errReg && encodersRegistered==1) {
        MFEncoder::attachHandler(handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encLeft, handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encLeftFast, handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encRight, handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encRightFast, handlerOnEncoder);
    }
  }
#ifdef DEBUG
  char *cp;
  if(errReg==1) cp=PSTR("Encoder limit exceeded");
  if(errReg==2) cp=PSTR("Encoder pin conflict");
  if(errReg==3) cp=PSTR("Encoder pin(s) not onboard");
  if(!errReg)   cp=PSTR("Added Encoder");
  cmdMessenger.sendCmd(kStatus,cp);
#endif
}

void ClearEncoders()
{
  clearRegisteredPins(kTypeEncoder);
  MunregPtr(encoders, encodersRegistered);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared encoders"));
#endif
}

///==========///
/// SEGMENTS ///
///==========///
void AddLedSegment(byte dataPin, byte csPin, byte clkPin, byte numDevices, byte brightness)
{
  byte errReg = 0;
  if (!isOnboard(dataPin) || !isOnboard(csPin) || !isOnboard(clkPin)) errReg=3;

  if(!errReg) {
    byte argList[5];
    argList[0] = dataPin;
    argList[1] = csPin;
    argList[2] = clkPin;
    argList[3] = numDevices;
    argList[4] = brightness;
    MregisterPtr(ledSegments, ledSegmentsRegistered, MAX_LEDSEGMENTS, argList, 5, NULL); //Name will be ignored anyway
  }
#ifdef DEBUG
  char *cp;
  if(errReg==1) cp=PSTR("Led Segment limit exceeded");
  if(errReg==2) cp=PSTR("Led Segment pin conflict");
  if(errReg==3) cp=PSTR("Led Segment pin(s) not onboard");
  if(!errReg)   cp=PSTR("Added Led Segment");
  cmdMessenger.sendCmd(kStatus,cp);
#endif
}

void ClearLedSegments()
{
  clearRegisteredPins(kTypeLedSegment);
  MunregPtr(ledSegments, ledSegmentsRegistered);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, PSTR("Cleared segments"));
#endif
}

///=========///
/// STEPPER ///
///=========///
void AddStepper(byte pin1, byte pin2, byte pin3, byte pin4, byte btnPin1)
{
  byte errReg = 0;
  if (!isOnboard(pin1) || !isOnboard(pin2) || !isOnboard(pin3) || !isOnboard(pin4)) errReg=3;

  if(!errReg) {
    byte argList[5];
    argList[0] = pin1;
    argList[1] = pin2;
    argList[2] = pin3;
    argList[3] = pin4;
    argList[4] = 0; //btnPin1;  // autoreset is not released yet
    MregisterPtr(steppers, steppersRegistered, MAX_STEPPERS, argList, 4, NULL);
    if(!errReg) {
      steppers[steppersRegistered-1]->setMaxSpeed(STEPPER_SPEED);
      steppers[steppersRegistered-1]->setAcceleration(STEPPER_ACCEL);
    }
  }
#ifdef DEBUG
  char *cp;
  if(errReg==1) cp=PSTR("Stepper limit exceeded");
  if(errReg==2) cp=PSTR("Stepper pin conflict");
  if(errReg==3) cp=PSTR("Stepper pin(s) not onboard");
  if(!errReg)   cp=PSTR("Added Stepper");
  cmdMessenger.sendCmd(kStatus,cp);
#endif
}

void ClearSteppers()
{
  clearRegisteredPins(kTypeStepper);
  MunregPtr(steppers, steppersRegistered);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared steppers"));
#endif
}

///========///
/// SERVOS ///
///========///
void AddServo(byte pin)
{
  byte errReg = 0;
  if(servosRegistered == MAX_MFSERVOS) errReg==1;
  if(isPinRegistered(pin))             errReg==2;
  if(!isOnboard(pin))                  errReg==3;
  if(!errReg) {
    servos[servosRegistered] = new MFServo();
    servos[servosRegistered]->attach(pin, true);
    registerPin(pin);  //, kTypeServo);
    servosRegistered++;
  }
#ifdef DEBUG
  if(errReg==1) cmdMessenger.sendCmd(kStatus,PSTR("Servo limit exceeded"));
  if(errReg==2) cmdMessenger.sendCmd(kStatus,PSTR("Servo pin conflict"));
  if(errReg==3) cmdMessenger.sendCmd(kStatus,PSTR("Servo pin(s) not onboard"));
  if(!errReg) cmdMessenger.sendCmd(kStatus,PSTR("Added Servo"));
#endif
}

void ClearServos()
{
  for (int i=0; i<servosRegistered; i++)
  {
    servos[i]->detach();
    delete servos[i]; servos[i] = NULL;
  }
  clearRegisteredPins(kTypeServo);
  servosRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared servos"));
#endif
}

///=============///
/// LCD Display ///
///=============///
void AddLcdDisplay (uint8_t address, uint8_t cols, uint8_t lines, char *name)
{
  if (lcd_12cRegistered == MAX_MFLCD_I2C) return;
  lcd_I2C[lcd_12cRegistered].attach(address, cols, lines);
  lcd_12cRegistered++;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Added lcdDisplay"));
#endif
}

void ClearLcdDisplays()
{
  for (int i=0; i!=lcd_12cRegistered; i++) {
    lcd_I2C[lcd_12cRegistered].detach();
  }
  lcd_12cRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared lcdDisplays"));
#endif
}


///************************************************************************************************

///===================///
/// IOBlock: Input165 ///
///===================///
void AddInput165(byte dataPin, byte csPin, byte clkPin, byte numDevices=1)
{
  byte errReg = 0;
  if (!isOnboard(dataPin) || !isOnboard(csPin) || !isOnboard(clkPin)) errReg=3;

  if(!errReg) {
    byte argList[4];
    argList[0] = dataPin;
    argList[1] = csPin;
    argList[2] = clkPin;
    argList[3] = numDevices;

    MregisterIOBPtr(MFInput165, argList, 4, NULL); //Name will be ignored anyway
  }
{
  byte errReg = 0;
  if (buttonsRegistered == MAX_BUTTONS) errReg=1;
  if (isPinRegistered(pin))             errReg=2;

  if(!errReg) {
    //buttons[buttonsRegistered] = MFButtonT(pin, name);
    buttons[buttonsRegistered].attach(pin);
    if(buttonsRegistered == 0) {
        MFButtonT::attachHandler(handlerOnButton);
    }
    registerPin(pin); //, kTypeButton);
    buttonsRegistered++;
  }
#ifdef DEBUG
  char *cp;
  if(errReg==1) cp=PSTR("Button limit exceeded");
  if(errReg==2) cp=PSTR("Button pin conflict");
  if(!errReg)   cp=PSTR("Added Button");
  cmdMessenger.sendCmd(kStatus,cp);
  //cmdMessenger.sendCmd(kStatus, PSTR("Added output.")); // " + name);
#endif
}

///=============///
/// XXX ///
///=============///
/*
void AddXXX (int a, int b, int c, String name)
{
    char    *pinList[?];    // define (common) at upper level
    byte    nPins;          // define (common) at upper level

    npins = 0;
    pinList[nPins++] = a;
    pinList[nPins++] = b;
    pinList[nPins++] = c;
    Mregister(XXX, XXXNRegd, MAX_XXX, pinList, nPins, name.toCharArray());
    // .... custom ops ....
#ifdef DEBUG
    cmdMessenger.sendCmd(kStatus,"Added xxx");
#endif
}

void ClearXXXs()
{
    Munreg(XXX, XXXNRegd);
#ifdef DEBUG
    cmdMessenger.sendCmd(kStatus,"Cleared XXXs");
#endif
}

///-------------------------------------------------------------------------------------------------

XXX are:
kTypeInputMtx
kTypeInput165
kTypeOutput595
kTypeOutLEDDM13
kTypeOutLED5940
kTypeInOutMCPS
kTypeInOutMCP0
*/
