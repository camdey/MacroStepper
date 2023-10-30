#ifndef VARIABLEDECLARATIONS_H
#define VARIABLEDECLARATIONS_H

#include <Arduino.h>
#include "TMCStepper.h"
#include "StepperControl.h"	
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <gfxButton.h>
#include "TimerFreeTone.h"
// #include <SPI.h>
#include "SdFat.h"
#include <fonts/Arimo_Regular_10.h>
#include <fonts/Arimo_Regular_16.h>
#include <fonts/Arimo_Regular_18.h>
#include <fonts/Arimo_Regular_20.h>
#include <fonts/Arimo_Regular_22.h>
#include <fonts/Arimo_Regular_24.h>
#include <fonts/Arimo_Regular_30.h>
#include <fonts/Arimo_Regular_36.h>
#include <fonts/Arimo_Regular_40.h>
#include <fonts/Arimo_Bold_20.h>
#include <fonts/Arimo_Bold_24.h>
#include <fonts/Arimo_Bold_30.h>
#include <fonts/Arimo_Bold_36.h>
#include <fonts/Arimo_Bold_40.h>
#include <fonts/Syncopate_Bold_36.h>
#include <fonts/Permanent_Marker_Regular_36.h>
#include <fonts/Lato_Black_34.h>
#include <fonts/Lato_Black_40.h>
#include <icons/ArrowUp.h>
#include <icons/ArrowDown.h>
#include <icons/DirectionCW.h>
#include <icons/DirectionCCW.h>
#include <icons/FlashBulb.h>
#include <icons/Play.h>
#include <icons/Pause.h>
#include <icons/RotateScreen.h>
#include <icons/Save.h>
#include <icons/Target.h>
#include <icons/Timer.h>
#include <icons/ToggleOn.h>
#include <icons/ToggleOff.h>
#include <icons/rgb_home_70.h>
#include <icons/rgb_settings_70.h>
#include <icons/rgb_flash_70.h>
#include <icons/rgb_target_70.h>
#include <icons/rgb_save_80.h>
#include <icons/rgb_back_80.h>
#include <icons/rgb_forward_80.h>
#include <icons/rgb_config_80.h>
#include <icons/rgb_cancel_white_80.h>
#include <icons/rgb_cancel_red_80.h>
#include <icons/rgb_flash_yellow_80.h>
#include <icons/rgb_flash_grey_80.h>
#include <icons/rgb_delay_80.h>
#include <icons/rgb_toggle_on_112_50.h>
#include <icons/rgb_toggle_off_112_50.h>
#include <icons/rgb_arrow_up_100.h>
#include <icons/rgb_arrow_down_100.h>
#include <icons/rgb_play_100.h>
#include <icons/rgb_pause_100.h>

extern TouchScreen          ts;
extern MCUFRIEND_kbv        tft;
extern gfxButton            btn;
extern SdFat                sd;

// Definitions for some common 16-bit color values:
#define	BLACK                       0x0000
#define	BLUE                        0x001F
#define	RED                         0xF800
#define	GREEN                       0x07E0
#define CYAN                        0x07FF
#define MAGENTA                     0xF81F
#define YELLOW                      0xFFE0
#define WHITE                       0xFFFF
#define GRAY                        0xE73C
#define DARKGRAY                    0x39E8
#define CUSTOM_GREEN_LITE           0x9736
#define CUSTOM_GREEN                0x4ECC
#define CUSTOM_RED                  0xFBCC
#define CUSTOM_RED_LITE             0xFCD1
#define CUSTOM_BLUE                 0x4EDE
#define CUSTOM_GREY                 0xCE7A
#define CUSTOM_GREY_LITE            0xDEFB
#define CUSTOM_YELLOW               0xFF46

