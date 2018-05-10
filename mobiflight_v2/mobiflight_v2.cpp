/**
 * Includes Core Arduino functionality
 **/
char foo;

#include <Arduino.h>
#include <avr/pgmspace.h>

///NOTE GCC - restore for orig MF
// >>> Remove .h import:
#include "mobiflight.h"
///END NOTE

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
// 1.9.0 : Support for rotary encoders with different detent configurations
// 2.0.0 : [WIP] GCC mods
const char version[8] = "2.0.0";

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

char            configBuffer[MEM_LEN_CONFIG] = "";

int             configLength = 0;
boolean         configActivated = false;

bool            powerSavingMode = false;

//byte            pinRegBuf[roundUp(MAX_LINES)];
//bitStore<byte>  pinsRegistered(pinRegBuf,roundUp(MAX_LINES));

// For pin overlay:
// a flag in <pinsRegIn> marks that the pin is registered as INPUT
// a flag in <pinsRegOut> marks that the pin is registered as OUTPUT
// a flag in <pinsRegInShared> (implies reg in <pinsRegIn>) marks that the input pin
//   can be additionally claimed by another client trying to register a sharable input pin
// a flag in <pinsRegOutShared> (implies reg in <pinsRegOut>) marks that the output pin
//   can be additionally claimed by another client trying to register a sharable output pin
// Currently, only Onboard pins can be shared; this is meant to allow for sharing of HW driving lines
// for certain interfaces (e.g. software I2C).
// Conceptually, Virtual pins could also be shared, but it must be defined exactly how,
// what for and to what extent.
byte            pBufIn  [roundUp(MAX_LINES)];
byte            pBufOut [roundUp(MAX_LINES)];
byte            pBufShd [NUM_ONB_PINS];         // Only onboard pins can (currently) be shared
bitStore<byte>  pinsRegIn (pBufIn, sizeof(pBufIn));
bitStore<byte>  pinsRegOut(pBufOut,sizeof(pBufOut));
bitStore<byte>  pinsRegSharable(pBufShd, sizeof(pBufShd));

const
unsigned long   POWER_SAVING_TIME = 60*15; // in seconds

CmdMessenger    cmdMessenger = CmdMessenger(Serial);
unsigned long   lastCommand;

MFOutput outputs[MAX_OUTPUTS];
byte outputsRegistered = 0;

//MFButton buttons[MAX_BUTTONS];
MFButtonT buttons[MAX_BUTTONS];
byte buttonsRegistered = 0;
long lastButtonUpdate  = 0;

MFSegments *ledSegments[MAX_LEDSEGMENTS];
byte ledSegmentsRegistered = 0;

MFEncoder *encoders[MAX_ENCODERS];
byte encodersRegistered = 0;

MFStepper *steppers[MAX_STEPPERS]; //
byte steppersRegistered = 0;

MFServo *servos[MAX_MFSERVOS];
byte servosRegistered = 0;

MFLCDDisplay lcd_I2C[MAX_MFLCD_I2C];
byte lcd_i2cRegistered = 0;

MFPeripheral *IOBlocks[MAX_IOBLOCKS];
byte IOBlocksRegistered = 0;

/*
byte        inStatusBuf   [roundUp(MAX_BUTTONS)];
byte        inStatusUpdBuf[roundUp(MAX_BUTTONS)];
byte        outStatusBuf  [roundUp(MAX_OUTPUTS)];
bitStore<byte>  InStatus   (inStatusBuf,    roundUp(MAX_BUTTONS));
bitStore<byte>  InStatusUpd(inStatusUpdBuf, roundUp(MAX_BUTTONS));
bitStore<byte>  OutStatus  (outStatusBuf,   roundUp(MAX_OUTPUTS));
*/
byte            IOStatusValBuf[roundUp(MAX_LINES)];
byte            IOStatusNewBuf[roundUp(MAX_LINES)];
bitStore<byte>  IOStatusVal(IOStatusValBuf, roundUp(MAX_LINES));
bitStore<byte>  IOStatusNew(IOStatusNewBuf, roundUp(MAX_LINES));

/// Dummy objects created in order to verify actual memory allocation during development

//#define COMPUTE_OBJ_SIZES
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
MF_LedControl SAMPLE_LedCtrl(1,2,3,4);
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

