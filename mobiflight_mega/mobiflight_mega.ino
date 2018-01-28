/**
 * Includes Core Arduino functionality
 **/
char foo;
#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

///NOTE GCC - restore for orig MF
// >>> Remove .h import:
#include "mobiflight.h"
// >>> Restore default args in these headers:
//void AddOutput(uint8_t pin = 1, String name = "Output")
//void AddButton(uint8_t pin = 1, String name = "Button")
//void AddEncoder(uint8_t pin1 = 1, uint8_t pin2 = 2, String name = "Encoder")
//void AddLcdDisplay(uint8_t address = 0x24, uint8_t cols = 16, uint8_t lines = 2, String name = "LCD")
///END NOTE

#define COMPUTE_OBJ_SIZES
//#define DEBUG 0
#define MTYPE_MEGA 1
#define MTYPE_MICRO 2
#define MTYPE_UNO 3
#define MF_STEPPER_SUPPORT 1
#define MF_SERVO_SUPPORT 1
#define MODULETYPE MTYPE_MEGA
//#define MODULETYPE MTYPE_MICRO
//#define MODULETYPE MTYPE_UNO

#if MODULETYPE == MTYPE_MEGA
#define MODULE_MAX_PINS 58
#endif

#if MODULETYPE == MTYPE_UNO
#define MODULE_MAX_PINS 13
#endif

#if MODULETYPE == MTYPE_MICRO
#define MODULE_MAX_PINS 20
#endif

#define STEPS 64
#define STEPPER_SPEED 600 // 300 already worked, 467, too?
#define STEPPER_ACCEL 900

#if MODULETYPE == MTYPE_MICRO
#define MAX_OUTPUTS     10 // ? Find correct value
#define MAX_BUTTONS     10 // ? Find correct value
#define MAX_LEDSEGMENTS 1
#define MAX_ENCODERS    4
#define MAX_STEPPERS    4
#define MAX_MFSERVOS    4
#define MAX_MFLCD_I2C   2
#define MAX_IOBLOCKS    4 // ? Find correct value
// Standard I2C pins
//#define SDA_STD_PIN  2
//#define SCL_STD_PIN  3
#endif

#if MODULETYPE == MTYPE_UNO
#define MAX_OUTPUTS     8 // ? Find correct value
#define MAX_BUTTONS     8 // ? Find correct value
#define MAX_LEDSEGMENTS 1
#define MAX_ENCODERS    2
#define MAX_STEPPERS    2
#define MAX_MFSERVOS    2
#define MAX_MFLCD_I2C   2
#define MAX_IOBLOCKS    4 // ? Find correct value
// Standard I2C pins
//#define SDA_STD_PIN  A4
//#define SCL_STD_PIN  A5

#endif

#if MODULETYPE == MTYPE_MEGA
#define MAX_OUTPUTS     40 //80
#define MAX_BUTTONS     50 //80
#define MAX_LEDSEGMENTS 4
#define MAX_ENCODERS    10 //20
#define MAX_STEPPERS    10
#define MAX_MFSERVOS    10
#define MAX_MFLCD_I2C   2
#define MAX_IOBLOCKS    10 // ? Find correct value
// Standard I2C pins
//#define SDA_STD_PIN  20
//#define SCL_STD_PIN  21
#endif

#define NUM_ONB_PINS    MODULE_MAX_PINS    // beyond this limit, I/Os are no longer onboard pins
#define isOnboard(pin)  ((pin)<MODULE_MAX_PINS)
#define MAX_LINES       128 //256 // Includes actual+virtual pins
#define NUM_VLINES      MAX_LINES-MODULE_MAX_PINS // Includes actual+virtual pins

#include <EEPROMex.h>
#include <bitStore.h>   //GCC
#include <CmdMessenger.h>  // CmdMessenger
#include <TicksPerSecond.h>
#include <Wire.h>

#include <MFButtonT.h>        //#include <MFButton.h>
#include <MFOutput.h>
//#include <Button.h>
//#include <RotaryEncoder.h>
//#include <RotaryEncoderShd.h>
#include <MFEncoder.h>
//#include <LedControl.h>     //  need the library
#include <MFSegments.h>       //  need the library
//#include <Servo.h>
#include <MFServo.h>
//#include <AccelStepper.h>
#include <MFStepper.h>
//#include <LiquidCrystal_I2C.h>
#include <MFLCDDisplay.h>
#include <MFInputMtx.h>
#include <MFInput165.h>
#include <MFOutput595.h>
#include <MFOutLEDDM13.h>
//#include <MFOutLED5940.h>
#include <MFIO_MCPS.h>
#include <MFIO_MCP0.h>

