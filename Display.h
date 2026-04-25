#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#include <TFT_eSPI.h>

#include "SDManager.h"
#include "KeyboardManager.h"
#include "AudioManager.h"

#include "Screen.h"
#include "VoidScreen.h"
#include "SplashScreen.h"
#include "MainMenu.h"
#include "MusicPlayer.h"

#define CALIBRATION_FILE "/TouchData.cal"
#define REPEAT_CAL false

class Display {
  public:
    Display(KeyboardManager* km, SDManager* sdManager, AudioManager* audioManager);

    static TFT_eSPI& tft();

    void init();
    void calibrateTouch();
    void render();
    void onTouch(uint16_t x, uint16_t y);

    String getScreen() const;
    void setScreen(const String &screen);

    uint16_t getMinX() const;
    uint16_t getMaxX() const;
    uint16_t getMinY() const;
    uint16_t getMaxY() const;
    int getWidth() const;
    int getHeight() const;

    bool getTouch(uint16_t *x, uint16_t *y) const;

    // Returns the debounce window (ms) the currently-active screen wants
    // applied between successive accepted touch events. Falls back to the
    // base-class default if no screen is set yet.
    unsigned long getPressDebounceMs() const;

    void logCalibration(const char* source) const;
    
    AudioManager* getAudioManager() const;

  private:
    uint16_t calData[5];
    String CURR_SCREEN;

    VoidScreen voidScreen;
    SplashScreen splashScreen;
    MainMenu mainMenu;
    MusicPlayer musicPlayer;
  
    Screen* currentScreen;
    SDManager* sdManager;
    AudioManager* audioManager;
};

#endif // DISPLAY_H