enum E_ptypes;
enum E_cmds;
enum E_errors;
/*
enum
{
  kTypeNotSet,        // 0
  kTypeButton,        // 1
  kTypeEncoderSingleDetent, // 2 (retained for backwards compatibility, use kTypeEncoder for new configs)
  kTypeOutput,        // 3
  kTypeLedSegment,    // 4
  kTypeStepper,       // 5
  kTypeServo,         // 6
  kTypeLcdDisplayI2C, // 7
  kTypeEncoder,       // 8
  // New IOblock peripherals (GCC 2018-01):
  kTypeInputMtx,      // 9
  kTypeInput165,      // 10
  kTypeOutput595,     // 11
  kTypeOutLEDDM13,    // 12
  kTypeOutLED5940,    // 13
  kTypeInOutMCPS,     // 14
  kTypeInOutMCP0,     // 15
  kTypeInputMPX,      // 16
};

// This is the list of recognized commands. These can be commands that can either be sent or received.
// In order to receive, attach a callback function to these events
enum
{
  kInitLEDModule,      // 0
  kSetLEDModule,       // 1
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
  kSetBoardName,       // 19
  kGenNewSerial,       // 20
  kResetStepper,       // 21
  kSetZeroStepper,     // 22
  kTrigger,            // 23
  kResetBoard,         // 24
  kSetLcdDisplayI2C,   // 25
};

enum {
  kErrNone ,        // 0
  kErrFull,         // 1
  kErrConflict,     // 2
  kErrNotOnboard,   // 3
  kErrGeneric,      // 4
};
*/

// Callbacks define on which received commands we take action
void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);

  // *** System setup ***
  cmdMessenger.attach(kGetInfo, OnGetInfo);
  cmdMessenger.attach(kGenNewSerial, OnGenNewSerial);
  cmdMessenger.attach(kResetBoard, OnResetBoard);
  cmdMessenger.attach(kSetBoardName, OnSetBoardName);

  // *** Configuration ***
  cmdMessenger.attach(kGetConfig, OnGetConfig);
  cmdMessenger.attach(kSetConfig, OnSetConfig);
  cmdMessenger.attach(kResetConfig, OnResetConfig);
  cmdMessenger.attach(kSaveConfig, OnSaveConfig);
  cmdMessenger.attach(kActivateConfig, OnActivateConfig);

  // *** Output commands ***
  cmdMessenger.attach(kTrigger, OnTrigger);
  cmdMessenger.attach(kSetPin, OnSetPin);
  cmdMessenger.attach(kSetStepper, OnSetStepper);
  cmdMessenger.attach(kSetServo, OnSetServo);
  cmdMessenger.attach(kResetStepper, OnResetStepper);
  cmdMessenger.attach(kSetZeroStepper, OnSetZeroStepper);
  cmdMessenger.attach(kInitLEDModule, OnInitLEDModule);
  cmdMessenger.attach(kSetLEDModule, OnSetLEDModule);
  cmdMessenger.attach(kSetLcdDisplayI2C, OnSetLcdDisplayI2C);

#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Attached callbacks"));
#endif

}

void storeName()
{
  char prefix[] = "#";
  EEPROM.writeBlock<char>(MEM_OFFSET_NAME, prefix, 1);
  EEPROM.writeBlock<char>(MEM_OFFSET_NAME+1, name, MEM_LEN_NAME-1);
}

void restoreName()
{
  char testHasName[1] = "";
  EEPROM.readBlock<char>(MEM_OFFSET_NAME, testHasName, 1);
  if (testHasName[0] != '#') return;

  EEPROM.readBlock<char>(MEM_OFFSET_NAME+1, name, MEM_LEN_NAME-1);
}

void resetBoard()
{
  EEPROM.setMaxAllowedWrites(1000);
  EEPROM.setMemPool(0, eepromSize);

  configBuffer[0]='\0';
  //readBuffer[0]='\0';
  generateSerial(false);
  MFButtonT::setBitStore(&IOStatusVal, &IOStatusNew, NUM_ONB_PINS);
  MFOutput::setBitStore(&IOStatusVal, &IOStatusNew, NUM_ONB_PINS);
  clearRegisteredPins();
  lastCommand = millis();
  loadConfig();
  restoreName();
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

void resetConfig()
{
  ClearButtons();
  ClearEncoders();
  ClearOutputs();
  ClearLedSegments();
  ClearServos();
  ClearSteppers();
  ClearLcdDisplays();
  ClearIOBlocks();
  configLength = 0;
  configActivated = false;
}

void loadConfig()
{
  resetConfig();
  EEPROM.readBlock<char>(MEM_OFFSET_CONFIG, configBuffer, MEM_LEN_CONFIG);
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus, PSTR("Restored config"));
  cmdMessenger.sendCmd(kStatus, configBuffer);
#endif
  for(configLength=0;configLength!=MEM_LEN_CONFIG;configLength++) {
    if (configBuffer[configLength]!='\0') continue;
    break;
  }
  readConfig(configBuffer);
  activateConfig();
}

