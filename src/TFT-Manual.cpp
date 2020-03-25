#include "TFT-Main.h"
#include "TFT-Home.h"
#include "TFT-Manual.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"
#include "gfxButton.h"

#define num_btns_Manual 8
gfxButton btn_array_Manual[num_btns_Manual];
gfxTouch  tch_array_Manual[num_btns_Manual];


gfxButton btn_StepDistance =   gfxB.initButton(       "manual", "fillRoundRect",  -10,  5,    135,  65,   15, CUSTOM_BLUE  );
gfxButton btn_StepNumber   =   gfxB.initButton(       "manual", "fillRoundRect",  -10,  85,   135,  65,   15, CUSTOM_BLUE  );
gfxButton btn_Position     =   gfxB.initButton(       "manual", "fillRoundRect",  -10,  165,  135,  65,   15, CUSTOM_BLUE  );
gfxButton btn_Flash        =   gfxB.initBitmapButton( "manual", aperture,         155,  15,   50,   50,       CUSTOM_RED   );
gfxButton btn_Reset        =   gfxB.initBitmapButton( "manual", reset,            155,  95,   50,   50,       WHITE        );
gfxButton btn_Back         =   gfxB.initBitmapButton( "manual", backArrow,        155,  175,  50,   50,       WHITE        );
gfxButton btn_ArrowUp      =   gfxB.initBitmapButton( "manual", arrowUp,          230,  15,   60,   68,       CUSTOM_GREEN );
gfxButton btn_ArrowDown    =   gfxB.initBitmapButton( "manual", arrowDown,        230,  135,  60,   68,       CUSTOM_RED   );
gfxTouch  tch_StepDistance =   gfxT.addToggle(    btn_StepDistance,  func_StepDistance,  "btn_StepDistance",  20 );
gfxTouch  tch_Flash        =   gfxT.addToggle(    btn_Flash,         func_Flash,         "btn_Flash",         20 );
gfxTouch  tch_Reset        =   gfxT.addMomentary( btn_Reset,         func_Reset,         "btn_Reset",         20 );
gfxTouch  tch_Back         =   gfxT.addMomentary( btn_Back,          func_Back,          "btn_Back",          20 );
gfxTouch  tch_ArrowUp      =   gfxT.addMomentary( btn_ArrowUp,       func_ArrowUp,       "btn_ArrowUp",       20 );
gfxTouch  tch_ArrowDown    =   gfxT.addMomentary( btn_ArrowDown,     func_ArrowDown,     "btn_ArrowDown",     20 );


void initManualButtons() {
  btn_array_Manual[0] = btn_StepDistance;
  btn_array_Manual[1] = btn_StepNumber;
  btn_array_Manual[2] = btn_Position;
  btn_array_Manual[3] = btn_Flash;
  btn_array_Manual[4] = btn_Reset;
  btn_array_Manual[5] = btn_Back;
  btn_array_Manual[6] = btn_ArrowUp;
  btn_array_Manual[7] = btn_ArrowDown;

  tch_array_Manual[0] = tch_StepDistance;
  // tch_array_Manual[1] = NULL;
  // tch_array_Manual[2] = NULL;
  tch_array_Manual[3] = tch_Flash;
  tch_array_Manual[4] = tch_Reset;
  tch_array_Manual[5] = tch_Back;
  tch_array_Manual[6] = tch_ArrowUp;
  tch_array_Manual[7] = tch_ArrowDown;
}


void populateManualScreen() {
  // draw buttons
  for (int i=0; i < num_btns_Manual; i++) {
    btn_array_Manual[i].drawButton(tft);
  }
  // draw text
  btn_StepDistance.writeText( tft, Arimo_Regular_24, String("Step Dist."),  WHITE, "c");
  btn_StepNumber.writeText(   tft, Arimo_Regular_24, String("Step Nr."),    WHITE, "c");
  btn_Position.writeText(     tft, Arimo_Regular_24, String("Rail Pos."),   WHITE, "c");
}