const byte MEM_OFFSET_NAME   = 0;
const byte MEM_LEN_NAME      = 48;
const byte MEM_OFFSET_SERIAL = MEM_OFFSET_NAME + MEM_LEN_NAME;
const byte MEM_LEN_SERIAL    = 11;
const byte MEM_OFFSET_CONFIG = MEM_OFFSET_NAME + MEM_LEN_NAME + MEM_LEN_SERIAL;

// 1.0.1 : Nicer firmware update, more outputs (20)
// 1.1.0 : Encoder support, more outputs (30)
// 1.2.0 : More outputs (40), more inputs (40), more led segments (4), more encoders (20), steppers (10), servos (10)
// 1.3.0 : Generate New Serial
// 1.4.0 : Servo + Stepper support
// 1.4.1 : Reduce velocity
// 1.5.0 : Improve servo behaviour
// 1.6.0 : Set name
// 1.6.1 : Reduce servo noise
// 1.7.0 : New Arduino IDE, new AVR, Uno Support
// 1.7.1 : More UNO stability
// 1.7.2 : "???"
// 1.7.3 : Servo behaviour improved, fixed stepper bug #178, increased number of buttons per module (MEGA)
// 1.8.0 : added support for LCDs
const char version[8] = "1.8.0";

#if MODULETYPE == MTYPE_MEGA
char type[20]                = "MobiFlight Mega";
char serial[MEM_LEN_SERIAL]  = "1234567890";
char name[MEM_LEN_NAME]      = "MobiFlight Mega";
int eepromSize               = EEPROMSizeMega;
const int  MEM_LEN_CONFIG    = 1024;
#endif

#if MODULETYPE == MTYPE_MICRO
char type[20]                = "MobiFlight Micro";
char serial[MEM_LEN_SERIAL]  = "0987654321";
char name[MEM_LEN_NAME]      = "MobiFlight Micro";
int eepromSize               = EEPROMSizeMicro;
const int  MEM_LEN_CONFIG    = 256;
#endif

#if MODULETYPE == MTYPE_UNO
char type[20]                = "MobiFlight Uno";
char serial[MEM_LEN_SERIAL]  = "0987654321";
char name[MEM_LEN_NAME]      = "MobiFlight Uno";
int eepromSize               = EEPROMSizeUno;
const int  MEM_LEN_CONFIG    = 256;
#endif

char configBuffer[MEM_LEN_CONFIG] = "";

int configLength = 0;
boolean configActivated = false;

bool powerSavingMode = false;
//byte pinsRegistered[MODULE_MAX_PINS];
byte pinRegBuf  [roundUp(MAX_LINES)];
bitStore<byte>  pinsRegistered(pinRegBuf,roundUp(MAX_LINES));

const unsigned long POWER_SAVING_TIME = 60*15; // in seconds

CmdMessenger cmdMessenger = CmdMessenger(Serial);
unsigned long lastCommand;

MFOutput outputs[MAX_OUTPUTS];
byte outputsRegistered = 0;

//MFButton buttons[MAX_BUTTONS];
MFButtonT buttons[MAX_BUTTONS];
byte buttonsRegistered = 0;
long lastButtonUpdate  = 0;

MFSegments ledSegments[MAX_LEDSEGMENTS];
byte ledSegmentsRegistered = 0;

MFEncoder encoders[MAX_ENCODERS];
byte encodersRegistered = 0;

MFStepper *steppers[MAX_STEPPERS]; //
byte steppersRegistered = 0;

MFServo servos[MAX_MFSERVOS];
byte servosRegistered = 0;

MFLCDDisplay lcd_I2C[MAX_MFLCD_I2C];
byte lcd_12cRegistered = 0;

MFPeripheral *IOblocks[MAX_IOBLOCKS];
byte IOBlocksRegistered = 0;

byte        inStatusBuf   [roundUp(MAX_BUTTONS)];
byte        inStatusUpdBuf[roundUp(MAX_BUTTONS)];
byte        outStatusBuf  [roundUp(MAX_OUTPUTS)];
bitStore<byte>  InStatus   (inStatusBuf,    roundUp(MAX_BUTTONS));
bitStore<byte>  InStatusUpd(inStatusUpdBuf, roundUp(MAX_BUTTONS));
bitStore<byte>  OutStatus  (outStatusBuf,   roundUp(MAX_OUTPUTS));

#ifdef COMPUTE_OBJ_SIZES
const byte OBJ_SIZE[] = {
  sizeof(MFButtonT),
  sizeof(MFOutput),
  sizeof(MFEncoder),
  sizeof(MFSegments),
  sizeof(MFServo),
  sizeof(MFStepper),
  sizeof(MFLCDDisplay),
  sizeof(MFInputMtx),
  sizeof(MFInput165),
  sizeof(MFOutput595),
  sizeof(MFOutLEDDM13),
  0, //sizeof MFOutLED5940,
  sizeof(MFIO_MCPS),
  sizeof(MFIO_MCP0),
  sizeof(MFPeripheral),
};

