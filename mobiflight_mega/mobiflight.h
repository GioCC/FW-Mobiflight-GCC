/********************************************************************
*
* mobiflight.h
*
* Declarations for MobiFlight core
* Originally part of main file mobiflight_XXXX.ino, now extracted to allow
* splitting the original file to a more manageable size and also
* allow the project to be built with a "conventional" IDE (not the Arduino one)
*
* Project:  MobiFlight v2
* Hardware: Arduino Mega (/ Uno / Nano)
* Author:   Giorgio CROCI CANDIANI g.crocic@gmail.com
* Date:     2018-01
*
********************************************************************/

//GCC
// mobiflight.h
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