// microsteps and positions
#define AUTOSTACK_MOTOR_STEPS       400                 // number of steps per revolution for the autostack stepper
#define ORBIS_MOTOR_STEPS           200                 // number of steps per revolution for the orbis stepper
#define NR_MICROSTEPS               16                  // number of microsteps per full step
#define MICROSTEP_DIST              0.3125              // distance travelled per microstep in um = 2 / 400 / 16
#define RAIL_LENGTH                 200                 // total length of actuator rails
#define STAGE_LENGTH                62                  // total length of rail stage minus rubber pads on stage ends
#define END_STOPPER_THICKNESS       18                  // rubber bushings protrude and prevents stage from fully reaching rear/front ends plus rubber pads on stage
#define RAIL_TRAVEL_LENGTH          120.40              // 150mm rail length minus 75mm stage length minus 1.9mm for bushing overhang
#define MIN_RAIL_POSITION           0
#define MAX_RAIL_POSITION           384000              // (120 travel / 2mm pitch) * 400 steps * 16 microsteps
#define SAFE_ZONE_BUFFER            6400                // if homed rail, don't allow movement with 2mm of ends (400 * 16)
#define STEALTH_CHOP_VMAX           200000              // max target velocity (VMAX) during StealthChop movements 
#define MAX_STEPS_PER_MOVEMENT      16000               // maximum number of steps allowed per movement. 16000 = 5mm
#define STACK_DWELL_TIME            250                 // time between movements in an AutoStack to give the rail a moment to settle
#define SHUTTER_PULL_TIME           800                 // time the shutter is pulled low to ensure successful triggering
#define MAX_LED_VAL                 570                 // max dimmer value to send to pwm to keep current of driver at 1.2A or below (60% of 2A)
#define STEPS_PER_VMAX              727                 // number of steps per second for every 1000 VMAX

//=====definitions for touch screen coordinates=====//
// Arduino Due + 2.8" TFT
// #define TS_MINX                  160
// #define TS_MAXX                  850
// #define TS_MINY                  132
// #define TS_MAXY                  900
// Arduino Due + 3.5" TFT
#define TS_MINX                     900 // down
#define TS_MAXX                     120 // up
#define TS_MINY                     100 // left
#define TS_MAXY                     940 // right

// Grand Central M4 + 2.8" TFT
// #define TS_MINX                  320
// #define TS_MAXX                  760
// #define TS_MINY                  235
// #define TS_MAXY                  810
// Grand Central M4 + 3.5" TFT
// #define TS_MINX                  760
// #define TS_MAXX                  315
// #define TS_MINY                  852
// #define TS_MAXY                  220

// pin definitions for touch inputs
#define YP                          A1          // must be an analog pin
#define XM                          A2          // must be an analog pin
#define YM                          7           // can be a digital pin
#define XP                          6           // can be a digital pin

// pin definitions for tft screen
// #define LCD_CS                      A3
// #define LCD_CD                      A2
// #define LCD_WR                      A1
// #define LCD_RD                      A0
// #define LCD_RESET                   A4

// stepper1 pins
#define DIAG0_1_PIN                 38
#define DIAG1_1_PIN                 40
// #define DIR_1_PIN                    30          // use SPI stepping with TMC5160
// #define STEP_1_PIN                   32          // use SPI stepping with TMC5160
#define CS_1_PIN                    34
#define EN_1_PIN                    37              // pin 36 is meant to be connected but not working on the Grand Central, 37 is tied to 36 and 36 does not connect to arduino
#define R_1_SENSE                   0.075f          // Watterott TMC5160 uses 0.075


// stepper2 pins
#define DIAG0_2_PIN                 24
#define DIAG1_2_PIN                 26
// #define DIR_2_PIN                    28          // use SPI stepping with TMC5160
// #define STEP_2_PIN                   30          // use SPI stepping with TMC5160
#define CS_2_PIN                    28
#define EN_2_PIN                    42
#define R_2_SENSE                   0.075f          // Watterott TMC5160 uses 0.075


// misc hardware pins
#define XSTICK_PIN                  A7              // joystick X-axis pin (controls fwd and rev)
#define RSTICK_PIN                  A6              // joystick R-axis pin (controls rotation)
#define ZSTICK_PIN                  A5              // button-press from joystick
#define POT_PIN                     A9              // pin for potentiometer to control LED brightnes
#define PIEZO_PIN                   22              // pin for Piezo buzzer
#define FLASH_SENSOR_PIN            A8              // pin for GA1A12S202 light sensor
#define SHUTTER_PIN                 48              // pin for pulling camera focus and shutter to GND via opto
#define LED_PIN                     12              // pin for outputting PWM signal for LED light


#endif
