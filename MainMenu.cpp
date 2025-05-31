#include "MainMenu.h"
#include "VoidScreen.h"
#include "Buttons.h"

MainMenu::MainMenu(TFTFunc tft, KeyboardManager* km)
  : Screen(tft, km) {
  // Turn off screen
  buttons[0] = new Button(
    tft,
    BUTTON_SHUTDOWN,
    BUTTON_RED_PRESSED,
    BUTTON_WIDTH,
    BUTTON_HEIGHT,
    480 - BUTTON_WIDTH - 10, 320 - BUTTON_HEIGHT - 10,
    false
  );

  // Arrow up
  buttons[1] = new Button(
    tft,
    BUTTON_ARROW_UP,
    BUTTON_GRAY_PRESSED,
    BUTTON_WIDTH,
    BUTTON_HEIGHT,
    BUTTON_WIDTH + 10 * 2, 10,
    false
  );
  
  // Arrow right
  buttons[2] = new Button(
    tft,
    BUTTON_ARROW_RIGHT,
    BUTTON_GRAY_PRESSED,
    BUTTON_WIDTH,
    BUTTON_HEIGHT,
    BUTTON_WIDTH * 2 + 10 * 3, 160 - BUTTON_HEIGHT / 2,
    false
  );
  
  // Arrow down
  buttons[3] = new Button(
    tft,
    BUTTON_ARROW_DOWN,
    BUTTON_GRAY_PRESSED,
    BUTTON_WIDTH,
    BUTTON_HEIGHT,
    BUTTON_WIDTH + 10 * 2, 320 - BUTTON_HEIGHT - 10,
    false
  );
  
  // Arrow left
  buttons[4] = new Button(
    tft,
    BUTTON_ARROW_LEFT,
    BUTTON_GRAY_PRESSED,
    BUTTON_WIDTH,
    BUTTON_HEIGHT,
    10, 160 - BUTTON_HEIGHT / 2,
    false
  );
  
  // Stratagem Lock
  buttons[5] = new Button(
    tft,
    BUTTON_LOCK_OPEN,
    BUTTON_LOCK_CLOSED,
    BUTTON_WIDTH,
    BUTTON_HEIGHT,
    480 - BUTTON_WIDTH - 10, 160 - BUTTON_HEIGHT / 2,
    true
  );
}

TFT_eSPI& MainMenu::render() {
  TFT_eSPI& tft = Screen::render();

  // Example: Draw a title
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.setTextDatum(MC_DATUM);
  // tft.drawString("Main Menu", tft.width() / 2, 20);

  for (int i = 0; i < BUTTON_COUNT; ++i) {
    buttons[i] -> draw(false);
  }

  return tft;
}

String MainMenu::onClick(uint16_t x, uint16_t y) {
  if (buttons[0] -> onClick(x, y, true)) {
    return VoidScreen_TITLE;
  }

  for (int i = 1; i <= 4; ++i) {
    if (buttons[i] -> onClick(x, y, true)) {
      if (stratagemLock) {
        keyboardManager -> sendArrow(i - 1);
        delay(KEY_DELAY);
      }
    }
  }

  if (buttons[5] -> onClick(x, y, !stratagemLock)) {
    stratagemLock = !stratagemLock;
    keyboardManager -> toggleKey(KEY_LEFT_CTRL, stratagemLock);
  }

  return "";
}