void checkManualButtons(int touch_x, int touch_y, int touch_z) {
  // if screen pressed
  if (touch_z >= 100 && touch_z <= 1000) {
    for (int i=0; i < num_btns_Manual; i++) {
      tch_array_Manual[i].checkButton("manual", touch_x, touch_y);
    }
  }

  // TODO - something better so we don't check the entire array every loop??
  // else if screen not pressed, re-enable toggle
  if (touch_z == 0) {
    for (int i=0; i < num_btns_Manual; i++) {
      if (tch_array_Manual[i].touchType == "toggle") {
        tch_array_Manual[i].toggleCoolOff();
      }
    }
  }
}


void func_StepDistance(bool btnActive) {
  if (btnActive == true) {
    arrowsActive = !arrowsActive;
    editMovementDistance = !editMovementDistance;

    if (arrowsActive == false && editMovementDistance == false) {
      btn_StepDistance.writeText(tft, Arimo_Regular_24, String("Step Dist."), WHITE, "c");
      updateValueField("Step Dist", WHITE);
    }
    if (arrowsActive == true && editMovementDistance == true) {
      btn_StepDistance.writeText(tft, Arimo_Regular_24, String("Step Dist."), YELLOW, "c");
      updateValueField("Step Dist", YELLOW);
    }
  }
  else {
    btn_StepDistance.drawButton(tft, CUSTOM_BLUE);
    btn_StepDistance.writeText(tft, Arimo_Regular_24, String("Step Dist."), WHITE, "c");
    updateValueField("Step Dist", WHITE);
  }
}


void func_StepNumber() {
  btn_StepNumber.drawButton(tft, CUSTOM_BLUE);
  btn_StepNumber.writeText(tft, Arimo_Regular_24, String("Step Nr."), WHITE, "c");
  updateValueField("Step Nr", WHITE);
}


void func_Position() {
  btn_Position.drawButton(tft, CUSTOM_BLUE);
  btn_Position.writeText(tft, Arimo_Regular_24, String("Rail Pos."), WHITE, "c");
  updateValueField("Rail Position", WHITE);
}


void func_Flash(bool btnActive) {
  int colour;
  // pick button colour based on shutter state
  if (shutterEnabled == true) {
    colour = CUSTOM_GREEN;
  } else {
    colour = CUSTOM_RED;
  }
  // draw button, but only toggleShutter if button has been pressed
  if (btnActive == true) {
    toggleShutter();
    btn_Flash.drawButton(tft, colour);
  }
  else {
    btn_Flash.drawButton(tft, colour);
  }
}


void func_Reset(bool btnActive) {
  if (btnActive == true) {
    // when reset button pressed, redraw step nr button and print new value via updateValueField
    btn_Reset.drawButton(tft, WHITE);
    btn_StepNumber.writeText(tft, Arimo_Regular_24, String("Step Nr."), WHITE, "c");

    manualMovementCount = 0;
    prevManualMovementCount = 0;
    updateValueField("Step Nr", WHITE);
  }
  else {
    btn_Reset.drawButton(tft, WHITE);
  }
}


void func_Back(bool btnActive) {
  if (btnActive == true) {
    // go back to start screen
    startScreen();
  }
  else {
    btn_Back.drawButton(tft, WHITE);
  }
}


void func_ArrowUp(bool btnActive) {
  if (btnActive == true) {
    // take photo if shutter enabled
    if (shutterEnabled == true && activeScreen == 2) {
      shutterTriggered = triggerShutter();
    }
    stepperMoved = stepMotor(1, 500); // forward
  }
  else {
    btn_ArrowUp.drawButton(tft, CUSTOM_GREEN);
  }
}


void func_ArrowDown(bool btnActive) {
  if (btnActive == true) {
    // move stepper
    stepperMoved = stepMotor(-1, 500); // reverse
  }
  else {
    btn_ArrowDown.drawButton(tft, CUSTOM_RED);
  }
}
