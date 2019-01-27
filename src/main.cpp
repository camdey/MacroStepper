// ********* Macro Stepper v1.1 *********
// *** AUTHOR: Cam Dey
// *** DATE: 2018-01-27
// **************************************

// Macro Stepper was written to automate the focus stacking procedure commonly used
// in macrophotography. The manual mode allows the user to input the distance to travel
// per step (here called a "Movement") and whether to trigger the camera via shutter cable
// or not. The automatic mode ("AutoStack") will make a specified number of Movements
// based on the user inputted "Start" and "End" points, as well as the distance specified
// per Movement. A shutter delay can also be specified so that there is a pause before each Movement.
// This delay can be used to allow your camera flash time to recharge before the next shot.

// DEFINITIONS:
// STEP = one full step of the stepper stepper motor
// MOVEMENT = one full movement of linear rail by specified distance, consisting of multiple steps
// PROECDURE = one completed stack procedure, consisting of multiple movements
// DISTANCE = distance travelled per movement = number of steps per movement * 0.0025mm
// MANUAL MODE = user input required to step motor by specified distance
// AUTOSTACK = automatic mode, stepper completes procedure without further input

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <AccelStepper.h>
#include <Fonts/Arimo_Regular_10.h>
#include <Fonts/Arimo_Regular_16.h>
#include <Fonts/Arimo_Regular_18.h>
#include <Fonts/Arimo_Regular_20.h>
#include <Fonts/Arimo_Bold_20.h>
#include <Fonts/Arimo_Regular_22.h>
#include <Fonts/Arimo_Regular_24.h>
#include <Fonts/Arimo_Bold_24.h>
#include <Fonts/Arimo_Regular_30.h>
#include <Fonts/Arimo_Bold_30.h>
#include <Fonts/Syncopate_Bold_36.h>
#include <Fonts/Permanent_Marker_Regular_36.h>
#include <Fonts/Lato_Black_34.h>

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// touch screen orientation definitions
#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;

// tft definitions
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Definitions for some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0xE73C
#define iZettleGreenLite  0x9736
#define iZettleGreen      0x4ECC
#define iZettleRed        0xFBCC
#define iZettleRedLite    0xFCD1
#define iZettleBlue       0x4EDE
#define iZettleGrey       0xCE7A
#define iZettleGreyLite   0xDEFB

// touch definitions
#define minPressure 5
#define maxPressure 2000

// The X Stepper pins
#define stepperDirPin 27
#define stepperStepPin 29
#define stepperSleepPin 30
#define stepperEnablePin 41
AccelStepper stepper(AccelStepper::DRIVER, stepperStepPin, stepperDirPin);

#define rearLimitPin 51
#define forwardLimitPin 49
#define xStickPin A12 // analog pin connected to X output
#define yStickPin A13
#define zStickPin A14 //68
#define shutterPin 45

// --- Printing screen functions --- //
void startScreen();
void manualScreen();
void autoScreen();
void autoConfigScreen();
void drawPlayPause(bool greyPlay, bool greyPause);
void drawArrows();
void displayPosition();
void updateProgress(bool screenRefresh);
void estimateDuration(bool screenRefresh);
// void drawGrid();
// --- Touch screen functions --- //
void touchScreen();
void startScreenTouch(TSPoint&);
void manualScreenTouch(TSPoint&);
void autoScreenTouch(TSPoint&);
void autoConfigScreenTouch(TSPoint&);
int arrowsTouch(TSPoint&, bool stepMotor, int val);
// --- Stepper motion functions --- //
void autoStack();
void homeRail();
void dryRun();
void stepperStep(int direction, unsigned long delay);
// --- Enable/Disable functions --- //
void toggleJoystick();
void toggleShutter();
void resetAutoStack();
// --- Inputs and Outputs --- //
void joyStick();
void triggerShutter(bool lowHigh);
void limitSwitch();
// --- Set value functions --- //
void setStepDistance();
void setShutterDelay();
void setAutoStackPositions(bool setStart, bool setEnd);


// --- currentTimes and elapsed times --- //
unsigned long currentTime = 0;        // current time in millis()
unsigned long subRoutine1Time = 0;    // time subroutine1 last ran
unsigned long subRoutine2Time = 0;    // time subroutine2 last ran
unsigned long prevStepTime = 0;       // previous step time reading
unsigned long prevShutterTime = 0;    // previous time shutter triggered
unsigned long prevGenericTime = 0;    // generic timer for toggles and such
unsigned long genericTouchDelay = 200; // 200ms touch delay for toggles
int prevMinutes = 1;                  // duration of autoStack
int prevSeconds = 1;                  // duration of autoStack
char prevTimeMinutesSeconds[6] = "00:00";       // previous duration time