//struct {
MFButtonT     SAMPLE_but;
MFOutput      SAMPLE_out;
MFEncoder     SAMPLE_enc;
MFSegments    SAMPLE_seg;
MF_LedControl    SAMPLE_LedCtrl(1,2,3,4);
MFServo       SAMPLE_srv;
MFStepper     SAMPLE_stp;
MFLCDDisplay  SAMPLE_LCD;
MFInputMtx    SAMPLE_inMtx;
MFInput165    SAMPLE_in165;
MFOutput595   SAMPLE_out595;
MFOutLEDDM13  SAMPLE_LEDDM13;
//MFOutLED5940;
MFIO_MCPS     SAMPLE_MCPS;
MFIO_MCP0     SAMPLE_MCP0;
//MFPeripheral  MFPer;
//} OBJ_SAMPLE;
#endif // COMPUTE_OBJ_SIZES

enum
{
  kTypeNotSet,        // 0
  kTypeButton,        // 1
  kTypeEncoder,       // 2
  kTypeOutput,        // 3
  kTypeLedSegment,    // 4
  kTypeStepper,       // 5
  kTypeServo,         // 6
  kTypeLcdDisplayI2C, // 7
  // New IO bank peripherals (GCC 2018-01):
  kTypeInputMtx,      // 8
  kTypeInput165,      // 9
  kTypeOutput595,     // 10
  kTypeOutLEDDM13,    // 11
  kTypeOutLED5940,    // 12
  kTypeInOutMCPS,     // 13
  kTypeInOutMCP0,     // 14
};

// This is the list of recognized commands. These can be commands that can either be sent or received.
// In order to receive, attach a callback function to these events
enum
{
  kInitModule,         // 0
  kSetModule,          // 1
  kSetPin,             // 2
  kSetStepper,         // 3
  kSetServo,           // 4
  kStatus,             // 5, Command to report status
  kEncoderChange,      // 6
  kButtonChange,       // 7
  kStepperChange,      // 8
  kGetInfo,            // 9
  kInfo,               // 10
  kSetConfig,          // 11
  kGetConfig,          // 12
  kResetConfig,        // 13
  kSaveConfig,         // 14
  kConfigSaved,        // 15
  kActivateConfig,     // 16
  kConfigActivated,    // 17
  kSetPowerSavingMode, // 18
  kSetName,            // 19
  kGenNewSerial,       // 20
  kResetStepper,       // 21
  kSetZeroStepper,     // 22
  kTrigger,            // 23
  kResetBoard,         // 24
  kSetLcdDisplayI2C,   // 25
};

// Callbacks define on which received commands we take action
void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kInitModule, OnInitModule);
  cmdMessenger.attach(kSetModule, OnSetModule);
  cmdMessenger.attach(kSetPin, OnSetPin);
  cmdMessenger.attach(kSetStepper, OnSetStepper);
  cmdMessenger.attach(kSetServo, OnSetServo);
  cmdMessenger.attach(kGetInfo, OnGetInfo);
  cmdMessenger.attach(kGetConfig, OnGetConfig);
  cmdMessenger.attach(kSetConfig, OnSetConfig);
  cmdMessenger.attach(kResetConfig, OnResetConfig);
  cmdMessenger.attach(kSaveConfig, OnSaveConfig);
  cmdMessenger.attach(kActivateConfig, OnActivateConfig);
  cmdMessenger.attach(kSetName, OnSetName);
  cmdMessenger.attach(kGenNewSerial, OnGenNewSerial);
  cmdMessenger.attach(kResetStepper, OnResetStepper);
  cmdMessenger.attach(kSetZeroStepper, OnSetZeroStepper);
  cmdMessenger.attach(kTrigger, OnTrigger);
  cmdMessenger.attach(kResetBoard, OnResetBoard);
  cmdMessenger.attach(kSetLcdDisplayI2C, OnSetLcdDisplayI2C);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Attached callbacks");
#endif

}

void OnResetBoard() {
  EEPROM.setMaxAllowedWrites(1000);
  EEPROM.setMemPool(0, eepromSize);

  configBuffer[0]='\0';
  //readBuffer[0]='\0';
  generateSerial(false);
  //MFButton::setBitStore(&InStatus, &InStatusUpd, MAX_ONB_PIN);
  MFButtonT::setBitStore(&InStatus, &InStatusUpd, NUM_ONB_PINS);
  MFOutput::setBitStore(&OutStatus, NUM_ONB_PINS);
  clearRegisteredPins();
  lastCommand = millis();
  loadConfig();
  _restoreName();
}

