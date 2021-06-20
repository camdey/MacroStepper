#include "GlobalVariables.h"
#include "menu/UI-Main.h"
#include "menu/UI-Video.h"
#include "menu/UI-Global.h"

namespace video_screen {
  #define num_btns 6
  gfxButton *btn_array[num_btns];

  gfxButton btn_Speed         =   btn.initButton("Speed",   "fillRoundRect",  0,    20,   160,  80,   15, DARKGRAY,   true  );
  gfxButton btn_Back          =   btn.initBitmapButton(backArrow,  220,  220,  80, 80, WHITE,      true  );
  gfxButton btn_PlayPause     =   btn.initBitmapButton(play,       30,  100,  120,  120,  CUSTOM_GREEN, true);
  gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,    350,  20,   120,  120,  CUSTOM_GREEN, true);
  gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown,  350,  180,  120,  120,  CUSTOM_RED,   true);
  gfxButton btn_Direction     =   btn.initBitmapButton(dir,  350,  180,  120,  120,  CUSTOM_RED,   true);

  float prevStepperRpm = 0.00;
  #define MAX_VIDEO_SPEED
  #define MAX_VIDEO_RPM


  void initAutoButtons() {
    btn_array[0] = &btn_Speed;
    btn_array[1] = &btn_Back;
    btn_array[2] = &btn_PlayPause;
    btn_array[3] = &btn_ArrowUp;
    btn_array[4] = &btn_ArrowDown;
    btn_array[5] = &btn_Direction;

    btn_Speed.addToggle(func_Speed,             0 );
    btn_Direction.addToggle(func_Direction,  0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_PlayPause.addToggle(func_PlayPause,     0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );
  }


  void populateAutoScreen() {
    setCurrentScreen("Video");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
        btn_array[i]->drawButton();
      }
    }

    // draw text
    func_Speed.writeTextTopCentre(Arimo_Regular_30, WHITE);
  }


  void checkAutoButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
  }


  void func_Speed(bool btnActive) {
    if (btnActive ) {
      setArrowsEnabled(true);
      setEditMovementDistance(true);
      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(get360Speed()));
    }
    else if (!btnActive) {
      setArrowsEnabled(false);
      setEditMovementDistance(false);
      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(get360Speed()));
    }
  }


  void func_Direction(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("Stack");
    }
  }


  void func_PlayPause(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      btn_PlayPause.drawButton(BLACK); // replace existing button
      btn_PlayPause.updateBitmap(pause); // update bitmap image
      btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
      btn_PlayPause.drawButton(); // draw
    }
    else if (!btnActive && !areArrowsEnabled()) {
      btn_PlayPause.drawButton(BLACK); // replace existing button
      btn_PlayPause.updateBitmap(play); // update bitmap image
      btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
      btn_PlayPause.drawButton(); // draw
      }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && canEditMovementDistance() && areArrowsEnabled()) {
      incrementVideoSpeed();
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(get360Speed()));
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && canEditMovementDistance() && areArrowsEnabled()) {
      decrementVideoSpeed();
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(get360Speed()));
    }
  }


  void calcStepperSpeed() {
      int speed = map(getAvgReading(), 0, 1023, 0, MAX_VIDEO_SPEED); 
      stepper.setSpeed(speed*getStepperDirection());
      float stepperRpm = round(map(speed, 0, MAX_VIDEO_SPEED, 0, MAX_VIDEO_RPM*100)/10.00)/10.00; // convert to 0-500, divide by 10 and round, divide by 10
      if (stepperRpm != prevStepperRpm) {
          prevStepperRpm = stepperRpm;
          printSpeed(stepperRpm);
      }
  }
}