// --- Screen flags --- //
int activeScreen = 1;           // currently displayed screen
bool arrowsActive = 0;          // take arrow touch readings
bool editShutterDelay = 0;      // set shutter delay time
bool editStartPosition = 0;     // set start point for auto mode
bool editEndPosition = 0;       // set end point for auto mode
bool editMovementDistance = 0;  // set step distance in any mode
// --- Input and Output values --- //
int xStickPos = 0;              // ADC value of x-axis joystick
int zStickVal = 1;              // increment count of z-axis button press
int prevZStickVal = 1;          // only increment per button press
int shutterDelay = 3;           // delay between step and shutter trigger
int prevDelay = 3;              // previous delay value
// --- Enable/Disable functionality --- //
bool bootFlag = 0;              // runs rehoming sequence
bool goToStart = 1;             // move to start for autoStack procedure
bool joystickState = 1;         // enabled/disabled
bool autoStackFlag = 0;         // enables function for stack procedure
bool pauseAutoStack = 0;        // pause stack procedure
bool shutterState = 0;          // disabled/enabled
bool stepDue = 0;               // step is due when movement complete
bool targetFlag = 0;            // resets stepper target
// --- Position values --- //
long rearLimitPos = 0;          // limit switch nearest stepper
long forwardLimitPos = 10000;   // limit switch furtherest from stepper
int startPosition = 0;          // start position for stack procedure
int prevStartPosition = 0;
int endPosition = 0;            // end position for stack procedure
int prevEndPosition = 0;
int prevStepperPosition = 1;    // used for showing position of stepper if changed
int manualMovementCount = 0;    // count of manual movements
int prevManualMovementCount = 0;
// --- Stepper motor variables --- //
int stepsPerMovement = 1;       // number of steps required for a given distance
int numMovements = 0;           // equals total distance / step multiplier
int prevNumMovements = 1;       // previous numMovements value
int stepCount = 0;              // number of steps taken in a given movement
double distancePerMovement = 2.50; // distance in micrometres travelled per movement
double prevDistance = 2.50;     // previous step distance in micrometres
int movementProgress = 0;       // number of completed movements
int prevMovementProgress = 1;   // used for overwriting prev movement progress
char prevProgressMovements[10] = "0/0"; // used for overwriting prev movement progress

