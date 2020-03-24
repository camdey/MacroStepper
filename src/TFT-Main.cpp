#include "TFT-Home.h"
#include "TFT-Manual.h"
#include "gfxButton.h"


gfxButton gfxB;
gfxTouch  gfxT;

void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce) {
  gfxT.setToggleDebounce(toggleDebounce);
  gfxT.setMomentaryDebounce(momentaryDebounce);

  initHomeButtons();
  initManualButtons();

}