// Setup function
void setup()
{
  Serial.begin(115200);
  attachCommandCallbacks();
  OnResetBoard();
  cmdMessenger.printLfCr();
#ifdef COMPUTE_OBJ_SIZES
  // Just required so the compiler doesn't dismiss OBJ_SIZE because unused
  if(OBJ_SIZE[0])
    cmdMessenger.printLfCr();
#endif // COMPUTE_OBJ_SIZES
}

void generateSerial(bool force)
{
  EEPROM.readBlock<char>(MEM_OFFSET_SERIAL, serial, MEM_LEN_SERIAL);
  if (!force&&serial[0]=='S'&&serial[1]=='N') return;
  randomSeed(analogRead(0));
  sprintf(serial,"SN-%03x-", (unsigned int) random(4095));
  sprintf(&serial[7],"%03x", (unsigned int) random(4095));
  EEPROM.writeBlock<char>(MEM_OFFSET_SERIAL, serial, MEM_LEN_SERIAL);
}

void loadConfig()
{
  resetConfig();
  EEPROM.readBlock<char>(MEM_OFFSET_CONFIG, configBuffer, MEM_LEN_CONFIG);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, "Restored config");
  cmdMessenger.sendCmd(kStatus, configBuffer);
#endif
  for(configLength=0;configLength!=MEM_LEN_CONFIG;configLength++) {
    if (configBuffer[configLength]!='\0') continue;
    break;
  }
  readConfig(configBuffer);
  _activateConfig();
}

void _storeConfig()
{
  EEPROM.writeBlock<char>(MEM_OFFSET_CONFIG, configBuffer, MEM_LEN_CONFIG);
}

void SetPowerSavingMode(bool state)
{
  // disable the lights ;)
  powerSavingMode = state;
  PowerSaveLedSegment(state);
#ifdef DEBUG
  if (state)
    cmdMessenger.sendCmd(kStatus, "On");
  else
    cmdMessenger.sendCmd(kStatus, "Off");
#endif
  //PowerSaveOutputs(state);
}

void updatePowerSaving() {
  if (!powerSavingMode && ((millis() - lastCommand) > (POWER_SAVING_TIME * 1000))) {
    // enable power saving
    SetPowerSavingMode(true);
  } else if (powerSavingMode && ((millis() - lastCommand) < (POWER_SAVING_TIME * 1000))) {
    // disable power saving
    SetPowerSavingMode(false);
  }
}

// Loop function
void loop()
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
  updatePowerSaving();

  // if config has been reset
  // and still is not activated
  // do not perform updates
  // to prevent mangling input for config (shared buffers)
  if (!configActivated) return;

  readButtons();
  readEncoder();

  // segments do not need update
  updateSteppers();
  updateServos();
}

bool isPinRegistered(byte pin) {
  return (pin < MAX_LINES ? pinsRegistered.get(pin) : true);
}

//bool isPinRegisteredForType(byte pin, byte type) {
//  return pinsRegistered[pin] == type;
//}

void registerPin(byte pin) {
  if(pin < MAX_LINES) pinsRegistered.set(pin);
}

void clearRegisteredPins() {
    pinsRegistered.clr();
}
//void clearRegisteredPins(byte type) {
//  for(int i=0; i<MODULE_MAX_PINS;++i)
//    if (pinsRegistered[i] == type)
//      pinsRegistered[i] = kTypeNotSet;
//}