// 'cog', 50x50px
const unsigned char cogWheel [] PROGMEM = {
	0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x18, 0x07, 0xf8,
	0x06, 0x00, 0x00, 0x00, 0x7e, 0x07, 0xf8, 0x0f, 0x80, 0x00, 0x00, 0xff, 0x0f, 0xfc, 0x3f, 0xc0,
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xff,
	0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x0f, 0xff,
	0x80, 0x00, 0x00, 0x7f, 0xe0, 0x03, 0xff, 0x80, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0xff, 0xff, 0x00,
	0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x7f, 0xff, 0x00, 0x00, 0x3f, 0xff, 0x80, 0xff, 0xfe,
	0x00, 0x00, 0x1f, 0xff, 0xc0, 0xff, 0xfc, 0x00, 0x00, 0x1f, 0xff, 0xc0, 0x3f, 0xfc, 0x00, 0x00,
	0x0f, 0xff, 0x80, 0x1f, 0xfc, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x07, 0xfc,
	0x00, 0x07, 0xf8, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x07,
	0xf8, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x0f, 0xf8, 0x00,
	0x00, 0x0f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x0f,
	0xff, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0xc0, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0xc0,
	0x7f, 0xff, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x3f, 0xff,
	0xc0, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xf0, 0x03, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xfe, 0x1f,
	0xff, 0x80, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0xfe, 0x0f,
	0xfc, 0x3f, 0x80, 0x00, 0x00, 0x7c, 0x07, 0xf8, 0x1f, 0x80, 0x00, 0x00, 0x18, 0x07, 0xf0, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00
};
// 'shutter', 50x50px
const unsigned char aperture [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x30, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff,
	0xbc, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xbe, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xbf, 0x80,
	0x00, 0x00, 0x87, 0xff, 0xff, 0x9f, 0xc0, 0x00, 0x01, 0xf0, 0xff, 0xff, 0x9f, 0xe0, 0x00, 0x03,
	0xfe, 0x1f, 0xff, 0x9f, 0xf0, 0x00, 0x07, 0xff, 0xc7, 0xff, 0xdf, 0xf0, 0x00, 0x0f, 0xff, 0xf1,
	0xff, 0xdf, 0xf8, 0x00, 0x0f, 0xff, 0xfc, 0x7f, 0xdf, 0xfc, 0x00, 0x1f, 0xff, 0xfe, 0x1f, 0xdf,
	0xfc, 0x00, 0x1f, 0xff, 0xf8, 0x07, 0xdf, 0xfe, 0x00, 0x3f, 0xff, 0xe0, 0x01, 0xcf, 0xfe, 0x00,
	0x3f, 0xff, 0x80, 0x00, 0xcf, 0xff, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x7f, 0xfc,
	0x00, 0x00, 0x0f, 0xff, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x7f, 0xe0, 0x00, 0x00,
	0x0f, 0xff, 0x00, 0x7f, 0xcc, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0xff, 0x9c, 0x00, 0x00, 0x0f, 0xfd,
	0x80, 0xfe, 0x7c, 0x00, 0x00, 0x0f, 0xf3, 0x80, 0xfc, 0xfc, 0x00, 0x00, 0x0f, 0xe7, 0x80, 0xf9,
	0xfc, 0x00, 0x00, 0x0f, 0xcf, 0x80, 0xf3, 0xfc, 0x00, 0x00, 0x0f, 0x9f, 0x80, 0xcf, 0xfc, 0x00,
	0x00, 0x0e, 0x7f, 0x80, 0x1f, 0xfc, 0x00, 0x00, 0x0c, 0xff, 0x80, 0x3f, 0xfc, 0x00, 0x00, 0x01,
	0xff, 0x80, 0x7f, 0xfc, 0x00, 0x00, 0x07, 0xff, 0x80, 0x7f, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0x00,
	0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x3f, 0xfd, 0x80, 0x00, 0x7f, 0xff, 0x00, 0x3f, 0xfd,
	0xe0, 0x01, 0xff, 0xfe, 0x00, 0x1f, 0xfd, 0xf0, 0x07, 0xff, 0xfe, 0x00, 0x1f, 0xfc, 0xfc, 0x1f,
	0xff, 0xfc, 0x00, 0x0f, 0xfc, 0xff, 0x1f, 0xff, 0xfc, 0x00, 0x0f, 0xfc, 0xff, 0xc7, 0xff, 0xf8,
	0x00, 0x07, 0xfe, 0xff, 0xf1, 0xff, 0xf8, 0x00, 0x03, 0xfe, 0xff, 0xfc, 0x3f, 0xf0, 0x00, 0x01,
	0xfe, 0xff, 0xff, 0x87, 0xe0, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xf0, 0xc0, 0x00, 0x00, 0x7e, 0x7f,
	0xff, 0xfc, 0x00, 0x00, 0x00, 0x3e, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x1f, 0x7f, 0xff, 0xfc,
	0x00, 0x00, 0x00, 0x07, 0x7f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x01, 0x7f, 0xff, 0xe0, 0x00, 0x00,
	0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00
};
// 'home', 50x42px
const unsigned char house [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x3f, 0x80,
	0x00, 0x00, 0x00, 0x3f, 0xff, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xbf, 0x80, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x03, 0xff,
	0xff, 0xff, 0x80, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff,
	0x80, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00,
	0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x07, 0xff, 0xff, 0xff,
	0xff, 0xf8, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe,
	0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x03,
	0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xff,
	0xff, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff,
	0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00,
	0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff,
	0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f,
	0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0,
	0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'delay', 50x50px
const unsigned char delayClock [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff,
	0xf0, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x07, 0xfe, 0x00,
	0x00, 0x00, 0xff, 0x80, 0x00, 0xff, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x01,
	0xfc, 0x01, 0xc0, 0x1f, 0xc0, 0x00, 0x03, 0xf0, 0x01, 0xc0, 0x07, 0xe0, 0x00, 0x07, 0xe0, 0x01,
	0xc0, 0x03, 0xf0, 0x00, 0x0f, 0xc0, 0x01, 0xc0, 0x01, 0xf8, 0x00, 0x0f, 0x80, 0x01, 0xc0, 0x00,
	0xf8, 0x00, 0x1f, 0x80, 0x01, 0xc0, 0x00, 0xfc, 0x00, 0x1f, 0x00, 0x01, 0xc0, 0x00, 0x7c, 0x00,
	0x3e, 0x00, 0x01, 0xc0, 0x00, 0x7e, 0x80, 0x3e, 0x00, 0x01, 0xc0, 0x00, 0x3f, 0x80, 0x3e, 0x00,
	0x01, 0xc0, 0x00, 0x3f, 0x80, 0x7c, 0x00, 0x01, 0xc0, 0x00, 0xff, 0x80, 0x7c, 0x00, 0x01, 0xc0,
	0x00, 0xff, 0x80, 0x7c, 0x00, 0x01, 0xc0, 0x00, 0x7f, 0x00, 0x7c, 0x00, 0x03, 0xf0, 0x00, 0x3f,
	0x00, 0x78, 0x00, 0x03, 0xf0, 0x00, 0x0f, 0x00, 0x78, 0x00, 0x07, 0xf8, 0x00, 0x07, 0x00, 0x78,
	0x00, 0x07, 0xf8, 0x00, 0x03, 0x00, 0x78, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x03,
	0xfc, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x0f, 0x80,
	0x00, 0x00, 0x7c, 0x00, 0x00, 0x07, 0xc0, 0x10, 0x00, 0x3c, 0x00, 0x00, 0x01, 0xe0, 0x38, 0x00,
	0x3e, 0x00, 0x00, 0x00, 0xc0, 0x3c, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x1f, 0x00,
	0x00, 0x00, 0x00, 0x7c, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x0f, 0x80, 0x00, 0x00,
	0x00, 0xf8, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x03, 0xf0,
	0x00, 0x07, 0xf0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x01,
	0xfe, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0xff, 0x80, 0x00, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xe0,
	0x03, 0xff, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};




// ***** --- PROGRAM --- ***** //

void setup(void) {
  Serial.begin(9600);
  tft.reset();

  pinMode(rearLimitPin, INPUT);
  pinMode(forwardLimitPin, INPUT);
  pinMode(zStickPin, INPUT_PULLUP);

  pinMode(shutterPin, OUTPUT);
  pinMode(stepperSleepPin, OUTPUT);
  digitalWrite(stepperSleepPin, HIGH);
  pinMode(stepperEnablePin, OUTPUT);
  digitalWrite(stepperEnablePin, LOW);

  stepper.setMaxSpeed(1600);
  stepper.setAcceleration(1400);

  uint16_t identifier = tft.readID();

  if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else {
    Serial.print(F("No driver found"));
  }

  tft.setFont(&Arimo_Regular_24);
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(1);

  startScreen();
}

void loop() {
  currentTime = millis();

  // run AutoStack sequence if enabled
  if (autoStackFlag == 1 && pauseAutoStack == 0) {
    autoStack();
  }
  // needs to be called every loop
  stepper.run();
  // take touch reading
  if (currentTime - subRoutine1Time >= 50) {
    touchScreen();
    subRoutine1Time = millis();
  }
  // take joystick and limit switch reading, put stepper to sleep
  if (currentTime - subRoutine2Time >= 100) {
    // check joystick for movement
    xStickPos = analogRead(xStickPin);
    // move if past threshold and not in autoStack mode
    if ((xStickPos >= 600 || xStickPos <= 400) && autoStackFlag == 0) {
      joyStick();
    }
    // check limit switches
    if (digitalRead(rearLimitPin) + digitalRead(forwardLimitPin) < 2) {
      limitSwitch();
    }
    // sleep if stepper inactive, update position on manual screen
    if (stepper.isRunning() == 0 && (autoStackFlag == 0 || pauseAutoStack == 1) && digitalRead(stepperSleepPin) == HIGH) {
      digitalWrite(stepperSleepPin, LOW);
      // refresh position on manual screen after stepping completed
      if (prevStepperPosition != stepper.currentPosition() && activeScreen == 2) {
        displayPosition();
      }
    }
    // only increment per button click
    if (digitalRead(zStickPin) == HIGH) {
      prevZStickVal = zStickVal;
    }
    // increment joystick button for changing stepper speed
    if (digitalRead(zStickPin) == LOW && prevZStickVal == zStickVal) {
      zStickVal++;
      if (zStickVal > 3) {
        zStickVal = 1;
      }
    }

    subRoutine2Time = millis();
  }
  // reset target to currentPosition
  if (targetFlag == 1) {
    stepper.move(0);
    stepper.setSpeed(0);
    targetFlag = 0;
  }
  // run homing sequence if first loop
  if (bootFlag == 0) {
    bootFlag = 1;
    // homeRail();
  }
}

void touchScreen() {
  currentTime = millis(); // call frequently just in case

  TSPoint point = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (point.z > minPressure && point.z < maxPressure) {
    switch (activeScreen) {
      case 1:
        startScreenTouch(point); // main screen
        break;
      case 2:
        manualScreenTouch(point); // screen for manual mode
        break;
      case 3:
        autoScreenTouch(point); // screen for autoStack
        break;
      case 4:
        autoConfigScreenTouch(point); // autoStack config
        break;
    }
  }
}

void startScreen() {
  activeScreen = 1;
  tft.fillScreen(BLACK);

  tft.setTextColor(WHITE);
  tft.setCursor(30, 45);
  tft.setFont(&Permanent_Marker_Regular_36);
  tft.println("MacroStepper");

  tft.setTextColor(BLACK);
  // Manual option
  tft.fillRoundRect(20, 90, 130, 60, 8, iZettleRed);
  tft.setCursor(30,130);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Manual");

  // Auto option
  tft.fillRoundRect(170, 90, 130, 60, 8, iZettleGreen);
  tft.setCursor(200,130);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Auto");

  // Homing option
  // tft.fillRect(0, 200, 320, 40, iZettleBlue);
  tft.drawBitmap(20, 190, house, 50, 42, iZettleGreen);

  // Clear autoStack
  tft.setCursor(235,225);
  tft.setFont(&Arimo_Bold_20);
  if (autoStackFlag == 0) {
    tft.setTextColor(iZettleGreen, BLACK);
  } else if (autoStackFlag == 1) {
    tft.setTextColor(iZettleRed, BLACK);
  }
  tft.println("Reset");
}

void startScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // manual box - tft.fillRoundRect(20, 90, 130, 60, 8, iZettleRed);
  if ((xPos >= 20 && xPos <= 150) && (yPos >= 90 && yPos <= 150)) {
    manualScreen();
  }
  // auto box - tft.fillRoundRect(170, 90, 130, 60, 8, iZettleGreen);
  if ((xPos >= 170 && xPos <= 300) && (yPos >= 90 && yPos <= 150)) {
    autoScreen();
  }
  // homing func - tft.setCursor(20,225);
  if ((xPos >= 20 && xPos <= 120) && (yPos >= 200 && yPos <= 240)) {
    tft.drawBitmap(20, 190, house, 50, 42, iZettleRed);
    homeRail();
    tft.drawBitmap(20, 190, house, 50, 42, iZettleGreen);
  }
  // clear autoStack run
  if ((xPos >= 230 && xPos <= 320) && (yPos >= 200 && yPos <= 240) && autoStackFlag == 1) {
    resetAutoStack(); // reset autostack settings
  }
}

