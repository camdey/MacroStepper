#include "TFT-Home.h"
#include "TFT-Manual.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"
#include "gfxButton.h"

#define numButtons 6
gfxButton gfxB;
gfxTouch  gfxT;
gfxButton buttonArray[numButtons];
gfxTouch touchArray[numButtons];


gfxButton b_manual_StepDistance =   gfxB.initButton(        "manual", "fillRoundRect",  -10,  5,    135,  65,   15, CUSTOM_BLUE );
gfxButton b_manual_StepNumber   =   gfxB.initButton(        "manual", "fillRoundRect",  -10,  85,   135,  65,   15, CUSTOM_BLUE );
gfxButton b_manual_Position     =   gfxB.initButton(        "manual", "fillRoundRect",  -10,  165,  135,  65,   15, CUSTOM_BLUE );
gfxButton b_manual_Flash        =   gfxB.initBitmapButton(  "manual", aperture,         155,  15,   50,   50,       CUSTOM_RED  );
gfxButton b_manual_Reset        =   gfxB.initBitmapButton(  "manual", reset,            155,  95,   50,   50,       WHITE       );
gfxButton b_manual_Back         =   gfxB.initBitmapButton(  "manual", backArrow,        155,  175,  50,   50,       WHITE       );
gfxTouch  t_manual_StepDistance =   gfxT.addToggle(         b_manual_StepDistance,  manual_StepDistance,  "b_manual_StepDistance",  20 );
gfxTouch  t_manual_Flash        =   gfxT.addToggle(         b_manual_Flash,         manual_Flash,         "b_manual_Flash",         20 );
gfxTouch  t_manual_Reset        =   gfxT.addMomentary(      b_manual_Reset,         manual_Reset,         "b_manual_Reset",         20 );
gfxTouch  t_manual_Back         =   gfxT.addMomentary(      b_manual_Back,          manual_Back,          "b_manual_Back",          20 );

void initButtons() {
  gfxT.setToggleDebounce(250);
  gfxT.setMomentaryDebounce(100);

  buttonArray[0] = b_manual_StepDistance;
  buttonArray[1] = b_manual_StepNumber;
  buttonArray[2] = b_manual_Position;
  buttonArray[3] = b_manual_Flash;
  buttonArray[4] = b_manual_Reset;
  buttonArray[5] = b_manual_Back;

  touchArray[0] = t_manual_StepDistance;
  // touchArray[1] = NULL;
  // touchArray[2] = NULL;
  touchArray[3] = t_manual_Flash;
  touchArray[4] = t_manual_Reset;
  touchArray[4] = t_manual_Back;
}

void manual_StepDistance(bool btnActive) {
  if (btnActive == true) {
    arrowsActive = !arrowsActive;
    editMovementDistance = !editMovementDistance;

    if (arrowsActive == false && editMovementDistance == false) {
      b_manual_StepDistance.writeText(tft, Arimo_Regular_24, String("Step Dist."), WHITE, "c");
      updateValueField("Step Dist", WHITE);
    }
    if (arrowsActive == true && editMovementDistance == true) {
      b_manual_StepDistance.writeText(tft, Arimo_Regular_24, String("Step Dist."), YELLOW, "c");
      updateValueField("Step Dist", YELLOW);
    }
  }
  else {
    b_manual_StepDistance.drawButton(tft, CUSTOM_BLUE);
    b_manual_StepDistance.writeText(tft, Arimo_Regular_24, String("Step Dist."), WHITE, "c");
    updateValueField("Step Dist", WHITE);
  }
}

void manual_StepNumber() {
  b_manual_StepNumber.drawButton(tft, CUSTOM_BLUE);
  b_manual_StepNumber.writeText(tft, Arimo_Regular_24, String("Step Nr."), WHITE, "c");
  updateValueField("Step Nr", WHITE);
}

void manual_Position() {
  b_manual_Position.drawButton(tft, CUSTOM_BLUE);
  b_manual_Position.writeText(tft, Arimo_Regular_24, String("Rail Pos."), WHITE, "c");
  updateValueField("Rail Position", WHITE);
}

void manual_Flash(bool btnActive) {
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
    b_manual_Flash.drawButton(tft, colour);
  }
  else {
    b_manual_Flash.drawButton(tft, colour);
  }
}

void manual_Reset(bool btnActive) {
  if (btnActive == true) {
    // when reset button pressed, redraw step nr button and print new value via updateValueField
    b_manual_Reset.drawButton(tft, WHITE);
    b_manual_StepNumber.writeText(tft, Arimo_Regular_24, String("Step Nr."), WHITE, "c");

    manualMovementCount = 0;
    prevManualMovementCount = 0;
    updateValueField("Step Nr", WHITE);
  }
  else {
    b_manual_Reset.drawButton(tft, WHITE);
  }
}

void manual_Back(bool btnActive) {
  if (btnActive == true) {
    // go back to start screen
    startScreen();
  }
  else {
    b_manual_Back.drawButton(tft, WHITE);
  }
}
