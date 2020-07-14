#ifndef VARIABLEDECLARATIONS_H
#define VARIABLEDECLARATIONS_H

#include <Arduino.h>
// #include <TMC2130Stepper.h>
#include "TMCStepper.h"
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <gfxButton.h>
// #include <AccelStepper.h>
#include <Fonts/Arimo_Regular_10.h>
#include <Fonts/Arimo_Regular_16.h>
#include <Fonts/Arimo_Regular_18.h>
#include <Fonts/Arimo_Regular_20.h>
#include <Fonts/Arimo_Regular_22.h>
#include <Fonts/Arimo_Regular_24.h>
#include <Fonts/Arimo_Regular_30.h>
#include <Fonts/Arimo_Regular_36.h>
#include <Fonts/Arimo_Regular_40.h>
#include <Fonts/Arimo_Bold_20.h>
#include <Fonts/Arimo_Bold_24.h>
#include <Fonts/Arimo_Bold_30.h>
#include <Fonts/Arimo_Bold_36.h>
#include <Fonts/Arimo_Bold_40.h>
#include <Fonts/Syncopate_Bold_36.h>
#include <Fonts/Permanent_Marker_Regular_36.h>
#include <Fonts/Lato_Black_34.h>
#include <Fonts/Lato_Black_40.h>
#include <Icons/ArrowUp.h>
#include <Icons/ArrowDown.h>
#include <Icons/BackArrow.h>
#include <Icons/Cancel.h>
#include <Icons/CogWheel.h>
#include <Icons/DelayClock.h>
#include <Icons/FlashBulb.h>
#include <Icons/FlashOn.h>
#include <Icons/FlashOff.h>
#include <Icons/MainLogo.h>
#include <Icons/Play.h>
#include <Icons/Pause.h>
#include <Icons/Rotate.h>
#include <Icons/Target.h>
#include <Icons/Timer.h>

extern TouchScreen      ts;
extern TMC5160Stepper   driver;
extern MCUFRIEND_kbv 	  tft;
extern gfxButton        gfxB;
extern gfxTouch         gfxT;

// Definitions for some common 16-bit color values:
#define	BLACK   						0x0000
#define	BLUE    						0x001F
#define	RED     						0xF800
#define	GREEN   						0x07E0
#define CYAN    						0x07FF
#define MAGENTA 						0xF81F
#define YELLOW  						0xFFE0
#define WHITE   						0xFFFF
#define GRAY    						0xE73C
#define DARKGRAY						0x39E8
#define CUSTOM_GREEN_LITE		0x9736
#define CUSTOM_GREEN				0x4ECC
#define CUSTOM_RED					0xFBCC
#define CUSTOM_RED_LITE			0xFCD1
#define CUSTOM_BLUE					0x4EDE
#define CUSTOM_GREY					0xCE7A
#define CUSTOM_GREY_LITE		0xDEFB
#define CUSTOM_YELLOW       0xFF46

// touch screen pressure values
#define minPressure 			5
#define maxPressure 			2000
// microsteps and positions
#define nrMicrosteps			16						// number of microsteps per full step
#define microstepDistance 1.25          // distance travelled per microstep
#define railLength        200           // total length of actuator rails
#define stageLength       60            // total length of rail stage
#define endStoppers       18            // screw bushing protrudes and prevents stage from fully reaching rear end
#define railTravel        120           // 150mm rail length minus 75mm stage length minus 1.9mm for bushing overhang
#define minPosition       0
#define maxPosition       384000        // (120mm travel / 2mm pitch) * 400 steps * 16 microsteps

// definitions for touch screen orientation
// Arduino Due + 2.8" TFT
// #define TS_MINX 					160
// #define TS_MAXX 					850
// #define TS_MINY 					132
// #define TS_MAXY 					900
// Arduino Due + 3.5" TFT
#define TS_MINX 					846
#define TS_MAXX 					148
#define TS_MINY 					937
#define TS_MAXY 					140
// Grand Central M4 + 2.8" TFT
// #define TS_MINX 					320
// #define TS_MAXX 					760
// #define TS_MINY 					235
// #define TS_MAXY 					810
// grand central M4 + 3.5" TFT
// #define TS_MINX 					760
// #define TS_MAXX 					315
// #define TS_MINY 					852
// #define TS_MAXY 					220

