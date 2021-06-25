#include "GlobalVariables.h"
#include "StepperControl.h"
#include "DriverConfig.h"
#include "menu/UI-Main.h"
#include "menu/UI-Video360.h"
#include "menu/UI-Global.h"

namespace video_screen {
  #define num_btns 6
  gfxButton *btn_array[num_btns];

  gfxButton btn_PlayPause     =   btn.initBitmapButton(play,        30,   100,  120,  120,  CUSTOM_GREEN, true  );
  gfxButton btn_Speed         =   btn.initButton("RPM", "fillRoundRect",  160,  20, 160,  80, 15, DARKGRAY, true  );
  gfxButton btn_Back          =   btn.initBitmapButton(backArrow,   200,  220,  80,   80,   WHITE,        true  );
  gfxButton btn_Direction     =   btn.initBitmapButton(dir_cw,      200,  120,  80,   80,   CUSTOM_GREEN, true  );
  gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,     350,  20,   120,  120,  CUSTOM_GREEN, true  );
  gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown,   350,  180,  120,  120,  CUSTOM_RED,   true  );
 

  #define STEPS_PER_VMAX 727 // number of steps per second for every 1000 VMAX
  #define ORBIS_MOTOR_STEPS 200 // number of steps per revolution for the stepper motor used for Orbis
  int nrSteps = 10000;


  void initVideo360Buttons() {
    btn_array[0] = &btn_Speed;
    btn_array[1] = &btn_Back;
    btn_array[2] = &btn_PlayPause;
    btn_array[3] = &btn_ArrowUp;
    btn_array[4] = &btn_ArrowDown;
    btn_array[5] = &btn_Direction;

    btn_Speed.addToggle(func_Speed,             0 );
    btn_Direction.addToggle(func_Direction,     0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_PlayPause.addToggle(func_PlayPause,     0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );
  }


  void populateVideo360Screen() {
    setCurrentScreen("Video360");
    setJoystickMaxVelocity(5000); // lower joystick speed

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
        btn_array[i]->drawButton();
      }
    }

    // draw text
    btn_Speed.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_Speed.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getRevsPerMinute()/10.00, 1));
  }


  void checkVideo360Buttons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
  }


  void func_Speed(bool btnActive) {
    if (btnActive ) {
      setArrowsEnabled(true);
      btn_Speed.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_Speed.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getRevsPerMinute()/10.00, 1));
    }
    else if (!btnActive) {
      setArrowsEnabled(false);
      btn_Speed.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_Speed.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getRevsPerMinute()/10.00, 1));
    }
  }


  void func_Direction(bool btnActive) {
    if (btnActive) {
      btn_Direction.drawButton(BLACK); // replace existing button
      btn_Direction.updateBitmap(dir_ccw); // update bitmap image
      btn_Direction.updateColour(CUSTOM_RED); // change colour
      btn_Direction.drawButton(); // draw
      setVideo360Target(getVideo360Target()*-1); // change direction
    }
    else if (!btnActive) {
      btn_Direction.drawButton(BLACK); // replace existing button
      btn_Direction.updateBitmap(dir_cw); // update bitmap image
      btn_Direction.updateColour(CUSTOM_GREEN); // change colour
      btn_Direction.drawButton(); // draw
      setVideo360Target(getVideo360Target()*-1); // change direction
    }
    // if video360 is active, change direction while moving
    if (isVideo360Active()) {
      video360(getVideo360Target());
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      if (isVideo360Active()) {
        video360(0); // pause movement
        func_PlayPause(false); // reset PlayPause button
        btn_PlayPause.setButtonActive(false); // reset button state
        setVideo360Active(false);
      }
      configStealthChop(); // reset VMAX
      setJoystickMaxVelocity(100000); // reset back to original value
      populateScreen("Orbis");
    }
  }


  void func_PlayPause(bool btnActive) {
    // make sure VMAX is updated before starting
    rpmToVmax();
    if (btnActive) {
      btn_PlayPause.drawButton(BLACK); // replace existing button
      btn_PlayPause.updateBitmap(pause); // update bitmap image
      btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
      btn_PlayPause.drawButton(); // draw
      video360(getVideo360Target());
      setVideo360Active(true);
    }
    else if (!btnActive) {
      btn_PlayPause.drawButton(BLACK); // replace existing button
      btn_PlayPause.updateBitmap(play); // update bitmap image
      btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
      btn_PlayPause.drawButton(); // draw
      video360(0);
      setVideo360Active(false);
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && areArrowsEnabled()) {
      setRevsPerMinute(getRevsPerMinute()+1);
      btn_Speed.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getRevsPerMinute()/10.00, 1));
      rpmToVmax();
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && areArrowsEnabled()) {
      setRevsPerMinute(getRevsPerMinute()-1);
      btn_Speed.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getRevsPerMinute()/10.00, 1));
      rpmToVmax();
    }
  }


  // convert desired RPM to VMAX for setting max speed
  // store STEPS_PER_VMAX and RPM as integers and then divide by necessary denominator to convert back
  void rpmToVmax() {
      int stepsPerRev = ORBIS_MOTOR_STEPS * nrMicrosteps;     // 200 * 16 = 3200
      int rpm = getRevsPerMinute();                           // e.g. 5.0rpm = 50
      int totalSteps = rpm * stepsPerRev*1.00 / 10;           // 50 * 3200 / 10 = 16000
      int stepsPerSecond = totalSteps*1.00 / 60;              // 16000 / 60 = 266 SPS
      int vMax = stepsPerSecond*1.00 / STEPS_PER_VMAX * 1000; // 266 / 727 * 1000 = 365.8
      setTargetVelocity(vMax);
  }

}
