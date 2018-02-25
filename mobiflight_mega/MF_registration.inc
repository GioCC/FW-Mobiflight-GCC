/// ===================
/// MF_registration.inc
///
/// macro inclusion file for mobiflight_mega_ino

inline
byte isPinRegdAsIn(byte pin)
{
    return (pin < MAX_LINES ? pinsRegIn.get(pin) : true);
}
inline
byte isPinRegdAsOut(byte pin)
{
    return (pin < MAX_LINES ? pinsRegOut.get(pin) : true);
}
inline
byte isPinSharable(byte pin)
{
    return (pin < NUM_ONB_PINS ? pinsRegSharable.get(pin) : false);
}

inline
byte isPinRegistered(byte pin)
{
    return (pin < MAX_LINES ? (isPinRegdAsIn(pin)||isPinRegdAsOut(pin)) : true);
}

//bool isPinRegisteredForType(byte pin, byte type) {
//  return pinsRegistered[pin] == type;
//}

//void registerPin(byte pin)
//    { if(pin < MAX_LINES) pinsRegistered.set(pin); }

#define checkPinReg(p, i, s)  registerPin(p, i, s, 1)

// Register (if allowed) or check registrability of a pin with supplied features
// Returnes 1 of OK, 0 if not allowed / not registered
byte registerPin(byte pin, byte isInput, byte sharable, byte checkonly)
{
  // Rules:
  // Pin number must be within max range
  // Can't declare a pin sharable if that pin no. isn't sharable
  // Can't assign a pin if it is already assigned with the opposite direction
  // If already assigned with the SAME direction, can assign it if both the new one
  // and the already assigned one(s) is (are) sharable
  //
  // Note: isPinSharable()=true indicates that that pin no. supports sharing (ie: onboard pins),
  // while pinsRegSharable()=true indicates that another client has claimed that pin, but it can be shared.
  if(pin >= MAX_LINES) return 0;
  if(sharable && !isPinSharable(pin)) return 0;
  if(isInput) {
    if(pinsRegOut.get(pin)) return 0;
    if(pinsRegIn.get(pin)&&(!sharable || !pinsRegSharable.get(pin))) return 0;
    if(!checkonly) pinsRegIn.set(pin);
  } else {
    if(pinsRegIn.get(pin)) return 0;
    if(pinsRegOut.get(pin)&&(!sharable || !pinsRegSharable.get(pin))) return 0;
    if(!checkonly) pinsRegOut.set(pin);
  }
  if(!checkonly && sharable) pinsRegSharable.set(pin);
  return 1;
}

/// Completely release pin allocation
// Warning: takes care of registration only.
// It is supposed to be preceded (or followed) by other functions which
// perform a status reset in case of physical pins.
void clearRegisteredPins(void)
{
    //pinsRegistered.clr();
    pinsRegIn.clr();
    pinsRegOut.clr();
    pinsRegSharable.clr();
}


/// Release pin allocation by peripheral type
// Warning: takes care of registration only.
// It is supposed to be preceded (or followed) by other functions which
// perform a status reset in case of physical pins.
void clearRegisteredPins(byte type)
{
  // WARNING
  // This function is guaranteed to work fully as intended only for these types,
  // kTypeButton, kTypeOutput, kTypeEncoder, kTypeServo, kTypeStepper, kTypeInputMtx
  //
  // For other types, clearing the flags the way we do it here is not entirely correct
  // (because of sharable pins; the above types are "safe" because they allow no pin sharing).
  // If a pin is marked as sharable (i.e. it is possibly shared actually) it would require
  // a rather expensive search to locate other possible co-holders.
  // Since sharing is not very frequent, and also it is likely to occur between clients
  // of the same group (which we are all erasing), we can trust this function to be used
  // under proper discipline.
  // If you want to be absolutely safe, just refrain from using this selective version
  // (which is not really necessary anyway) and only use the general clearRegisteredPins(void)

  byte p[5];
  byte ne = 0;
  byte np;
  //MFPeripheral *mfp = NULL;
  MFPeripheral **mfpp = NULL;

  switch(type) {
    case kTypeButton:
        for(byte i=0; i<buttonsRegistered; i++) pinsRegIn.clr(buttons[i].getPin());
        break;
    case kTypeOutput:
        for(byte i=0; i<outputsRegistered; i++) pinsRegOut.clr(outputs[i].getPin());
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
        mfpp = IOBlocks;
        //unregIOBlocks();  //TODO check if correct
    break;
    default:
    break;
  }
  for(byte i=0; i<ne; i++) {
      np = 0;
      //if(mfp) {
      //    np = mfp[i].pinCount();
      //    mfp[i].getPins(p);
      //}
      if(mfpp) {
          np = mfpp[i]->pinCount();
          mfpp[i]->getPins(p);
      }
      for(byte k=0; k<np; k++) {
        // Don't know which one is set, clears both anyway
        pinsRegOut.clr(p[k]);
        pinsRegIn.clr(p[k]);
        // Also clean sharable flag
        pinsRegSharable.clr(p[k]);
      }
  }
}