void manualScreen() {
  activeScreen = 2;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  // tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 5, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");

  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Step Number
  // tft.fillRoundRect(5, 85, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 85, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");

  tft.setCursor(30, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println(manualMovementCount);

  // Rail Posoition
  // tft.fillRoundRect(5, 165, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 165, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Rail Pos.");
  tft.setCursor(5, 220);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(stepper.currentPosition()*0.0025, 4));

  // auto shutter
  if (shutterState == 1) {
    tft.drawBitmap(155, 15, aperture, 50, 50, iZettleGreen);
  } else if (shutterState == 0) {
    tft.drawBitmap(155, 15, aperture, 50, 50, iZettleRed);
  }

  // reset steps
  tft.setCursor(150, 125);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(iZettleRed);
  tft.println("Reset");

  // back to main screen
  tft.setCursor(150, 205);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  drawArrows();
}

void manualScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // toggle shutter on/off
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 10 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      if (shutterState == 1) {
        tft.drawBitmap(155, 15, aperture, 50, 50, iZettleGreen);
      } else if (shutterState == 0) {
        tft.drawBitmap(155, 15, aperture, 50, 50, iZettleRed);
      }

      prevGenericTime = millis();
    }
  }
  // reset manual movement count
  if ((xPos >= 145 && xPos <= 225) && (yPos >= 120 && yPos <= 140)) {
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(manualMovementCount), 30, 140, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, iZettleBlue);

    manualMovementCount = 0;
    prevManualMovementCount = 0;
    tft.setTextColor(WHITE);
    tft.setCursor(30, 140);
    tft.println(manualMovementCount);
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == 0 && editMovementDistance == 0) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }
      if (arrowsActive == 1 && editMovementDistance == 1) {
        tft.setTextColor(YELLOW);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }

      prevGenericTime = millis();
    }
  }
  // set step size
  if (arrowsActive == 1 && editMovementDistance == 1) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(point, 0, stepsPerMovement);
    setStepDistance();
  }
  // step motor by specified distance
  if (arrowsActive == 0) {
    int val = arrowsTouch(point, 1, 0);
  }
  // back button
  if ((xPos >= 145 && xPos <= 200) && (yPos >= 190 && yPos <= 210) && arrowsActive == 0) {
    startScreen();
  }
}