void storeConfig()
{
  EEPROM.writeBlock<char>(MEM_OFFSET_CONFIG, configBuffer, MEM_LEN_CONFIG);
}

void activateConfig()
{
  configActivated = true;
}

void parse(char **parms, byte nparms, char **pp)
{
  char *tp;
  //if(nparms==0) return;
  for(int8_t i=0; i<(nparms-1); i++) {
      parms[i] = strtok_r(NULL, ".", pp);
  }
  /// should handle the case of less parms found than expected
  parms[nparms-1] = tp = strtok_r(NULL, ":", pp);   // last token returned can be followed either by ':' or end of string
  // if more parameters are provided than required, the additional ones
  // become part of the last one: strip them.
  while((tp = strchr(tp, '.')) != NULL) parms[nparms-1] = ++tp;
}

#define atoi(s) fast_atoi(s)

void readConfig(String cfg)
{
  char readBuffer[MEM_LEN_CONFIG+1] = "";
  char *p = NULL;
  cfg.toCharArray(readBuffer, MEM_LEN_CONFIG);

  char *command = strtok_r(readBuffer, ".", &p);    // First call; will do next ones in parse(.)
  char *params[8];
  if (*command == 0) return;

  do {
    switch (atoi(command)) {
      case kTypeButton:
        parse(params, 2, &p); // pin, name
        AddButton(atoi(params[0])); //, params[1]);
      break;

      case kTypeOutput:
        parse(params, 2, &p); // pin, name
        AddOutput(atoi(params[0])); //, params[1]);
      break;

      case kTypeLedSegment:
        parse(params, 6, &p); // pinData, pinCS, pinCLK, brightness, numModules, Name
        // AddLedSegment(dataPin, clkPin, csPin, numDevices, brightness)
        AddLedSegment(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[4]), atoi(params[3]));
      break;

      case kTypeStepper:
        parse(params, 6, &p); // pin1, pin2, pin3, pin4, btnpin, Name
        // AddStepper(pin1, pin2, int , pin4)
        AddStepper(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
      break;

      case kTypeServo:
        parse(params, 2, &p); // pin1, Name
        // AddServo(pin)
        AddServo(atoi(params[0]));
      break;

      case kTypeEncoderSingleDetent:
        parse(params, 3, &p); // pin1, pin2, Name
        // AddEncoder(pin1, pin2, encType=0, name)
        AddEncoder(atoi(params[0]), atoi(params[1]), 0, params[2]);
      break;

      case kTypeEncoder:
        parse(params, 4, &p); // pin1, pin2, encType, Name
        // AddEncoder(pin1, pin2, encType, name)
        AddEncoder(atoi(params[0]), atoi(params[1]), atoi(params[2]), params[3]);
      break;

      case kTypeLcdDisplayI2C:
        parse(params, 4, &p); // addr, cols, lines, name
        // AddLcdDisplay(address, cols, lines, name)
        AddLcdDisplay(atoi(params[0]), atoi(params[1]), atoi(params[2]), params[3]);
      break;
      // New IO bank peripherals (GCC 2018-01):
      case kTypeInputMtx:
        parse(params, 5, &p); // Row0, Col0, NRows, NCols, base
        AddInputMtx(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
      break;
      case kTypeInputMPX:
        parse(params, 2, &p); // inPin, firstSelPin, base
        AddInputMPX(atoi(params[0]), atoi(params[1]), atoi(params[2]));
      break;
      case kTypeInput165:
        parse(params, 5, &p); // pinData, pinCS, pinCLK, base, numDevices
        AddInput165(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
      break;
      case kTypeOutput595:
        parse(params, 5, &p); // pinData, pinCS, pinCLK, base, numDevices
        AddOutput595(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
      break;
      case kTypeOutLEDDM13:
        parse(params, 5, &p); // pinData, pinCS, pinCLK, base, numDevices
        AddOutLEDDM13(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
      break;
      case kTypeInOutMCP0:
        parse(params, 6, &p); // pinSDA, pinSCL, addr, IOdir1, IOdir2, base
        AddIOMCP0(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]),
                  atoi(params[4]), atoi(params[5]));
      break;
      case kTypeInOutMCPS:
        parse(params, 8, &p); // MOSI, MISO, pinCS, pinCLK, addr, IOdir1, IOdir2, base
        AddIOMCPS(atoi(params[0]), atoi(params[1]), atoi(params[2]), atoi(params[3]),
                  atoi(params[4]), atoi(params[5]), atoi(params[6]), atoi(params[7]));
      break;

      case kTypeOutLED5940:
      default:
        // read to the end of the current command which is
        // apparently not understood
        params[0] = strtok_r(NULL, ":", &p); // read to end of unknown command
    }
    command = strtok_r(NULL, ".", &p);
  } while (command!=NULL);
}

void setPowerSavingMode(bool state)
{
  // disable the lights ;)
  powerSavingMode = state;
  powerSaveLedSegment(state);
  ///TODO Add powersave for other peripherals that might have it
#ifdef DEBUG
  if (state)
    cmdMessenger.sendCmd(kStatus, PSTR("On"));
  else
    cmdMessenger.sendCmd(kStatus, PSTR("Off"));
#endif
  //PowerSaveOutputs(state);
}

void updatePowerSaving()
{
  if (!powerSavingMode && ((millis() - lastCommand) > (POWER_SAVING_TIME * 1000))) {
    // enable power saving
    setPowerSavingMode(true);
  } else if (powerSavingMode && ((millis() - lastCommand) < (POWER_SAVING_TIME * 1000))) {
    // disable power saving
    setPowerSavingMode(false);
  }
}

void powerSaveLedSegment(bool state)
{
  for (int i=0; i!= ledSegmentsRegistered; ++i) {
    ledSegments[i]->powerSavingMode(state);
  }

  for (int i=0; i!= outputsRegistered; ++i) {
    outputs[i].powerSavingMode(state);
  }
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
    servos[i]->update();
  }
}

void updateIOBlocks(void)
{
  for (int i=0; i!=IOBlocksRegistered; i++) {
    IOBlocks[i]->update(NULL, NULL);
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
    encoders[i]->update();
  }
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
  updateIOBlocks();
}

//#include "MF_registration.inc"

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

///===============///
/// Config events ///
///===============///

void OnSetConfig()
{
#ifdef DEBUG
  cmdMessenger.sendCmd(kStatus,PSTR("Setting config start"));
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
  cmdMessenger.sendCmd(kStatus,PSTR("Setting config end"));
#endif
}

void OnResetConfig()
{
  resetConfig();
  cmdMessenger.sendCmd(kStatus, PSTR("OK"));
}

void OnSaveConfig()
{
  storeConfig();
  cmdMessenger.sendCmd(kConfigSaved, PSTR("OK"));
}

void OnActivateConfig()
{
  readConfig(configBuffer);
  activateConfig();
  cmdMessenger.sendCmd(kConfigActivated, PSTR("OK"));
}

void OnUnknownCommand()
{
  // Called when a received command has no attached function
  lastCommand = millis();
  cmdMessenger.sendCmd(kStatus,PSTR("n/a"));
}

void OnResetBoard()
{
  resetBoard();
}

void OnGetInfo()
{
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

void OnSetPin()
{
  // Read led state argument, interpret string as boolean
  int pin = cmdMessenger.readIntArg();
  int state = cmdMessenger.readIntArg();
  // Set output line
  //digitalWrite(pin, state > 0 ? HIGH : LOW);
  outputs[pin].set(state);
  lastCommand = millis();
}

void OnInitLEDModule()
{
  int module = cmdMessenger.readIntArg();
  int subModule = cmdMessenger.readIntArg();
  int brightness = cmdMessenger.readIntArg();
  ledSegments[module]->setBrightness(subModule,brightness);
  lastCommand = millis();
}

void OnSetLEDModule()
{
  int module = cmdMessenger.readIntArg();
  int subModule = cmdMessenger.readIntArg();
  char * value = cmdMessenger.readStringArg();
  byte points = (byte) cmdMessenger.readIntArg();
  byte mask = (byte) cmdMessenger.readIntArg();
  ledSegments[module]->display(subModule, value, points, mask);
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
  servos[servo]->moveTo(newValue);
  lastCommand = millis();
}

void OnSetLcdDisplayI2C()
{
  int address  = cmdMessenger.readIntArg();
  char *output   = cmdMessenger.readStringArg();
  lcd_I2C[address].display(output);
  cmdMessenger.sendCmd(kStatus, output);
}

void OnGenNewSerial()
{
  generateSerial(true);
  cmdMessenger.sendCmdStart(kInfo);
  cmdMessenger.sendCmdArg(serial);
  cmdMessenger.sendCmdEnd();
}

void OnSetBoardName()
{
  String cfg = cmdMessenger.readStringArg();
  cfg.toCharArray(&name[0], MEM_LEN_NAME);
  storeName();
  cmdMessenger.sendCmdStart(kStatus);
  cmdMessenger.sendCmdArg(name);
  cmdMessenger.sendCmdEnd();
}

void OnTrigger()
{
  for(int i=0; i!=buttonsRegistered; i++) {
    buttons[i].trigger();
  }
}