byte checkVirtualPins(byte base, byte nBanks)
{
    byte i = 64+(base*8);   // Virtual inputs start from #64
    for(byte b=0; b<nBanks; b++) {
        for(byte msk=0x01; msk; msk<<=1, i++) {
            if(isPinRegistered(i)) return kErrConflict;
        }
    }
    return kErrNone;
}

/// Register a set of virtual pins belonging to an object
// Also generates Buttons and Outputs associated to virtual pins
byte registerVirtualPins(MFIOBlock *obj, byte checkOnly=false)
{
    byte i = 64+(obj->getBase()*8);   // Virtual inputs start from #64
    for(byte b=0; b<obj->getSize(); b++) {
        for(byte msk=0x01; msk; msk<<=1, i++) {
            if(isPinRegistered(i)) return kErrConflict;
            if(!checkOnly) {
                if(obj->getInputMap(b) & msk)  AddButton(i);
                if(obj->getOutputMap(b) & msk) AddOutput(i);
            }
        }
    }
    return kErrNone;
}

// Virtual pins are unregistered through ClearOutputs() and ClearButtons()
/*
byte unregisterVirtualPins(MFIOBlock *obj)
{
    byte i = 64+(obj->getBase()*8);   // Virtual inputs start from #64
    for(byte b=0; b<obj->getSize(); b++) {
        for(byte msk=0x01; msk; msk<<=1, i++) {
            pinsRegistered.clr(pin);
        }
    }
  return kErrNone;
}
*/

/// Register peripheral
// Registers peripheral object, NOT driver pins.
// Used for peripherals which are NOT IOBlocks (which have their own procedure).
// Each peripheral type has its own register array.
//see http://forum.arduino.cc/index.php?topic=376860.msg2598440#msg2598440
template<class T>
byte registerPeripheral(T *vec[], byte &nElem, byte nMax, byte *argList, byte nPins, char *Name)
{
    //byte j;
    byte errReg = 0;
    if (nElem < nMax) {
        //Can't check pins in bulk, should know whether I or O
        //for(j=0; j<nPins && !errReg; j++) { if(isPinRegistered(argList[j])) { errReg = 2; }}
        if(!errReg) {
            T *objp = new T;
            vec[nElem] = objp;
            // Static cast here is not required as much to upcast vec[n]
            // as to identify its type, which the compiler has no way of knowing
            // (so it can't detect whether it has an attach() method).
            (static_cast<MFPeripheral *>(objp))->attach(argList, Name);
            //Can't register pins in bulk, should know whether I or O
            //for(j=0; j<nPins; j++) registerPin(argList[j]);
            nElem++;
        }
    } else {
      errReg = 1;
    }
    return errReg;
}

/// Unregister peripheral
// The peripheral obj is deallocated.
// Driver pins are (expected to be) reset HW-wise by detach(), however
// their reservations are NOT released here.
// Driver pins must be previously released through ClearXXXX() functions.
template<class T>
void unregPeripherals(T *vec[], byte &n)
{
    for (byte j=0; j<n; j++) {
        vec[j]->detach();
        delete vec[j]; vec[j] = NULL;
    }
    n = 0;
}

