#ifndef MAINMENU_H
#define MAINMENU_H

#include "Screen.h"
#include "Buttons.h"

#define MainMenu_TITLE "MAIN_MENU"

#define BUTTON_COUNT 7

class MainMenu : public Screen {
  public:
    static constexpr const char* SCREEN_TITLE = MainMenu_TITLE;

    MainMenu(TFTFunc tft, KeyboardManager* km);

    TFT_eSPI& render() override;
    String onClick(uint16_t x, uint16_t y) override;

  private:
    Button* buttons[BUTTON_COUNT]; // Array of pointers to Button objects
    bool stratagemLock = false;
};

#endif // MAINMENU_H
