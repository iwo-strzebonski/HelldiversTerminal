#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "Screen.h"

// You can adjust these as needed for your project
#define SplashScreen_HEIGHT 320
#define SplashScreen_WIDTH  480
#define SplashScreen_TITLE "SPLASH_SCREEN"

// Dummy image data for demonstration (replace with your real image data)
extern const uint16_t SplashScreenImage[] PROGMEM;

class SplashScreen : public Screen {
  public:
    static constexpr const char* SCREEN_TITLE = SplashScreen_TITLE;

    SplashScreen(TFTFunc tft, KeyboardManager* km);

    // Override render and onClick as needed
    TFT_eSPI& render() override;
    String onClick(uint16_t x, uint16_t y) override;

    // SplashScreen's whole surface advances to MainMenu, so use a longer
    // debounce for the same reason as VoidScreen -- avoids one physical
    // tap being counted as several taps across consecutive screens.
    unsigned long getPressDebounceMs() const override { return 500; }
};

#endif // SPLASHSCREEN_H