// pin definitions for touch inputs
#define YP 								A3 						// must be an analog pin, use "An" notation!
#define XM 								A2            // must be an analog pin, use "An" notation!
#define YM 								9             // can be a digital pin
#define XP 								8             // can be a digital pin

// pin definitions for tft screen
#define LCD_CS 						A3
#define LCD_CD 						A2
#define LCD_WR 						A1
#define LCD_RD						A0
#define LCD_RESET 				A4

// driver pins
#define DIAG1_PIN 				24
#define DIAG0_PIN 				26
#define DIR_PIN 					28
#define STEP_PIN 					30
#define CS_PIN 						32
#define EN_PIN 						34
#define R_SENSE           0.075f       // Watterott TMC5160 uses 0.075
// handled by SPI library
// #define MOSI_PIN 				ICSP4
// #define MISO_PIN 				ICSP1
// #define SCK_PIN 					ICSP3

// misc hardware pins
#define XSTICK_PIN 				A6            // joystick X-axis pin (controls fwd and rev)
#define ZSTICK_PIN 				A7            // button-press from joystick
#define GODOX_PIN 				A8            // pin for GA1A12S202 light sensor
#define PIEZO_PIN         22            // pin for Piezo buzzer
#define SONY_PIN          36            // pin for pulling camera focus and shutter to GND via opto

// --- currentTimes and elapsed times --- //
extern unsigned long currentTime;
extern unsigned long prevButtonCheck;
extern unsigned long prevJoystickCheck;
extern unsigned long prevStepTime;
extern unsigned long recycleTime;
extern unsigned long prevGenericTime;
extern unsigned long genericTouchDelay;
extern unsigned long lastReadFlash;
extern int prevMinutes;
extern int prevSeconds;
extern char prevTimeMinutesSeconds[6];

// --- Screen flags --- //
extern int activeScreen;
extern bool arrowsActive;
extern bool editShutterDelay;
extern bool editStartPosition;
extern bool editEndPosition;
extern bool editMovementDistance;
extern bool editFlashOnValue;
extern bool editFlashOffValue;
extern bool testFlash;
extern bool screenRotated;
// --- Input and Output values --- //
extern int xStickPos;
extern int zStickVal;
extern int xPosUpper;
extern int xPosDiff;
extern int xPosResting;
extern int xPosLower;
extern int prevZStickVal;
extern int shutterDelay;
extern int prevDelay;
extern int joyStickSpeed;
extern int flashValue;
extern int prevFlashValue;
extern int flashThreshold;
extern int prevFlashThreshold;
extern int flashOnValue;
extern int flashOffValue;
// --- Enable/Disable functionality --- //
extern bool runHomingSequence;
extern bool homedRail;
extern bool goToStart;
extern bool joystickState;
extern bool autoStackFlag;
extern bool pauseAutoStack;
extern bool shutterEnabled;
extern bool targetFlag;
extern bool flashReady;
extern bool stallGuardConfigured;
extern bool autoStackMax;
// --- Position values --- //
extern long startPosition;
extern long prevStartPosition;
extern long endPosition;
extern long prevEndPosition;
extern long prevStepperPosition;
extern int manualMovementCount;
extern int prevManualMovementCount;
extern volatile long moveDist;
extern volatile bool stepperDisabled;
extern volatile bool directionFwd;
extern volatile long fwdPosition;
extern volatile long bwdPosition;
extern bool firstFwdStall;
// --- Stepper motor variables --- //
extern int stepsPerMovement;
extern int movementsRequired;
extern int prevMovementsRequired;
extern int stepCount;
extern double distancePerMovement;
extern double prevDistance;
extern int completedMovements;
extern int prevCompletedMovements;
extern char prevAutoStackProgress[10];
extern bool stepperMoved;
extern bool shutterTriggered;
extern bool triggerFailed;

extern int joystickMaxVelocity;
extern int stepperMaxSpeed;
extern int rampSteps;

extern String currentScreen;
extern String stepDist;

#endif