/// Register IOBlock
// Registers IOBlock objects;
// also registers the associated virtual pins and generates Button and Output objects for them.
// Does NOT register driver pins.
template<class T>
byte registerIOB(byte *argList, byte nPins, byte base, byte nBlocks, char *Name)
{
    //byte j;
    byte errReg = 0;
    if (IOBlocksRegistered<MAX_IOBLOCKS) {
        //Can't check pins in bulk, should know whether I or O
        //for(j=0; j<nPins && !errReg; j++) { if(isPinRegistered(argList[j])) { errReg = 2; }}
        if(!errReg) {
          errReg = checkVirtualPins(base, nBlocks);
        }
        if(!errReg) {
            T *obj = new T;
            IOBlocks[IOBlocksRegistered] = (MFPeripheral *)obj;
            IOBlocks[IOBlocksRegistered]->attach(argList, Name);
            obj->bind(&IOStatusVal, &IOStatusNew, base);
            //Can't register onboard pins in bulk, should know whether I or O
            //for(j=0; j<nPins; j++) { registerPin(argList[j]); }
            IOBlocksRegistered++;
            //Can register virtual pins in bulk, now obj carries the info about direction
            registerVirtualPins((MFIOBlock *)obj);
        }
    } else {
      errReg = 1;
    }
    return errReg;
}


/// Unregister IOBlock
// When unregistering an IOBlock, the IOBlock obj is deallocated and
// driver pins are (expected to be) reset HW-wise by detach().
// NO pin reservations (either drivers or virtual IOs) are released here!
// Driver pins must be previously released through ClearIOBlocks().
// Virtual pins are assigned to Outputs and Buttons, therefore they must be
// released (even later) through ClearOutputs() and ClearButtons().

void unregIOBlocks(void)
{
    MFPeripheral *iobp;
    for (byte j=0; j<IOBlocksRegistered; j++) {
        iobp = IOBlocks[j];
        iobp->detach();
        delete iobp; IOBlocks[j] = NULL;
    }
    IOBlocksRegistered = 0;
}


// BEWARE:
// arglist contains <nPins> interface pin at the start, then (possibly) more arguments
// <nPins>  is the number of PINS, not of arguments - the called .attach() method is supposed
// to be aware of the required arg count!

// Error msg management:

//TODO Check if error msgs can be collated with one of these macros
// called like: Merrors(Device) or: Merrors("Device")
// (both compile, but how do they behave?)
#define Merrors(x) \
  { \
    const char *cp; \
    if(errReg==kErrFull)        cp=PSTR("IOBlock (##x##) limit exceeded"); \
    if(errReg==kErrConflict)    cp=PSTR("IOBlock (##x##) pin conflict"); \
    if(errReg==kErrNotOnboard)  cp=PSTR("IOBlock (##x##) pin(s) not onboard"); \
    if(!errReg)   cp=PSTR("Added IOBlock (##x##)"); \
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
  byte errReg = kErrNone;
  if (outputsRegistered == MAX_OUTPUTS) errReg=kErrFull;
  // Direct I/O pins (whether onboard or virtual) can't share assignment with anything else
  if (isPinRegistered(pin))             errReg=kErrConflict;

  if(!errReg) {
    //outputs[outputsRegistered] = MFOutput(pin);
    outputs[outputsRegistered].attach(pin);
    registerPin(pin, 0, 0);
    outputsRegistered++;
  }

#ifdef DEBUG
  const char *cp;
  if(errReg==kErrFull)      cp=PSTR("Output limit exceeded");
  if(errReg==kErrConflict)  cp=PSTR("Output pin conflict");
  if(!errReg)   cp=PSTR("Added Output");
  cmdMessenger.sendCmd(kStatus,cp);
  //cmdMessenger.sendCmd(kStatus, PSTR("Added output.")); // " + name);
#endif
}

