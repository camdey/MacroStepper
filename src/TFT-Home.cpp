#include "TFT-Home.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"
#include "gfxButton.h"

#define numButtons 5
gfxButton gfxB;
gfxTouch  gfxT;
gfxButton buttonArray[numButtons];
gfxTouch touchArray[numButtons];

gfxButton b_main_Auto     =   gfxB.initButton(        "main", "fillRoundRect",  170,  100,  130,  60,   8, CUSTOM_GREEN );
gfxButton b_main_Manual   =   gfxB.initButton(        "main", "fillRoundRect",  20,   100,  130,  60,   8, CUSTOM_BLUE  );
gfxButton b_main_Home     =   gfxB.initBitmapButton(  "main", house,            30,   190,  50,   42,      WHITE        );
gfxButton b_main_Flash    =   gfxB.initBitmapButton(  "main", flash,            145,  190,  40,   42,      WHITE        );
gfxButton b_main_Rotate   =   gfxB.initBitmapButton(  "main", rotate_icon,      250,  190,  45,   39,      WHITE        );
gfxTouch  t_main_Auto     =   gfxT.addMomentary(  b_main_Auto,    main_Auto,    "b_main_Auto",    20  );
gfxTouch  t_main_Manual   =   gfxT.addMomentary(  b_main_Manual,  main_Manual,  "b_main_Manual",  20  );
gfxTouch  t_main_Home     =   gfxT.addMomentary(  b_main_Home,    main_Home,    "b_main_Home",    20  );
gfxTouch  t_main_Flash    =   gfxT.addMomentary(  b_main_Flash,   main_Flash,   "b_main_Flash",   20  );
gfxTouch  t_main_Rotate   =   gfxT.addMomentary(  b_main_Rotate,  main_Rotate,  "b_main_Rotate",  20  );

void initButtons() {
  gfxT.setToggleDebounce(250);
  gfxT.setMomentaryDebounce(100);

  buttonArray[0] = b_main_Manual;
  buttonArray[1] = b_main_Auto;
  buttonArray[2] = b_main_Home;
  buttonArray[3] = b_main_Flash;
  buttonArray[4] = b_main_Rotate;

  touchArray[0] = t_main_Manual;
  touchArray[1] = t_main_Auto;
  touchArray[2] = t_main_Home;
  touchArray[3] = t_main_Flash;
  touchArray[4] = t_main_Rotate;
}

void main_Auto(bool btnActive) {
  if (btnActive == true) {
    autoScreen();
  }
  else {
    b_main_Auto.drawButton(tft, CUSTOM_GREEN);
    b_main_Auto.writeText(tft, Arimo_Bold_30, String("Auto"), BLACK, "c");
  }
}

void main_Manual(bool btnActive) {
  if (btnActive == true) {
    manualScreen();
  }
  else {
    b_main_Manual.drawButton(tft, CUSTOM_BLUE);
    b_main_Manual.writeText(tft, Arimo_Bold_30, String("Manual"), BLACK, "c");
  }
}

void main_Home(bool btnActive) {
  if (btnActive == true) {
    b_main_Home.drawButton(tft, CUSTOM_RED);
    runHomingSequence = true;
    homeRail();
    b_main_Home.drawButton(tft, WHITE);
  }
  else {
    b_main_Home.drawButton(tft, WHITE);
  }
}

void main_Flash(bool btnActive) {
  if (btnActive == true) {
    flashScreen();
  }
  else {
    b_main_Flash.drawButton(tft, WHITE);
  }
}

void main_Rotate(bool btnActive) {
  if (btnActive == true) {
    rotateScreen(); // rotate screen, touch controls, and joystick
    startScreen();
  }
  else {
    b_main_Rotate.drawButton(tft, WHITE);
  }
}
