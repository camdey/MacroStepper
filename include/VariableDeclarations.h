#ifndef VARIABLEDECLARATIONS_H
#define VARIABLEDECLARATIONS_H

#include <Arduino.h>
#include <TMC2130Stepper.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <AccelStepper.h>
#include <Fonts/Arimo_Regular_10.h>
#include <Fonts/Arimo_Regular_16.h>
#include <Fonts/Arimo_Regular_18.h>
#include <Fonts/Arimo_Regular_20.h>
#include <Fonts/Arimo_Regular_22.h>
#include <Fonts/Arimo_Regular_24.h>
#include <Fonts/Arimo_Regular_30.h>
#include <Fonts/Arimo_Bold_20.h>
#include <Fonts/Arimo_Bold_24.h>
#include <Fonts/Arimo_Bold_30.h>
#include <Fonts/Syncopate_Bold_36.h>
#include <Fonts/Permanent_Marker_Regular_36.h>
#include <Fonts/Lato_Black_34.h>
#include <Icons/Aperture_Icon.h>
#include <Icons/Back_Arrow_Icon.h>
#include <Icons/Cog_Wheel_Icon.h>
#include <Icons/Delay_Clock_Icon.h>
#include <Icons/House_Icon.h>
#include <Icons/Main_Logo_Icon.h>
#include <Icons/Reset_Icon.h>
#include <Icons/Reset_40_Icon.h>

extern TouchScreen      ts;
extern TMC2130Stepper   driver;
extern AccelStepper 		stepper;
extern MCUFRIEND_kbv 	  tft;

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

// touch screen pressure values
#define minPressure 			5
#define maxPressure 			2000
// microstep values
#define nrMicrosteps			4								// number of microsteps per full step
#define microstepDistance 1.25						// distance travelled per microstep

// definitions for touch screen orientation
#define TS_MINX 					100
#define TS_MAXX 					920
#define TS_MINY 					70
#define TS_MAXY 					920

// pin definitions for touch inputs
#define YP 								A3 						// must be an analog pin, use "An" notation!
#define XM 								A2 						// must be an analog pin, use "An" notation!
#define YM 								9 						// can be a digital pin
#define XP 								8 						// can be a digital pin

// pin definitions for tft screen
#define LCD_CS 						A3
#define LCD_CD 						A2
#define LCD_WR 						A1
#define LCD_RD						A0
#define LCD_RESET 				A4

// driver pins
#define DIR_PIN 					36
#define STEP_PIN 					34
#define EN_PIN 						42
#define DIAG_PIN 					18
#define CS_PIN 						53
#define MOSI_PIN 					51
#define MISO_PIN 					52
#define SCK_PIN 					52

// misc hardware pins
#define XSTICK_PIN 				A9 					// joystick X-axis pin (controls fwd and rev)
#define ZSTICK_PIN 				A8 					// button-press from joystick
#define FLASH_PIN 				A5 					// pin for light sensor
#define SHUTTER_PIN 			30 					// pin for pulling camera focus and shutter to GND via transistor

// --- currentTimes and elapsed times --- //
extern unsigned long currentTime;
extern unsigned long subRoutine1Time;
extern unsigned long subRoutine2Time;
extern unsigned long prevStepTime;
extern unsigned long recycleTime;
extern unsigned long prevGenericTime;
extern unsigned long genericTouchDelay;
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
// --- Input and Output values --- //
extern int xStickPos;
extern int zStickVal;
extern int prevZStickVal;
extern int shutterDelay;
extern int prevDelay;
extern int joystickSpeed;
extern int xStickUpper;
extern int xStickLower;
extern int xStickMid;
// --- Enable/Disable functionality --- //
extern bool bootFlag;
extern bool goToStart;
extern bool joystickState;
extern bool autoStackFlag;
extern bool pauseAutoStack;
extern bool shutterState;
extern bool targetFlag;
extern bool flashReady;
extern bool stallGuardConfigured;
// --- Position values --- //
extern int startPosition;
extern int prevStartPosition;
extern int endPosition;
extern int prevEndPosition;
extern int prevStepperPosition;
extern int prevManualMovementCount;
extern volatile long moveDist;
extern volatile bool stepperDisabled;
extern volatile bool directionFwd;
extern long fwdPosition;
extern long bwdPosition;
// --- Stepper motor variables --- //
extern int stepsPerMovement;
extern int numMovements;
extern int prevNumMovements;
extern int stepCount;
extern double distancePerMovement;
extern double prevDistance;
extern int movementProgress;
extern int prevMovementProgress;
extern char prevProgressMovements[10];
extern bool stepperMoved;
extern bool shutterTriggered;

#endif