void ClearOutputs(void)
{
  //Better to reset pin status to INPUT_PULLUP for onboard pins
  byte p;
  for(byte i=0; i<outputsRegistered; i++) {
    if(isOnboard(p = outputs[i].getPin())) pinMode(p, INPUT_PULLUP);
  }
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
  byte errReg = kErrNone;
  if (buttonsRegistered == MAX_BUTTONS) errReg=kErrFull;
  // Direct I/O pins (whether onboard or virtual) can't share assignment with anything else
  if (isPinRegistered(pin))             errReg=kErrConflict;

  if(!errReg) {
    //buttons[buttonsRegistered] = MFButtonT(pin, name);
    buttons[buttonsRegistered].attach(pin);
    if(buttonsRegistered == 0) {
        MFButtonT::attachHandler(handlerOnButton);
    }
    registerPin(pin, 1, 0);
    buttonsRegistered++;
  }
#ifdef DEBUG
  const char *cp;
  if(errReg==kErrFull)      cp=PSTR("Button limit exceeded");
  if(errReg==kErrConflict)  cp=PSTR("Button pin conflict");
  if(!errReg)               cp=PSTR("Added Button");
  cmdMessenger.sendCmd(kStatus,cp);
  //cmdMessenger.sendCmd(kStatus, PSTR("Added output.")); // " + name);
#endif
}

void ClearButtons(void)
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
  byte errReg = kErrNone;
  // Pin validation
  if (!isOnboard(pin1)
   || !isOnboard(pin2))      errReg=kErrNotOnboard;
  if (isPinRegistered(pin1)
   || isPinRegistered(pin2)) errReg=kErrConflict;

  if(!errReg) {
    byte  argList[3];
    argList[0] = pin1;
    argList[1] = pin2;
    argList[2] = encodersRegistered+1;
    errReg = registerPeripheral<MFEncoder>(encoders, encodersRegistered, MAX_ENCODERS, argList, 2, Name);

  #ifdef DEBUG
      if(errReg==kErrFull)      cmdMessenger.sendCmd(kStatus,PSTR("Encoder limit exceeded"));
      if(errReg==kErrConflict)  cmdMessenger.sendCmd(kStatus,PSTR("Encoder pin conflict"));
  #endif
    if(!errReg) {
        registerPin(pin1, 1, false);
        registerPin(pin2, 1, false);
        if(encodersRegistered==1) MFEncoder::attachHandler(handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encLeft, handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encLeftFast, handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encRight, handlerOnEncoder);
        //encoders[encodersRegistered].attachHandler(encRightFast, handlerOnEncoder);
    }
  }
#ifdef DEBUG
  const char *cp;
  if(errReg==kErrFull)          cp=PSTR("Encoder limit exceeded");
  if(errReg==kErrConflict)      cp=PSTR("Encoder pin conflict");
  if(errReg==kErrNotOnboard)    cp=PSTR("Encoder pin(s) not onboard");
  if(!errReg)   cp=PSTR("Added Encoder");
  cmdMessenger.sendCmd(kStatus,cp);
#endif
}

void ClearEncoders()
{
  clearRegisteredPins(kTypeEncoder);
  unregPeripherals(encoders, encodersRegistered);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared encoders"));
#endif
}

///==========///
/// SEGMENTS ///
///==========///
void AddLedSegment(byte dataPin, byte csPin, byte clkPin, byte numDevices, byte brightness)
{
  byte errReg = kErrNone;
  // Pin validation
  if(!isOnboard(dataPin) ||
     !isOnboard(csPin)   ||
     !isOnboard(clkPin)) errReg=kErrNotOnboard;
  if(!checkPinReg(dataPin, 0, true) ||
     !checkPinReg(clkPin,  0, true) ||
     !checkPinReg(csPin,   0, false))  errReg=kErrConflict;

  if(!errReg) {
    byte argList[5];
    argList[0] = dataPin;
    argList[1] = csPin;
    argList[2] = clkPin;
    argList[3] = numDevices;
    argList[4] = brightness;
    errReg = registerPeripheral<MFSegments>(ledSegments, ledSegmentsRegistered, MAX_LEDSEGMENTS, argList, 3, NULL); //Name will be ignored anyway
  }
  if(!errReg) {
    registerPin(dataPin, 0, true);
    registerPin(clkPin,  0, true);
    registerPin(csPin,   0, false); // this can't be shared
  }

#ifdef DEBUG
  const char *cp;
  if(errReg==kErrFull)       cp=PSTR("Led Segment limit exceeded");
  if(errReg==kErrConflict)   cp=PSTR("Led Segment pin conflict");
  if(errReg==kErrNotOnboard) cp=PSTR("Led Segment pin(s) not onboard");
  if(!errReg)   cp=PSTR("Added Led Segment");
  cmdMessenger.sendCmd(kStatus,cp);
#endif
}

