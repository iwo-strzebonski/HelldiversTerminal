#define USER_SETUP_LOADED 1 // for not loading internal USER_SETTINGS

#include "User_Setup.h"
#include "KeyboardManager.h"
#include "SDManager.h"
#include "AudioManager.h"
#include "Display.h"

KeyboardManager keyboardManager;
SDManager sdManager(SD_MISO, SD_MOSI, SD_SCLK, SD_CS);
AudioManager audioManager(BCLK_PIN, DIN_PIN, &sdManager);
Display display(&keyboardManager, &sdManager, &audioManager); 

long PRESS_DELAY = 250;
unsigned long previousMillis = 0;
bool setupOk = false;

void setup() {
  Serial.begin(115200);
  Serial.println("");

  delay(1000);

  bool sdInitialized = sdManager.begin();
  if (!sdInitialized) {
    Serial.println("SD initialization failed!");
    return;
  }

  if (!audioManager.begin()) {
    Serial.println("Audio initialization failed!");
    return;
  }

  // Load available MP3 tracks from the audio directory
  audioManager.loadTracksFromDirectory("/audio");

  Serial.println("SD and Audio initialized successfully");

  pinMode(BACKLIGHT, OUTPUT);
  display.init();

  delay(PRESS_DELAY);
  attachInterrupt(TFT_IRQ, screenPressed, LOW);

  // Bring up USB HID only after the rest of the system is ready, so that
  // a partially-initialized device still enumerates a usable CDC serial
  // endpoint and stays re-flashable over USB.
  keyboardManager.begin();

  setupOk = true;

  // audioManager.playMP3("/audio/Super Earth National Anthem.mp3");
}

void loop() {
  // If setup() failed, keep loop() light so USB CDC stays responsive and
  // the board can be re-flashed normally (no need to flash Blink via
  // BOOTSEL to recover).
  if (!setupOk) {
    delay(10);
    return;
  }

  audioManager.loop();
  delay(1);
}


void screenPressed() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < PRESS_DELAY) {
    return;
  }

  previousMillis = currentMillis;

  uint16_t t_x = 0, t_y = 0;
  bool pressed = display.getTouch(&t_x, &t_y);

  if (!pressed) {
    return;
  }

  display.onTouch(t_x, t_y);
}
