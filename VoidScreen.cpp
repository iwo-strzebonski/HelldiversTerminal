#include "VoidScreen.h"
#include "SplashScreen.h"

VoidScreen::VoidScreen(TFTFunc tft, KeyboardManager* km)
  : Screen(tft, km)
{
  // Any other initialization here
}

TFT_eSPI& VoidScreen::render() {
  TFT_eSPI& tft = Screen::render();

  analogWrite(BACKLIGHT, 0);
  
  return tft;
}

String VoidScreen::onClick(uint16_t x, uint16_t y) {
  analogWrite(BACKLIGHT, 255);
  return SplashScreen_TITLE;
}