void ClearLedSegments()
{
  clearRegisteredPins(kTypeLedSegment);
  unregPeripherals(ledSegments, ledSegmentsRegistered);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, PSTR("Cleared segments"));
#endif
}

///=========///
/// STEPPER ///
///=========///
void AddStepper(byte pin1, byte pin2, byte pin3, byte pin4, byte btnPin1)
{
  byte errReg = kErrNone;
  // Pin validation
  if (!isOnboard(pin1) ||
      !isOnboard(pin2) ||
      !isOnboard(pin3) ||
      !isOnboard(pin4)) errReg=kErrNotOnboard;
  if(!checkPinReg(pin1, 0, false) ||
     !checkPinReg(pin2, 0, false) ||
     !checkPinReg(pin3, 0, false) ||
     !checkPinReg(pin4, 0, false))  errReg=kErrConflict;


  if(!errReg) {
    byte argList[5];
    argList[0] = pin1;
    argList[1] = pin2;
    argList[2] = pin3;
    argList[3] = pin4;
    argList[4] = 0; //btnPin1;  // autoreset is not released yet
    errReg = registerPeripheral<MFStepper>(steppers, steppersRegistered, MAX_STEPPERS, argList, 4, NULL);
  }
  if(!errReg) {
    registerPin(pin1,0,false);
    registerPin(pin2,0,false);
    registerPin(pin3,0,false);
    registerPin(pin4,0,false);
    steppers[steppersRegistered-1]->setMaxSpeed(STEPPER_SPEED);
    steppers[steppersRegistered-1]->setAcceleration(STEPPER_ACCEL);
  }
#ifdef DEBUG
  const char *cp;
  if(errReg==kErrFull)       cp=PSTR("Stepper limit exceeded");
  if(errReg==kErrConflict)   cp=PSTR("Stepper pin conflict");
  if(errReg==kErrNotOnboard) cp=PSTR("Stepper pin(s) not onboard");
  if(!errReg)   cp=PSTR("Added Stepper");
  cmdMessenger.sendCmd(kStatus,cp);
#endif
}

void ClearSteppers()
{
  clearRegisteredPins(kTypeStepper);
  unregPeripherals(steppers, steppersRegistered);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared steppers"));
#endif
}

