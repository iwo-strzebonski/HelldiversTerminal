#include "Display.h"

Display::Display(KeyboardManager* km, SDManager* sdManager, AudioManager* audioManager)
  : CURR_SCREEN(""),
    sdManager(sdManager),
    audioManager(audioManager),
    voidScreen(&Display::tft, km),
    splashScreen(&Display::tft, km),
    mainMenu(&Display::tft, km),
    musicPlayer(&Display::tft, km, audioManager),
    currentScreen(&splashScreen)
{}

TFT_eSPI& Display::tft() {
  static TFT_eSPI instance;
  return instance;
}

void Display::init() {
  analogWrite(BACKLIGHT, 255);
  tft().init();
  tft().setRotation(1);
  tft().setSwapBytes(true);
  calibrateTouch();
  setScreen(SplashScreen::SCREEN_TITLE);
}

String Display::getScreen() const {
  return CURR_SCREEN;
}

void Display::setScreen(const String &screen) {
  CURR_SCREEN = screen;

  // Pause audio during screen transition to prevent SPI bus conflicts
  bool wasPlaying = audioManager && audioManager->isPlaying();
  if (audioManager) {
    handleAudioDuringScreenChange(*audioManager, true);
  }

  if (screen == VoidScreen::SCREEN_TITLE) {
    currentScreen = &voidScreen;
  } else if (screen == SplashScreen::SCREEN_TITLE) {
    currentScreen = &splashScreen;
  } else if (screen == MainMenu::SCREEN_TITLE) {
    currentScreen = &mainMenu;
  } else if (screen == MusicPlayer::SCREEN_TITLE) {
    currentScreen = &musicPlayer;
  }

  if (currentScreen) {
    currentScreen -> render();
  }

  // Resume audio after rendering is complete
  if (audioManager && wasPlaying) {
    handleAudioDuringScreenChange(*audioManager, false);
  }
}

AudioManager* Display::getAudioManager() const {
  return audioManager;
}

uint16_t Display::getMinX() const { return calData[0]; }
uint16_t Display::getMaxX() const { return calData[1]; }
uint16_t Display::getMinY() const { return calData[2]; }
uint16_t Display::getMaxY() const { return calData[3]; }
int Display::getWidth() const { return tft().width(); }
int Display::getHeight() const { return tft().height(); }

bool Display::getTouch(uint16_t *x, uint16_t *y) const {
  uint16_t t_x = 0, t_y = 0;
  bool pressed = tft().getTouchRaw(&t_x, &t_y);

  bool x_o = 0, y_o = 0;

  float scale_x = (float)tft().width() / (float)getMaxX();
  float scale_y = (float)tft().height() / (float)getMaxY();

  if (t_x < getMinX()) {
    *x = tft().width();
    x_o = 1;
  } else if (t_x > getMaxX() + getMinX()) {
    *x = 0;
    x_o = 1;
  }

  if (t_y < getMinY()) {
    *y = tft().height();
    y_o = 1;
  } else if (t_y > getMaxY() + getMinY()) {
    *y = 0;
    y_o = 1;
  }

  if (!x_o) {
    *x = (getMaxX() + getMinX() - t_x) * scale_x;
  }

  if (!y_o) {
    *y = (getMaxY() + getMinY() - t_y) * scale_y;
  }

  return pressed;
}

void Display::calibrateTouch() {
  uint8_t calDataOK = 0;

  // Use sdManager instead of FILESYSTEM
  if (sdManager -> exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL) {
      sdManager -> remove(CALIBRATION_FILE);
    } else {
      File f = sdManager -> open(CALIBRATION_FILE, "r");

      if (f) {
        if (f.read((uint8_t*)calData, sizeof(calData)) == sizeof(calData)) {
          calDataOK = 1;
        }

        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft().setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft().fillScreen(TFT_BLACK);
    tft().setCursor(20, 0);
    tft().setTextFont(2);
    tft().setTextSize(1);
    tft().setTextColor(TFT_WHITE, TFT_BLACK);

    tft().println("Touch corners as indicated");

    tft().setTextFont(1);
    tft().println();

    if (REPEAT_CAL) {
      tft().setTextColor(TFT_RED, TFT_BLACK);
      tft().println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft().calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft().setTextColor(TFT_GREEN, TFT_BLACK);
    tft().println("Calibration complete!");

    // store data
    File f = sdManager -> open(CALIBRATION_FILE, "w");

    if (f) {
      f.write((const uint8_t*)calData, sizeof(calData));
      f.close();
    }
  }
}

void Display::render() {
  if (currentScreen) {
    currentScreen -> render();
  }
}

void Display::onTouch(uint16_t x, uint16_t y) {

  if (!currentScreen) return;

  String nextScreen = currentScreen -> onClick(x, y);

  if (nextScreen.length() > 0 && nextScreen != CURR_SCREEN) {
    setScreen(nextScreen);
  }
}
