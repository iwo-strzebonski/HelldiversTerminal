#ifndef VOIDSCREEN_H
#define VOIDSCREEN_H

#include "Screen.h"

#define VoidScreen_TITLE "VOID_SCREEN"

class VoidScreen : public Screen {
  public:
    static constexpr const char* SCREEN_TITLE = VoidScreen_TITLE;

    VoidScreen(TFTFunc tft, KeyboardManager* km);

    // Override render and onClick as needed
    TFT_eSPI& render() override;
    String onClick(uint16_t x, uint16_t y) override;

    // The entire VoidScreen acts as one big "wake" button, so without a
    // longer debounce a single tap on MainMenu's shutdown button can
    // bounce straight through Void -> Splash -> MainMenu before the
    // user's finger has even left the panel.
    unsigned long getPressDebounceMs() const override { return 500; }
};

#endif // VOIDSCREEN_H
