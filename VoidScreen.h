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
};

#endif // VOIDSCREEN_H
