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

    // Software debounce window (ms) applied between successive accepted
    // touch events while this screen is active. Defaults to the standard
    // panel-chatter window; screens whose entire surface acts as a single
    // button (e.g. SplashScreen, VoidScreen) override this with a longer
    // value so a single tap doesn't cascade through several screens while
    // the user's finger is still on (or just leaving) the panel.
    static constexpr unsigned long DEFAULT_PRESS_DEBOUNCE_MS = 50;
    virtual unsigned long getPressDebounceMs() const { return DEFAULT_PRESS_DEBOUNCE_MS; }

  protected:
    TFTFunc getTFT;
    KeyboardManager* keyboardManager;
};

#endif
