#include "Screen.h"

Screen::Screen(TFTFunc tft, KeyboardManager* km) : getTFT(tft), keyboardManager(km) {}

TFT_eSPI& Screen::render() {
  TFT_eSPI& tft = getTFT();
  tft.fillScreen(TFT_BLACK);

  return tft;
}

String Screen::onClick(uint16_t x, uint16_t y) {
  return "";
}
