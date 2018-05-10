//GCC
// mobiflight.h

#define DEBUG
#define NO_ARDUINO_IDE

#define MTYPE_MEGA 1
#define MTYPE_MICRO 2
#define MTYPE_UNO 3
#define MF_STEPPER_SUPPORT 1
#define MF_SERVO_SUPPORT 1
#define MODULETYPE MTYPE_MEGA
//#define MODULETYPE MTYPE_MICRO
//#define MODULETYPE MTYPE_UNO

// Fixed definitions
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

// Custom (parametric) definitions
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
#define isOnboard(pin)  ((pin)<NUM_ONB_PINS)
#define MAX_LINES       128 //256 // Includes actual+virtual pins
#define NUM_VLINES      (MAX_LINES-MODULE_MAX_PINS) // Includes actual+virtual pins

#include <EEPROMex.h>
#include <bitStore.h>   //GCC
#include <CmdMessenger.h>  // CmdMessenger
#include <TicksPerSecond.h>
#include <Wire.h>

#include <MFButtonT.h>        //#include <MFButton.h>
#include <MFOutput.h>
#include <Multiplexer.h>
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
#include <MFInputMPX.h>
#include <MFInput165.h>
#include <MFOutput595.h>
#include <MFOutLEDDM13.h>
//#include <MFOutLED5940.h>
#include <MFIO_MCPS.h>
#include <MFIO_MCP0.h>

extern const byte MEM_OFFSET_NAME;
extern const byte MEM_LEN_NAME;
extern const byte MEM_OFFSET_SERIAL;
extern const byte MEM_LEN_SERIAL;
extern const byte MEM_OFFSET_CONFIG;
extern const int  MEM_LEN_CONFIG;

extern char type[];
extern char serial[];
extern char name[];
extern int eepromSize;

extern char        configBuffer[];
extern int         configLength;
extern boolean     configActivated;
extern bool        powerSavingMode;

extern byte            pBufIn  [];
extern byte            pBufOut [];
extern byte            pBufShd [];
extern bitStore<byte>  pinsRegIn;
extern bitStore<byte>  pinsRegOut;
extern bitStore<byte>  pinsRegSharable;

extern
const unsigned long    POWER_SAVING_TIME;

extern CmdMessenger    cmdMessenger;
extern unsigned long   lastCommand;

extern MFOutput     outputs[];
extern MFButtonT    buttons[];
extern MFSegments   *ledSegments[];
extern MFEncoder    *encoders[];
extern MFStepper    *steppers[];
extern MFServo      *servos[];
extern MFLCDDisplay lcd_I2C[];

extern byte outputsRegistered;
extern byte buttonsRegistered;
extern long lastButtonUpdate;
extern byte ledSegmentsRegistered;
extern byte encodersRegistered;
extern byte steppersRegistered;
extern byte servosRegistered;
extern byte lcd_i2cRegistered;

extern MFPeripheral *IOBlocks[];
extern byte IOBlocksRegistered;
extern byte            IOStatusValBuf[];
extern byte            IOStatusNewBuf[];
extern bitStore<byte>  IOStatusVal;
extern bitStore<byte>  IOStatusNew;

// This is the list of recognized (and managed) peripheral types.
enum E_ptypes
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
enum E_cmds
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

enum E_errors
{
  kErrNone ,        // 0
  kErrFull,         // 1
  kErrConflict,     // 2
  kErrNotOnboard,   // 3
  kErrGeneric,      // 4
};



//====================================================================================
#ifdef NO_ARDUINO_IDE

// Not required by Arduino IDE, added to allow compilation through a conventional IDE (CodeBlocks)
void attachCommandCallbacks(void);
void resetBoard(void);
void generateSerial(bool force);
void storeName(void);
void restoreName(void);
void loadConfig(void);
void storeConfig(void);
void resetConfig(void);
void activateConfig(void);
void readConfig(String cfg);
void parse(char **parms, byte nparms, char *p0);
void setPowerSavingMode(bool state);
void updatePowerSaving(void);
void updateSteppers(void);
void updateServos(void);
void updateIOBlocks(void);
void readButtons(void);
void readEncoder(void);
void setup(void);
void loop(void);
//bool isPinRegisteredForType(byte pin, byte type);
//void registerPin(byte pin, byte type);
byte isPinRegdAsIn(byte pin);
byte isPinRegdAsOut(byte pin);
byte isPinSharable(byte pin);
byte isPinRegistered(byte pin);
void registerPin(byte pin);
byte registerPin(byte pin, byte isInput, byte sharable, byte checkonly=0);
void clearRegisteredPins(void);
void clearRegisteredPins(byte type);
void unregIOBlocks(void);
template<class T> byte registerPeripherals(T *vec[], byte &n, byte nMax, byte *argList, byte nPins, char *Name);
template<class T> void unregPeripherals(T *vec[], byte &n);
template<class T> byte registerIOB(byte *argList, byte nPins, byte base, byte nBlocks, char *Name);
void unregIOBlocks(void);
void AddOutput(byte pin = 1);
void ClearOutputs(void);
void AddButton(byte pin = 1);
void ClearButtons(void);
void AddEncoder(byte pin1 = 1, byte pin2 = 2, byte type = 0, char *name = "Encoder");
void ClearEncoders(void);
void AddLedSegment(byte dataPin, byte csPin, byte clkPin, byte numDevices, byte brightness);
void ClearLedSegments(void);
void powerSaveLedSegment(bool state);
void AddStepper(byte pin1, byte pin2, byte pin3, byte pin4, byte btnPin1);
void ClearSteppers(void);
void AddServo(byte pin);
void ClearServos(void);
void AddLcdDisplay (byte address = 0x24, byte cols = 16, byte lines = 2, char *name = "LCD");
void ClearLcdDisplays(void);

void ClearIOBlocks(void);
void AddInputMtx(byte Row0, byte NRows, byte Col0, byte NCols, byte base);
void AddInputMPX(byte inPin, byte firstSelPin, byte base);
void AddInput165(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices);
void AddOutput595(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices);
void AddOutLEDDM13(byte dataPin, byte csPin, byte clkPin, byte base, byte numDevices);
void AddIOMCP0(byte SDAPin, byte SCLPin, byte addr, byte IOdir1, byte IOdir2, byte base);
void AddIOMCPS(byte MOSIPin, byte MISOPin, byte csPin, byte clkPin, byte addr, byte IOdir1, byte IOdir2, byte base);

void handlerOnButton(byte eventId, byte pin, const char *); //String name);
void handlerOnEncoder(byte eventId, byte pin, const char *); //String name);
void OnSetConfig(void);
void OnResetConfig(void);
void OnSaveConfig(void);
void OnActivateConfig(void);
void OnUnknownCommand(void);
void OnResetBoard(void);
void OnGetInfo(void);
void OnGetConfig(void);
void OnSetPin(void);
void OnInitLEDModule(void);
void OnSetLEDModule(void);
void OnSetStepper(void);
void OnResetStepper(void);
void OnSetZeroStepper(void);
void OnSetServo(void);
void OnSetLcdDisplayI2C(void);
void OnGenNewSerial(void);
void OnSetBoardName(void);
void OnTrigger(void);

#endif // NO_ARDUINO_IDE