void autoScreen() {
  activeScreen = 3;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  tft.fillRoundRect(-10, 5, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");

  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Estimated time remaining
  tft.fillRoundRect(-10, 85, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Est. Time");
  // show estimarted time
  estimateDuration(1);

  // Step progress
  tft.fillRoundRect(-10, 165, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Progress");
  // show progress, force update due to screen change
  updateProgress(1);

  // auto shutter
  if (shutterState == 1) {
    tft.drawBitmap(155, 15, aperture, 50, 50, iZettleGreen);
  } else if (shutterState == 0) {
    tft.drawBitmap(155, 15, aperture, 50, 50, iZettleRed);
  }

  // enter auto config
  tft.drawBitmap(155, 95, cogWheel, 50, 50, WHITE);

  // back to main screen
  tft.setCursor(150, 205);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  if (pauseAutoStack == 1 && autoStackFlag == 1) {
    drawPlayPause(0, 1);
  } else if (pauseAutoStack == 0 && autoStackFlag == 1) {
    drawPlayPause(1, 0);
  } else if (pauseAutoStack == 0 && autoStackFlag == 0) {
    drawPlayPause(0, 0);
  }
}

void autoScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // shutter toggle
  if ((xPos >= 140 && xPos <= 210) && (yPos >= 10 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      if (shutterState == 1) {
        tft.drawBitmap(155, 15, aperture, 50, 50, iZettleGreen);
      } else if (shutterState == 0) {
        tft.drawBitmap(155, 15, aperture, 50, 50, iZettleRed);
      }
      prevGenericTime = millis();
    }
  }
  // config button
  if ((xPos >= 140 && xPos <= 210) && (yPos >= 80 && yPos <= 150)) {
    autoConfigScreen();
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == 0 && editMovementDistance == 0) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }
      if (arrowsActive == 1 && editMovementDistance == 1) {
        tft.setTextColor(YELLOW);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }

      prevGenericTime = millis();
    }
  }
  // set step size
  if (arrowsActive == 1 && editMovementDistance == 1) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(point, 0, stepsPerMovement);
    setStepDistance();
  }
  // run AutoStack sequence
  if (arrowsActive == 0 && (xPos >= 230 && xPos <= 310) && (yPos >= 30 && yPos <= 120)) {
    autoStackFlag = 1;
    pauseAutoStack = 0; // re-enable autoStack + pause button
    drawPlayPause(1, 0); // grey out play, recolour pause button
  }
  // pause/resume AutoStack sequence
  if (arrowsActive == 0 && pauseAutoStack == 0 && autoStackFlag == 1 && (xPos >= 230 && xPos <= 310) && (yPos >= 135 && yPos <= 225)) {
    pauseAutoStack = 1;
    drawPlayPause(0, 1); // recolour play, grey out pause button
  }
  // back button
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 190 && yPos <= 210) && arrowsActive == 0) {
    startScreen();
  }
}

void autoConfigScreen() {
  activeScreen = 4;
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // set start point
  tft.setCursor(20, 50);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleGreen);
  tft.println("START");
  tft.setCursor(35, 65);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println(startPosition);

  // Set end point
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleRed);
  tft.println("END");
  tft.setCursor(35, 145);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println(endPosition);

  // Run from start to end and return
  // move back from end point to start point (with accel)
  // then run from start to end with no accel
  tft.setCursor(20, 210);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleBlue);
  tft.println("RUN");
  tft.setCursor(35, 225);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE, BLACK);
  tft.println(stepper.currentPosition());

  // set delay
  tft.drawBitmap(160, 70, delayClock, 50, 50, WHITE);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(155, 143);
  tft.println(shutterDelay);
  tft.setCursor(175, 143);
  tft.println(" sec");

  // back to auto screen
  tft.setCursor(150, 205);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  drawArrows();
}

void autoConfigScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // start button
  if ((xPos >= 10 && xPos <= 140) && (yPos >= 10 && yPos <= 80) && (editShutterDelay + editEndPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editStartPosition = !editStartPosition;

      if (arrowsActive == 0 && editStartPosition == 0) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 24, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editStartPosition == 1) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 24, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set start position
  if (editStartPosition == 1 && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(1, 0); //set start but not end position
  }
  // end button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && (editShutterDelay + editStartPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editEndPosition = !editEndPosition;

      if (arrowsActive == 0 && editEndPosition == 0) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 104, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editEndPosition == 1) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 104, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set end position
  if (editEndPosition == 1 && arrowsActive == 1) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }

    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(0, 1); //set end but not start position
  }
  // run button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 170 && yPos <= 240) && (editShutterDelay + editStartPosition + editEndPosition) == 0) {
    tft.fillRoundRect(10, 184, 5, 35, 4, YELLOW);
    dryRun();
    tft.fillRoundRect(10, 184, 5, 35, 4, BLACK);
  }
  // delay button
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 60 && yPos <= 130) && (editStartPosition + editEndPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editShutterDelay = !editShutterDelay;

      if (arrowsActive == 0 && editShutterDelay == 0) {
        tft.drawBitmap(160, 70, delayClock, 50, 50, WHITE);
      }
      if (arrowsActive == 1 && editShutterDelay == 1) {
        tft.drawBitmap(160, 70, delayClock, 50, 50, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set shutter delay
  if (editShutterDelay == 1 && arrowsActive == 1) {
    if (prevDelay != shutterDelay) {
      prevDelay = shutterDelay;
    }
    shutterDelay = arrowsTouch(point, 0, shutterDelay);
    setShutterDelay();
  }
  // show current position of stepper
  displayPosition();
  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208) && arrowsActive == 0) {
    autoScreen();
  }

  Serial.print("editStartPosition: ");
  Serial.println(editStartPosition);
  Serial.print("editEndPosition: ");
  Serial.println(editEndPosition);
  Serial.print("editShutterDelay: ");
  Serial.println(editShutterDelay);
  Serial.print("arrowsActive: ");
  Serial.println(arrowsActive);
}

void drawArrows() {
  tft.fillTriangle(230, 65, 260, 15, 290, 65, iZettleGreen);
  tft.fillRoundRect(245, 69, 30, 36, 4, iZettleGreen);

  tft.fillTriangle(230, 175, 260, 225, 290, 175, iZettleRed);
  tft.fillRoundRect(245, 135, 30, 36, 4, iZettleRed);
}

void drawPlayPause(bool greyPlay = 0, bool greyPause = 0) {
  if (greyPlay == 1) {
    tft.fillCircle(270, 75, 35, iZettleGreyLite);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  } else {
    tft.fillCircle(270, 75, 35, iZettleGreen);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  }

  if (greyPause == 1) {
    tft.fillCircle(270, 180, 35, iZettleGreyLite);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  } else {
    tft.fillCircle(270, 180, 35, iZettleBlue);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  }
}

int arrowsTouch(TSPoint &point, bool stepMotor, int val = 0) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  if ((xPos >= 230 && xPos <= 290) && (yPos >= 15 && yPos <= 105)) {
    val++;
    if (stepMotor == 1) { // if stepping required, move motor
      if (shutterState == 1 && activeScreen == 2) {
        triggerShutter(1);
        delay(300);
        triggerShutter(0);
      }
      Serial.print("forward: ");
      Serial.println(stepper.targetPosition());
      stepperStep(1, 500); // forward
    }
  }
  if ((xPos >= 230 && xPos <= 290) && (yPos >= 135 && yPos <= 225)) {
    val--;
    if (stepMotor == 1) { // if stepping required, move motor
      Serial.print("backward: ");
      Serial.println(stepper.targetPosition());
      stepperStep(-1, 500); // reverse
    }
  }

  return val;
}

void autoStack() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }
  // move stepper to start if not already there
  if (goToStart == 1) {
    while (stepper.currentPosition() != startPosition) {
      // move to start
      stepper.setSpeed(-600);
      stepper.moveTo(startPosition);
      stepper.runSpeedToPosition();

      goToStart = 0;
      stepCount = 0;
      stepDue = 1;
      movementProgress = 0;
      joystickState = 0;
    }
    stepper.move(0);
    stepper.setSpeed(0);
  }
  // take photo and increment progress for first step of each movement
  if (stepCount == 0 && stepDue == 1) {
    triggerShutter(1); // set trigger to high
    // delay(300);
    movementProgress++;
    // make sure correct screen is displaying
    if (activeScreen == 3) { // auto screen
      updateProgress(0); // don't force refresh
    }
    // prev movement progress value
    prevMovementProgress = movementProgress;
  }
  // keep stepping if not at end and haven't travelled a full movement
  if (movementProgress <= numMovements && stepDue == 1 && stepCount < stepsPerMovement) {
    stepperStep(1, 1); // forward direction, 1ms delay
    stepCount++;
  }
  // reset stepCount for next movement, estimate duration
  if (stepCount >= stepsPerMovement) {
    estimateDuration(0); // don't force refresh
    // only reset after 400ms to give time for shutter release unless shutter disabled
    if (shutterState == 1 && currentTime - prevShutterTime >= 400) {
      stepCount = 0;
      stepDue = 0;
      prevShutterTime = millis();
      // reset trigger by setting low
      triggerShutter(0);
    } else if (shutterState == 0) {
      stepCount = 0;
      stepDue = 0;
    }
  }
  // wait delay period before re-enabling stepping
  if (stepDue == 0) {
    stepperStep(0, shutterDelay*1000); // don't step and wait for delay
  }
  // stop AutoStack sequence if end reached
  if (movementProgress >= numMovements) {
    autoStackFlag = 0;
    goToStart = 1;
    movementProgress = 0;
    joystickState = 1;
    drawPlayPause(0, 0); // reset play to green
    triggerShutter(0); // reset trigger by setting low
  }
}