void clearRegisteredPins(byte type) {
  byte p[5];
  byte ne = 0;
  byte np;
  MFPeripheral *mfp;

  switch(type) {
    case kTypeButton:
        for(byte i=0; i<buttonsRegistered; i++) pinsRegistered.clr(buttons[i].getPin());
        break;
    case kTypeOutput:
        for(byte i=0; i<outputsRegistered; i++) pinsRegistered.clr(outputs[i].getPin());
        break;
    case kTypeEncoder:
        ne = encodersRegistered;
        mfp = encoders;
        break;
    case kTypeLedSegment:
        ne = ledSegmentsRegistered;
        mfp = ledSegments;
        break;
    case kTypeServo:
        ne = servosRegistered;
        mfp = servos;
        break;
    case kTypeStepper:
        // Stepper array is made of pointers, must write its separate code
        for(byte i=0; i<steppersRegistered; i++) {
            np = steppers[i]->pinCount();
            steppers[i]->getPins(p);
            for(byte k=0; k<np; k++) pinsRegistered.clr(p[k]);
        }
        ne = 0; // skip std processing
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
  if(ne) {
      for(byte i=0; i<ne; i++) {
          np = mfp[i].pinCount();
          mfp[i].getPins(p);
          for(byte k=0; k<np; k++) pinsRegistered.clr(p[k]);
      }
  }

}

///************************************************************************************************
// Registration macro

// Object array version:
#define Mregister(x, xNRegd, MAX_x, pinList, nPins, Name) \
{ \
    byte j;\
    if (xNRegd < MAX_x) {\
        for(j=0; j<nPins; j++) if(isPinRegistered(pinList[j])) break; \
        if(j == nPins) {\
            x[xNRegd].attach(pinList, Name); \
            for(j=0; j<nPins; j++) registerPin(pinList[j]); \
            xNRegd++; \
        }\
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
if(xNRegd == 1) {
    MFButtonT::attachHandler(btnOnRelease, handlerOnRelease);
    MFButtonT::attachHandler(btnOnPress, handlerOnRelease);
}
// end custom section
*/

// Object POINTER array version:
// (see http://forum.arduino.cc/index.php?topic=376860.msg2598440#msg2598440)

#define MregisterPtr(x, xNRegd, MAX_x, pinList, nPins, Name) \
{ \
    byte j;\
    if (xNRegd < MAX_x) {\
        for(j=0; j<nPins; j++) if(isPinRegistered(pinList[j])) break; \
        if(j == nPins) {\
            x[xNRegd] = new typeof(*x[0]); \
            x[xNRegd]->attach(pinList, Name); \
            for(j=0; j<nPins; j++) registerPin(pinList[j]); \
            xNRegd++; \
        }\
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
///************************************************************************************************

///========///
/// OUTPUT ///
///========///

void AddOutput(uint8_t pin, String name)
{
  if (outputsRegistered == MAX_OUTPUTS) return;
  if (isPinRegistered(pin)) return;
  //outputs[outputsRegistered] = MFOutput(pin);
  outputs[outputsRegistered].attach(pin);
  registerPin(pin); //, kTypeOutput);
  outputsRegistered++;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, "Added output " + name);
#endif
}

void ClearOutputs()
{
  clearRegisteredPins(kTypeOutput);
  outputsRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Cleared outputs");
#endif
}

///=========///
/// BUTTONS ///
///=========///
void AddButton(uint8_t pin, String name)
{
  if (buttonsRegistered == MAX_BUTTONS) return;
  if (isPinRegistered(pin)) return;

  //buttons[buttonsRegistered] = MFButtonT(pin, name);
  buttons[buttonsRegistered].attach(pin);
  if(buttonsRegistered == 0) {
      MFButtonT::attachHandler(handlerOnButton);
      //MFButtonT::attachHandler(btnOnRelease, handlerOnRelease);
      //MFButtonT::attachHandler(btnOnPress, handlerOnRelease);
      // BitStore is set during initial setup
  }
  registerPin(pin); //, kTypeButton);
  buttonsRegistered++;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, "Added button " + name);
#endif
}

void ClearButtons()
{
  clearRegisteredPins(kTypeButton);
  buttonsRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Cleared buttons");
#endif
}

///==========///
/// ENCODERS ///
///==========///
void AddEncoder(uint8_t pin1, uint8_t pin2, String name)
{
  if (encodersRegistered == MAX_ENCODERS) return;
  if (!isOnboard(pin1) || !isOnboard(pin2)) return;
  if (isPinRegistered(pin1) || isPinRegistered(pin2)) return;

  encoders[encodersRegistered] = MFEncoder();
  encoders[encodersRegistered].attach(pin1, pin2, encodersRegistered+1);
  if(encodersRegistered == 0) {
      encoders[encodersRegistered].attachHandler(handlerOnEncoder);
      //encoders[encodersRegistered].attachHandler(encLeft, handlerOnEncoder);
      //encoders[encodersRegistered].attachHandler(encLeftFast, handlerOnEncoder);
      //encoders[encodersRegistered].attachHandler(encRight, handlerOnEncoder);
      //encoders[encodersRegistered].attachHandler(encRightFast, handlerOnEncoder);
  }
  registerPin(pin1); //, kTypeEncoder);
  registerPin(pin2); //, kTypeEncoder);
  encodersRegistered++;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Added encoder");
#endif
}

void ClearEncoders()
{
  clearRegisteredPins(kTypeEncoder);
  encodersRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Cleared encoders");
#endif
}


///==========///
/// SEGMENTS ///
///==========///
void AddLedSegment(int dataPin, int csPin, int clkPin, int numDevices, int brightness)
{
  if (ledSegmentsRegistered == MAX_LEDSEGMENTS) return;

  if (isPinRegistered(dataPin) || isPinRegistered(clkPin) || isPinRegistered(csPin)) return;
  if (!isOnboard(dataPin) || !isOnboard(clkPin) || !isOnboard(csPin)) return;

  ledSegments[ledSegmentsRegistered].attach(dataPin,csPin,clkPin,numDevices,brightness); // lc is our object

  registerPin(dataPin); //, kTypeLedSegment);
  registerPin(csPin);   //, kTypeLedSegment);
  registerPin(clkPin);  //, kTypeLedSegment);
  ledSegmentsRegistered++;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Added Led Segment");
#endif
}

void ClearLedSegments()
{
  clearRegisteredPins(kTypeLedSegment);
  for (int i=0; i!=ledSegmentsRegistered; i++) {
    ledSegments[ledSegmentsRegistered].detach();
  }
  ledSegmentsRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, "Cleared segments");
#endif
}

void PowerSaveLedSegment(bool state)
{
  for (int i=0; i!= ledSegmentsRegistered; ++i) {
    ledSegments[i].powerSavingMode(state);
  }

  for (int i=0; i!= outputsRegistered; ++i) {
    outputs[i].powerSavingMode(state);
  }
}

///=========///
/// STEPPER ///
///=========///
void AddStepper(int pin1, int pin2, int pin3, int pin4, int btnPin1)
{
  if (steppersRegistered == MAX_STEPPERS) return;
  if (isPinRegistered(pin1) || isPinRegistered(pin2) || isPinRegistered(pin3) || isPinRegistered(pin4) /* || isPinRegistered(btnPin1) */) {
  if (!isOnboard(pin1) || !isOnboard(pin2) || !isOnboard(pin3) || !isOnboard(pin4)) return;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Conflict with stepper");
#endif
    return;
  }

  byte      pList[4];
  char      *nm;

  //MregisterPtr(steppers, steppersRegistered, MAX_STEPPERS, pList, 4, nm);

  steppers[steppersRegistered] = new MFStepper(pin1, pin2, pin3, pin4 /*, btnPin1*/ ); // is our object
  steppers[steppersRegistered]->setMaxSpeed(STEPPER_SPEED);
  steppers[steppersRegistered]->setAcceleration(STEPPER_ACCEL);
  registerPin(pin1);  //, kTypeStepper);
  registerPin(pin2);  //, kTypeStepper);
  registerPin(pin3);  //, kTypeStepper);
  registerPin(pin4);  //, kTypeStepper);
  // autoreset is not released yet
  // registerPin(btnPin1, kTypeStepper);
  steppersRegistered++;

#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Added stepper");
#endif
}

void ClearSteppers()
{
  for (int i=0; i!=steppersRegistered; i++)
  {
    delete steppers[steppersRegistered];
  }
  clearRegisteredPins(kTypeStepper);
  steppersRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Cleared steppers");
#endif
}

///========///
/// SERVOS ///
///========///
void AddServo(int pin)
{
  if (servosRegistered == MAX_MFSERVOS) return;
  if (isPinRegistered(pin)) return;
  if (!isOnboard(pin)) return;

  servos[servosRegistered].attach(pin, true);
  registerPin(pin);  //, kTypeServo);
  servosRegistered++;
}

void ClearServos()
{
  for (int i=0; i!=servosRegistered; i++)
  {
    servos[servosRegistered].detach();
  }
  clearRegisteredPins(kTypeServo);
  servosRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Cleared servos");
#endif
}

///=============///
/// LCD Display ///
///=============///
void AddLcdDisplay (uint8_t address, uint8_t cols, uint8_t lines, String name)
{
  if (lcd_12cRegistered == MAX_MFLCD_I2C) return;
  lcd_I2C[lcd_12cRegistered].attach(address, cols, lines);
  lcd_12cRegistered++;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Added lcdDisplay");
#endif
}

void ClearLcdDisplays()
{
  for (int i=0; i!=lcd_12cRegistered; i++) {
    lcd_I2C[lcd_12cRegistered].detach();
  }
  lcd_12cRegistered = 0;
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Cleared lcdDisplays");
#endif
}

///************************************************************************************************


///================///
/// EVENT HANDLERS ///
///================///
void handlerOnButton(byte eventId, uint8_t pin, const char *name)  //String name)
{
  cmdMessenger.sendCmdStart(kButtonChange);
  cmdMessenger.sendCmdArg(name);
  cmdMessenger.sendCmdArg(eventId);
  cmdMessenger.sendCmdEnd();
};

void handlerOnEncoder(byte eventId, uint8_t pin, const char *name)  //String name)
{
  cmdMessenger.sendCmdStart(kEncoderChange);
  cmdMessenger.sendCmdArg(name);
  cmdMessenger.sendCmdArg(eventId);
  cmdMessenger.sendCmdEnd();
};

/**
 ** config stuff
 **/
void OnSetConfig()
{
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Setting config start");
#endif

  lastCommand = millis();
  String cfg = cmdMessenger.readStringArg();
  int cfgLen = cfg.length();
  int bufferSize = MEM_LEN_CONFIG - (configLength+cfgLen);

  if (bufferSize>1) {
    cfg.toCharArray(&configBuffer[configLength], bufferSize);
    configLength += cfgLen;
    cmdMessenger.sendCmd(kStatus,configLength);
  } else
    cmdMessenger.sendCmd(kStatus,-1);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,"Setting config end");
#endif
}

void resetConfig()
{
  ClearButtons();
  ClearEncoders();
  ClearOutputs();
  ClearLedSegments();
  ClearServos();
  ClearSteppers();
  ClearLcdDisplays();
  configLength = 0;
  configActivated = false;
}

void OnResetConfig()
{
  resetConfig();
  cmdMessenger.sendCmd(kStatus, "OK");
}

void OnSaveConfig()
{
  _storeConfig();
  cmdMessenger.sendCmd(kConfigSaved, "OK");
}

void OnActivateConfig()
{
  readConfig(configBuffer);
  _activateConfig();
  cmdMessenger.sendCmd(kConfigActivated, "OK");
}

void _activateConfig() {
  configActivated = true;
}

void parse(char **parms, byte nparms, char *p0)
{
  char *p = p0;
  //if(nparms==0) return;
  for(int8_t i=0; i<(nparms-1); i++) {
      parms[i] = strtok_r(NULL, ".", &p);
  }
  parms[nparms-1] = strtok_r(NULL, ":", &p);
}

void readConfig(String cfg) {
  char readBuffer[MEM_LEN_CONFIG+1] = "";
  char *p = NULL;
  cfg.toCharArray(readBuffer, MEM_LEN_CONFIG);

  char *command = strtok_r(readBuffer, ".", &p);
  char *params[6];
  if (*command == 0) return;

  do {
    switch (atoi(command)) {
      case kTypeButton:
        parse(params, 2, p); // pin, name
        AddButton(atoi(params[0]), params[1]);
      break;

      case kTypeOutput:
        parse(params, 2, p); // pin, name
        AddOutput(atoi(params[0]), params[1]);
      break;

      case kTypeLedSegment:
        parse(params, 6, p); // pinData, pinCS, pinCLK, brightness, numModules, Name
        // AddLedSegment(dataPin, clkPin, csPin, numDevices, brightness)
        AddLedSegment(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[4]), atoi(params[3]));
      break;

      case kTypeStepper:
        parse(params, 6, p); // pin1, pin2, pin3, pin4, btnpin, Name
        // AddStepper(pin1, pin2, int , pin4)
        AddStepper(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
      break;

      case kTypeServo:
        parse(params, 2, p); // pin1, Name
        // AddServo(pin)
        AddServo(atoi(params[0]));
      break;

      case kTypeEncoder:
        parse(params, 3, p); // pin1, pin2, Name
        // AddEncoder(pin1, pin2, name)
        AddEncoder(atoi(params[0]), atoi(params[1]), params[2]);
      break;

      case kTypeLcdDisplayI2C:
        parse(params, 4, p); // addr, cols, lines, name
        // AddLcdDisplay(address, cols, lines, name)
        AddLcdDisplay(atoi(params[0]), atoi(params[1]), atoi(params[2]), params[3]);
      break;
      // New IO bank peripherals (GCC 2018-01):
      case kTypeInputMtx:
        ///parse(params, 4, p); // addr, cols, lines, Name
        ///AddInputMtx(....);
      break;
      case kTypeInput165:
        ///parse(params, 5, p); // pinData, pinCS, pinCLK, numModules, Name
        ///AddInput165(....);
      break;
      case kTypeOutput595:
        ///parse(params, 5, p); // pinData, pinCS, pinCLK, numModules, Name
        ///AddOutput595(....);
      break;
      case kTypeOutLEDDM13:
        ///parse(params, 5, p); // pinData, pinCS, pinCLK, numModules, Name
        ///AddOutLEDDM13(....);
      break;
      case kTypeOutLED5940:
        ///parse(params, 5, p); // pinData, pinCS, pinCLK, numModules, Name
        ///AddOutLED5940(....);
      break;
      case kTypeInOutMCPS:
        ///parse(params, 6, p); // pinData, pinCS, pinCLK, addr, numModules, Name
        ///AddInOutMCPS(....);
      break;
      case kTypeInOutMCP0:
        ///parse(params, 5, p); // pinSDA, pinSCL, addr, numModules, Name
        ///AddInOutMCP0(....);
      break;

      default:
        // read to the end of the current command which is
        // apparently not understood
        params[0] = strtok_r(NULL, ":", &p); // read to end of unknown command
    }
    command = strtok_r(NULL, ".", &p);
  } while (command!=NULL);
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
  lastCommand = millis();
  cmdMessenger.sendCmd(kStatus,"n/a");
}

void OnGetInfo() {
  lastCommand = millis();
  cmdMessenger.sendCmdStart(kInfo);
  cmdMessenger.sendCmdArg(type);
  cmdMessenger.sendCmdArg(name);
  cmdMessenger.sendCmdArg(serial);
  cmdMessenger.sendCmdArg(version);
  cmdMessenger.sendCmdEnd();
}

void OnGetConfig()
{
  lastCommand = millis();
  cmdMessenger.sendCmdStart(kInfo);
  cmdMessenger.sendCmdArg(configBuffer);
  cmdMessenger.sendCmdEnd();
}

// Callback function that sets led on or off
void OnSetPin()
{
  // Read led state argument, interpret string as boolean
  int pin = cmdMessenger.readIntArg();
  int state = cmdMessenger.readIntArg();
  // Set led
  digitalWrite(pin, state > 0 ? HIGH : LOW);
  lastCommand = millis();
}

void OnInitModule()
{
  int module = cmdMessenger.readIntArg();
  int subModule = cmdMessenger.readIntArg();
  int brightness = cmdMessenger.readIntArg();
  ledSegments[module].setBrightness(subModule,brightness);
  lastCommand = millis();
}

void OnSetModule()
{
  int module = cmdMessenger.readIntArg();
  int subModule = cmdMessenger.readIntArg();
  char * value = cmdMessenger.readStringArg();
  byte points = (byte) cmdMessenger.readIntArg();
  byte mask = (byte) cmdMessenger.readIntArg();
  ledSegments[module].display(subModule, value, points, mask);
  lastCommand = millis();
}

void OnSetStepper()
{
  int stepper    = cmdMessenger.readIntArg();
  long newPos    = cmdMessenger.readLongArg();

  if (stepper >= steppersRegistered) return;
  steppers[stepper]->moveTo(newPos);
  lastCommand = millis();
}

void OnResetStepper()
{
  int stepper    = cmdMessenger.readIntArg();

  if (stepper >= steppersRegistered) return;
  steppers[stepper]->reset();
  lastCommand = millis();
}

void OnSetZeroStepper()
{
  int stepper    = cmdMessenger.readIntArg();

  if (stepper >= steppersRegistered) return;
  steppers[stepper]->setZero();
  lastCommand = millis();
}

void OnSetServo()
{
  int servo    = cmdMessenger.readIntArg();
  int newValue = cmdMessenger.readIntArg();
  if (servo >= servosRegistered) return;
  servos[servo].moveTo(newValue);
  lastCommand = millis();
}

void OnSetLcdDisplayI2C()
{
  int address  = cmdMessenger.readIntArg();
  char *output   = cmdMessenger.readStringArg();
  lcd_I2C[address].display(output);
  cmdMessenger.sendCmd(kStatus, output);
}

void updateSteppers()
{
  for (int i=0; i!=steppersRegistered; i++) {
    steppers[i]->update();
  }
}

void updateServos()
{
  for (int i=0; i!=servosRegistered; i++) {
    servos[i].update();
  }
}

void readButtons()
{
  long now = millis();
  if (now-lastButtonUpdate > 10) {
    lastButtonUpdate = now;
  for(int i=0; i!=buttonsRegistered; i++) {
    buttons[i].update();
  }
  }
}

void readEncoder()
{
  for(int i=0; i!=encodersRegistered; i++) {
    encoders[i].update();
  }
}

void OnGenNewSerial()
{
  generateSerial(true);
  cmdMessenger.sendCmdStart(kInfo);
  cmdMessenger.sendCmdArg(serial);
  cmdMessenger.sendCmdEnd();
}

void OnSetName() {
  String cfg = cmdMessenger.readStringArg();
  cfg.toCharArray(&name[0], MEM_LEN_NAME);
  _storeName();
  cmdMessenger.sendCmdStart(kStatus);
  cmdMessenger.sendCmdArg(name);
  cmdMessenger.sendCmdEnd();
}

void _storeName() {
  char prefix[] = "#";
  EEPROM.writeBlock<char>(MEM_OFFSET_NAME, prefix, 1);
  EEPROM.writeBlock<char>(MEM_OFFSET_NAME+1, name, MEM_LEN_NAME-1);
}

void _restoreName() {
  char testHasName[1] = "";
  EEPROM.readBlock<char>(MEM_OFFSET_NAME, testHasName, 1);
  if (testHasName[0] != '#') return;

  EEPROM.readBlock<char>(MEM_OFFSET_NAME+1, name, MEM_LEN_NAME-1);
}

void OnTrigger()
{
  for(int i=0; i!=buttonsRegistered; i++) {
    buttons[i].trigger();
  }
}
