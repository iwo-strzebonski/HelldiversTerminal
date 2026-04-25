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
  // We deliberately use getTouchRaw() here instead of TFT_eSPI's
  // getTouch(): the latter takes several samples with built-in delays to
  // de-noise the panel, which is far too slow for our polling loop and
  // caused a noticeable input-handling stall. getTouchRaw() does a single
  // SPI read; we then apply the same calibration mapping that
  // TFT_eSPI::convertRawXY() applies internally so the resulting
  // coordinates are in screen space.
  uint16_t t_x = 0, t_y = 0;
  bool pressed = tft().getTouchRaw(&t_x, &t_y);
  if (!pressed) return false;

  // calData layout (produced by tft.calibrateTouch / consumed by setTouch):
  //   [0] x raw offset, [1] x raw range,
  //   [2] y raw offset, [3] y raw range,
  //   [4] flags: bit0 = rotate (swap raw axes), bit1 = invert_x, bit2 = invert_y
  uint16_t x_off = calData[0] ? calData[0] : 1;
  uint16_t x_rng = calData[1] ? calData[1] : 1;
  uint16_t y_off = calData[2] ? calData[2] : 1;
  uint16_t y_rng = calData[3] ? calData[3] : 1;
  bool rotate   = calData[4] & 0x01;
  bool invert_x = calData[4] & 0x02;
  bool invert_y = calData[4] & 0x04;

  int32_t w = tft().width();
  int32_t h = tft().height();

  int32_t xx, yy;
  if (!rotate) {
    xx = ((int32_t)t_x - (int32_t)x_off) * w / (int32_t)x_rng;
    yy = ((int32_t)t_y - (int32_t)y_off) * h / (int32_t)y_rng;
  } else {
    xx = ((int32_t)t_y - (int32_t)x_off) * w / (int32_t)x_rng;
    yy = ((int32_t)t_x - (int32_t)y_off) * h / (int32_t)y_rng;
  }
  if (invert_x) xx = w - xx;
  if (invert_y) yy = h - yy;

  // Clamp to screen so a slightly off-edge raw sample doesn't wrap around
  // to a wildly wrong UI region.
  if (xx < 0) {
    xx = 0;
  } else if (xx >= w) {
    xx = w - 1;
  }
  if (yy < 0) {
    yy = 0;
  } else if (yy >= h) {
    yy = h - 1;
  }

  *x = (uint16_t)xx;
  *y = (uint16_t)yy;
  return true;
}

void Display::logCalibration(const char* source) const {
  Serial.print("[Touch calibration] ");
  Serial.print(source);
  Serial.print(" calData = { ");
  for (uint8_t i = 0; i < 5; ++i) {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }
  Serial.print(" } screen=");
  Serial.print(tft().width());
  Serial.print("x");
  Serial.print(tft().height());
  Serial.print(" rotation=");
  Serial.println(tft().getRotation());
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
    logCalibration("loaded from SD");
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

    // tft().calibrateTouch() above already installs the freshly-produced
    // calibration via setTouch() internally, so we only need to log it.
    logCalibration("freshly calibrated");
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