void homeRail() {
  // wake stepper from sleep
  if(digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }

  while (digitalRead(rearLimitPin) == 1) { // start in reverse
    stepper.moveTo(-100000);
    stepper.run();
  }
  // stops motor instantly
  stepper.setSpeed(0);
  rearLimitPos = 0;
  stepper.setCurrentPosition(0); // set position

  while (digitalRead(forwardLimitPin) == 1) { // move to forward limit
    stepper.moveTo(100000);
    stepper.run();
  }
  // stops motor instantly
  stepper.setSpeed(0);
  // set forward position, move back to middle of rail
  forwardLimitPos = stepper.currentPosition(); // set position
  stepper.moveTo(forwardLimitPos / 2); // return to the middle
  stepper.runToPosition();
}

void dryRun() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }

  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }

  displayPosition();

  while (stepper.currentPosition() < endPosition) {
    stepper.setSpeed(300);
    stepper.moveTo(endPosition);
    stepper.runSpeedToPosition();
  }

  displayPosition();

  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }

  // stops motor instantly
  stepper.setSpeed(0);
  targetFlag = 1;
  movementProgress = 0;
  displayPosition();
}

void stepperStep(int stepDirection, unsigned long delay) {
  currentTime = millis();
  // step after elapsed amount of time
  if (currentTime - prevStepTime > delay) {
    // wake stepper from sleep
    if (digitalRead(stepperSleepPin) == LOW) {
      digitalWrite(stepperSleepPin, HIGH);
    }

    int stepVelocity = stepDirection * stepsPerMovement;
    Serial.println(stepVelocity);
    Serial.println(40*stepDirection);

    stepper.move(stepVelocity);
    stepper.setSpeed(40*stepDirection);
    stepper.runSpeed();

    stepDue = 1;
    prevStepTime = currentTime;
  }
}

void joyStick() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }
  // while joystick is operated
  while ((xStickPos >= 600 || xStickPos <= 400) && joystickState == 1) {
    // move either -1, 0, or 1
    stepper.move(map(xStickPos, 0, 1023, -1, 1));
    stepper.runSpeed();

    // check stick position again
    xStickPos = analogRead(xStickPin);
    // reset stepper target
    targetFlag = 1;

    // check limit switches
    if (digitalRead(rearLimitPin) + digitalRead(forwardLimitPin) < 2) {
      stepper.stop();
      break;
    }

    // set speed based on joystick input
    int stepperSpeed = 600;
    switch (zStickVal) {
      case 1:
        stepperSpeed = 800;
        break;
      case 2:
        stepperSpeed = 600;
        break;
      case 3:
        stepperSpeed = 300;
        break;
    }

    // set speed direction
    if (xStickPos <= 400) {
      stepper.setSpeed(stepperSpeed*-1);
    }
    if (xStickPos >= 600) {
      stepper.setSpeed(stepperSpeed);
    }
  }

  // // check start/end position adjustment
  if (editStartPosition == 1 && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    setAutoStackPositions(1, 0); //set start but not end position
  }
  if (editEndPosition == 1 && arrowsActive == 1) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }
    setAutoStackPositions(0, 1); //set end but not start position
  }
  if (activeScreen == 2 or activeScreen == 4) {
    displayPosition();
  }
  // update prev position
  prevStepperPosition = stepper.currentPosition();
}

void toggleJoystick() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    joystickState = !joystickState;

    prevGenericTime = millis();
  }
}

void toggleShutter() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    shutterState = !shutterState;
    prevGenericTime = millis();
  }
}

void resetAutoStack() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    autoStackFlag = 0;
    goToStart = 1;
    joystickState = 1;
    pauseAutoStack = 0;
    stepDue = 1;
    stepCount = 1;
    movementProgress = 0;
    numMovements = 0;
    startPosition = 0;
    endPosition = 0;
    tft.setCursor(235,225);
    tft.setFont(&Arimo_Bold_20);
    tft.setTextColor(iZettleGreen, BLACK);
    tft.println("Reset");

    prevGenericTime = millis();
  }
}

void setShutterDelay() {

  if (shutterDelay < 1) {
    shutterDelay = 1;
  } else if (shutterDelay > 15) {
    shutterDelay = 15;
  }

  if (prevDelay != shutterDelay) {
    // print new delay value
    tft.setCursor(155, 143);
    tft.setFont(&Arimo_Regular_20);
    tft.setTextColor(BLACK, BLACK);
    tft.println(prevDelay);
    tft.setCursor(155, 143);
    tft.setTextColor(WHITE, BLACK);
    tft.println(shutterDelay);
  }
}

void triggerShutter(bool lowHigh) {
  if (shutterState == 1 && lowHigh == 1) {
    digitalWrite(shutterPin, HIGH);
  } else if (lowHigh == 0) {
    digitalWrite(shutterPin, LOW);
  }
}

