#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <TFT_eSPI.h>
#include "Arduino.h"

#include "Screen.h"

#define BUTTON_HEIGHT 94
#define BUTTON_WIDTH 94

#define BUTTON_PRESSED_DELAY 200

class Button {
  public:
    Button(
      TFTFunc tft,
      const uint16_t* image,
      const uint16_t* image_touchdown,
      uint16_t width,
      uint16_t height,
      uint16_t posX,
      uint16_t posY,
      bool forcedState
    );

    virtual bool onClick(uint16_t x, uint16_t y, bool newState);

    virtual void draw(bool pressed = false);

  protected:
    TFTFunc getTFT;

    const uint16_t* image;
    const uint16_t* image_touchdown;

    uint16_t width;
    uint16_t height;
    uint16_t posX;
    uint16_t posY;
    bool forcedState;
};

extern const uint16_t BUTTON_PRESSED[] PROGMEM;
extern const uint16_t BUTTON_GRAY_PRESSED[] PROGMEM;
extern const uint16_t BUTTON_RED_PRESSED[] PROGMEM;

extern const uint16_t BUTTON_HOME[] PROGMEM;
extern const uint16_t BUTTON_SHUTDOWN[] PROGMEM;
extern const uint16_t BUTTON_AUDIO[] PROGMEM;

extern const uint16_t BUTTON_ARROW_UP[] PROGMEM;
extern const uint16_t BUTTON_ARROW_RIGHT[] PROGMEM;
extern const uint16_t BUTTON_ARROW_DOWN[] PROGMEM;
extern const uint16_t BUTTON_ARROW_LEFT[] PROGMEM;

extern const uint16_t BUTTON_LOCK_OPEN[] PROGMEM;
extern const uint16_t BUTTON_LOCK_CLOSED[] PROGMEM;

#endif