///========///
/// SERVOS ///
///========///
void AddServo(byte pin)
{
  byte errReg = kErrNone;
  if(servosRegistered == MAX_MFSERVOS) errReg=kErrFull;
  if(isPinRegistered(pin))             errReg=kErrConflict;
  if(!isOnboard(pin))                  errReg=kErrNotOnboard;
  if(!errReg) {
    servos[servosRegistered] = new MFServo();
    servos[servosRegistered]->attach(pin, true);
    registerPin(pin, 0, false);
    servosRegistered++;
  }
#ifdef DEBUG
  if(errReg==kErrFull)       cmdMessenger.sendCmd(kStatus,PSTR("Servo limit exceeded"));
  if(errReg==kErrConflict)   cmdMessenger.sendCmd(kStatus,PSTR("Servo pin conflict"));
  if(errReg==kErrNotOnboard) cmdMessenger.sendCmd(kStatus,PSTR("Servo pin(s) not onboard"));
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
  byte errReg = kErrNone;
  if (lcd_12cRegistered == MAX_MFLCD_I2C) return;
  // Pin validation (using fixed HW I2C)
  if(!checkPinReg(SDA, 0, true) ||
     !checkPinReg(SCL, 0, true))  errReg=kErrConflict;

  lcd_I2C[lcd_12cRegistered].attach(address, cols, lines);
  lcd_12cRegistered++;
  // Register std I2C pins to prevent accidental hijacking
  // I2C pins are to be registered as INPUTS by convention
  // (to avoid that they are forced to an unwanted state)
  registerPin(SDA, 1, true);
  registerPin(SCL, 1, true);
#ifdef DEBUG
  if(errReg==kErrConflict)   cmdMessenger.sendCmd(kStatus,PSTR("LCD pin conflict"));
  if(!errReg) cmdMessenger.sendCmd(kStatus,PSTR("Added LCD Display"));
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
///  IOBlocks
///************************************************************************************************

// IOBlock only have a common ClearXXX() function!
void ClearIOBlocks(void)
{
  clearRegisteredPins(kTypeInput165);   // any IOBlock type will do here
  unregIOBlocks();
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Cleared all IOBlocks"));
#endif
}


///===================///
/// IOBlock: Input165 ///
///===================///
void AddInput165(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices=1)
{
  byte errReg = kErrNone;
  // Pin validation
  if(!isOnboard(dataPin) ||
     !isOnboard(csPin)   ||
     !isOnboard(clkPin))             errReg=kErrNotOnboard;
  if(!checkPinReg(dataPin,1, true) ||
     !checkPinReg(clkPin, 0, true) ||
     !checkPinReg(csPin,  0, false)) errReg=kErrConflict;

  if(!errReg) {
    byte argList[5];        // This is the same as expected by specific MFmodule.attach()
    argList[0] = dataPin;
    argList[1] = csPin;
    argList[2] = clkPin;
    argList[3] = base;
    argList[4] = numDevices;
    //MregisterIOB(MFInput165, argList, 3, base, numDevices, NULL); //Name ignored
    errReg = registerIOB<MFInput165>(argList, 3, base, numDevices, NULL);
  }
  if(!errReg) {
    registerPin(dataPin,1, true);
    registerPin(clkPin, 0, true);
    registerPin(csPin,  0, false);
  }
  Merrors(Input165);
}

///====================///
/// IOBlock: Output595 ///
///====================///
void AddOutput595(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices=1)
{
  byte errReg = kErrNone;
  // Pin validation
  if(!isOnboard(dataPin) ||
     !isOnboard(csPin)   ||
     !isOnboard(clkPin))            errReg=kErrNotOnboard;
  if(!checkPinReg(dataPin,0, true) ||
     !checkPinReg(clkPin, 0, true) ||
     !checkPinReg(csPin,  0, false))  errReg=kErrConflict;

  if(!errReg) {
    byte argList[5];        // This is the same as expected by specific MFmodule.attach()
    argList[0] = dataPin;
    argList[1] = csPin;
    argList[2] = clkPin;
    argList[3] = base;
    argList[4] = numDevices;
    errReg = registerIOB<MFOutput595>(argList, 3, base, numDevices, NULL); //Name ignored
  }
  if(!errReg) {
    registerPin(dataPin,0, true);
    registerPin(clkPin, 0, true);
    registerPin(csPin,  0, false);
  }
  Merrors(Output595);
}

///========================///
/// IOBlock: OutputLEDDM13 ///
///========================///
void AddOutLEDDM13(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices=1)
{
  byte errReg = kErrNone;
  // Pin validation
  if(!isOnboard(dataPin) ||
     !isOnboard(csPin)   ||
     !isOnboard(clkPin))             errReg=kErrNotOnboard;
  if(!checkPinReg(dataPin,0, true) ||
     !checkPinReg(clkPin, 0, true) ||
     !checkPinReg(csPin,  0, false)) errReg=kErrConflict;

  if(!errReg) {
    byte argList[5];        // This is the same as expected by specific MFmodule.attach()
    argList[0] = dataPin;
    argList[1] = csPin;
    argList[2] = clkPin;
    argList[3] = base;
    argList[4] = numDevices;
    errReg = registerIOB<MFOutLEDDM13>(argList, 3, base, numDevices, NULL); //Name ignored
  }
  if(!errReg) {
    registerPin(dataPin,0, true);
    registerPin(clkPin, 0, true);
    registerPin(csPin,  0, false);
  }
  Merrors(OutputLED_DM13);
}

///========================///
/// IOBlock: InOutMCP23017 ///
///========================///
void AddIOMCP0(byte SDAPin, byte SCLPin, byte addr, byte IOdir1, byte IOdir2, byte base)
{
  byte errReg = kErrNone;
  if(SDAPin == SDA || SCLPin == SCL) {
    // If any std HW I2C pins is used, force both to be used
    SDAPin = SDA;
    SCLPin = SCL;
  }

  // Pin validation
  if(!isOnboard(SDAPin) ||
     !isOnboard(SCLPin))            errReg=kErrNotOnboard;
  if(!checkPinReg(SDAPin,1, true) ||
     !checkPinReg(SCLPin,1, true))  errReg=kErrConflict;

  if(!errReg) {
    byte np = 0;
    byte argList[5];        // This is the same as expected by specific MFmodule.attach()
    argList[0] = addr;
    argList[1] = SDAPin;
    argList[2] = SCLPin;
    argList[3] = IOdir1;
    argList[4] = IOdir2;
    errReg = registerIOB<MFIO_MCP0>(argList, np, base, 2, NULL); //Name ignored
  }
  if(SDAPin == SDA || SCLPin == SCL) {
    // I2C pins are to be registered as INPUTS by convention
    // (to avoid that they are forced to an unwanted state)
    registerPin(SDAPin,1,true);
    registerPin(SCLPin,1,true);
  }
  Merrors(IO-MCP0);
}

///========================///
/// IOBlock: InOutMCP23S17 ///
///========================///
void AddIOMCPS(byte MOSIpin, byte MISOpin, byte csPin,  byte clkPin,
               byte addr,    byte IOdir1,  byte IOdir2, byte base)
{
  byte errReg = kErrNone;
  // Pin validation
  // csPin might not be considered when using HW addressing, however we require it anyway for good practice.
  if(!isOnboard(MOSIpin) ||
     !isOnboard(MISOpin)||
     !isOnboard(csPin)     ||
     !isOnboard(clkPin))             errReg=kErrNotOnboard;
  if(!checkPinReg(MOSIpin,  1, true) ||
     !checkPinReg(MISOpin, 0, true) ||
     !checkPinReg(clkPin, 0, true)     ||
     !checkPinReg(csPin,  0, false)) errReg=kErrConflict;

  if(!errReg) {
    byte argList[7];        // This is the same as expected by specific MFmodule.attach()
    argList[0] = addr;
    argList[1] = MOSIpin;
    argList[2] = MISOpin;
    argList[3] = csPin;
    argList[4] = clkPin;
    argList[5] = IOdir1;
    argList[6] = IOdir2;
    errReg = registerIOB<MFIO_MCPS>(argList, 4, base, 2, NULL); //Name ignored
  }
  if(!errReg) {
    registerPin(MOSIpin, 0, true);
    registerPin(MISOpin, 1, true);
    registerPin(csPin,   0, true);
    registerPin(clkPin,  0, true);
  }
  Merrors(IO-MCPS);
}

///===================///
/// IOBlock: InputMtx ///
///===================///
void AddInputMtx(byte Row0, byte NRows, byte Col0, byte NCols, byte base)
{
  byte errReg = kErrNone;
  // Pin validation
  if(!errReg) {
    for(byte c=0; c<NCols && !errReg; c++) {
        if(!isOnboard(c))             errReg = kErrNotOnboard;
        if(!checkPinReg(c, 0, false)) errReg = kErrConflict;
        if(c>=Row0 && c<(Row0+NRows)) errReg = kErrConflict;    // Check overlap between Rs and Cs
    }
    for(byte r=0; r<NRows && !errReg; r++) {
        if(!isOnboard(r))             errReg = kErrNotOnboard;
        if(!checkPinReg(r, 1, false)) errReg = kErrConflict;
        // Overlap already checked in Col's scan
    }
  }
  if(!errReg) {
    byte argList[5];        // This is the same as expected by specific MFmodule.attach()
    argList[0] = Row0;
    argList[1] = Col0;
    argList[2] = NRows;
    argList[3] = NCols;
    argList[4] = base;
    // Driver pin count is 0, they were custom-checked before
    errReg = registerIOB<MFInputMtx>(argList, 0, base, NCols, NULL); //Name ignored
    // Register all driver pins
    if(!errReg) {
        for(byte c=0; c<NCols && !errReg; c++) registerPin(c, 0, false);
        for(byte r=0; r<NRows && !errReg; r++) registerPin(r, 1, false);
    }
  }
  Merrors(InputMtx);
}


///-------------------------------------------------------------------------------------------------