void setStepDistance() {

  // constrain multiplier range
  if (stepsPerMovement < 1) {
    stepsPerMovement = 1;
  } else if (stepsPerMovement > 40) {
    stepsPerMovement = 40;
  }

  // stepper is 200 steps/rev, linear rail has 1mm pitch, 1/2 gear reduction
  // 1 step = 1/200/2 = 0.0025mm or 2.5μm
  distancePerMovement = 2.50 * stepsPerMovement;

  if (prevDistance != distancePerMovement) {
    // print new delay value
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(prevDistance/1000, 4), 10, 60, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, iZettleBlue);
    tft.setCursor(10, 60);
    tft.setTextColor(YELLOW, iZettleBlue);
    tft.println(String(distancePerMovement/1000, 4));

    // calculate number of steps required to cover range
    numMovements = ceil((endPosition - startPosition)*1.00/stepsPerMovement);

    if (activeScreen == 3) {
      // update estimated time
      estimateDuration(0); // don't force refresh
      // update progress
      updateProgress(0); // don't force refresh
    }

    prevNumMovements = numMovements;
  }
}

void setAutoStackPositions(bool setStart, bool setEnd) {
  if (setStart == 1) {
    // lower limit
    if (startPosition < 0) {
      startPosition = 0;
    }
    // get new value
    startPosition = stepper.currentPosition();
    // print start point if changed
    if (prevStartPosition != startPosition) {
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(String(prevStartPosition), 35, 65, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 65);
      tft.setFont(&Arimo_Regular_16);
      tft.setTextColor(WHITE, BLACK);
      tft.println(startPosition);
    }
    // reset macroStepping
    goToStart = 1;
  }

  if (setEnd == 1) {
    // lower limit
    if (endPosition < 0) {
      endPosition = 0;
    }
    // get new value
    endPosition = stepper.currentPosition();
    // print end point if changed
    if (prevEndPosition != endPosition) {
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(String(prevEndPosition), 35, 145, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 145);
      tft.setFont(&Arimo_Regular_16);
      tft.setTextColor(WHITE, BLACK);
      tft.println(endPosition);
    }
  }

  // calculate number of steps required to cover range
  numMovements = ceil((endPosition - startPosition)/stepsPerMovement);
  prevNumMovements = numMovements;
}

void displayPosition() {
  // update for new values
  if (prevStepperPosition != stepper.currentPosition()) {
    int16_t x1, y1;
    uint16_t w, h;

    if (activeScreen == 2) { // 2 = manual screen
      // update rail position value
      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevStepperPosition*0.0025, 4), 5, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setCursor(5, 220);
      tft.setTextColor(WHITE, iZettleBlue);
      tft.println(String(stepper.currentPosition()*0.0025, 4));

      // update movement count for manual screen
      manualMovementCount++;

      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevManualMovementCount), 30, 140, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setTextColor(WHITE);
      tft.setCursor(30, 140);
      tft.println(manualMovementCount);

      prevManualMovementCount = manualMovementCount;
    }

    if (activeScreen == 4) { // 4 = autoConfigScreen
      tft.setFont(&Arimo_Regular_16);
      tft.getTextBounds(String(prevStepperPosition), 35, 225, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 225);
      tft.setTextColor(WHITE, BLACK);
      tft.println(stepper.currentPosition());
    }

    prevStepperPosition = stepper.currentPosition();
  }
}

void estimateDuration(bool screenRefresh) {
  int duration = numMovements * shutterDelay;
  int elapsed = movementProgress * shutterDelay;
  int remaining = duration - elapsed;
  int minutes = remaining / 60;
  int seconds = remaining % 60;
  char timeMinutesSeconds[5];

  int16_t x, y;
  uint16_t w, h;
  tft.setFont(&Arimo_Bold_30);
  tft.setTextColor(WHITE);
  sprintf_P(timeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);

  if ((minutes != prevMinutes || seconds != prevSeconds || screenRefresh == 1) && activeScreen == 3) {
    tft.getTextBounds(String(prevTimeMinutesSeconds), 15, 140, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, iZettleBlue);
    tft.setCursor(15, 140);
    tft.println(timeMinutesSeconds);

    prevMinutes = minutes;
    prevSeconds = seconds;
    // assign new time to prev variable
    sprintf_P(prevTimeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
  }
}

void updateProgress(bool screenRefresh) {
  char progressMovements[10];

  int16_t x, y;
  uint16_t w, h;
  sprintf_P(progressMovements, PSTR("%02d/%02d"), movementProgress, numMovements);

  if ((movementProgress != prevMovementProgress) || numMovements != prevNumMovements || screenRefresh == 1) {
    tft.getTextBounds(String(prevProgressMovements), 20, 220, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, iZettleBlue);
    tft.setFont(&Arimo_Bold_24);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 220);
    tft.println(progressMovements);

    prevNumMovements = numMovements;
    prevMovementProgress = movementProgress;
    // assign new time to prev variable
    sprintf_P(prevProgressMovements, PSTR("%02d:%02d"), movementProgress, numMovements);
  }
}

void limitSwitch() {
  // wake stepper from sleep
  if(digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }

  while (digitalRead(rearLimitPin) == 0) { // switch closest to motor
    rearLimitPos = stepper.currentPosition();
    // move by 1 step until limit no longer triggered
    stepper.move(1);
    stepper.setSpeed(600);
    stepper.runSpeed();
    // reset stepper target
    targetFlag = 1;
  }

  while (digitalRead(forwardLimitPin) == 0) { //switch furtherest from motor
    forwardLimitPos = stepper.currentPosition();
    // move by -1 step until limit no longer triggered
    stepper.move(-1);
    stepper.setSpeed(-600);
    stepper.runSpeed();
    // reset stepper target
    targetFlag = 1;
  }
}
