#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

#include <TFT_eSPI.h>

#include "KeyboardManager.h"

using TFTFunc = TFT_eSPI& (*)();

class Screen {
  public:
    Screen(TFTFunc tft, KeyboardManager* km);

    virtual TFT_eSPI& render();
    virtual String onClick(uint16_t x, uint16_t y);

  protected:
    TFTFunc getTFT;
    KeyboardManager* keyboardManager;
};

#endif
