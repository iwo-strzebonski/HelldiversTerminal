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

// Software debounce window for the resistive touch panel. Edge detection
// already gives us one logical event per tap; this only exists to swallow
// the brief raw-coordinate glitches the XPT2046 can emit at the moment a
// finger first contacts the panel.
long PRESS_DELAY = 50;
unsigned long previousMillis = 0;
bool setupOk = false;
bool wasPressed = false;

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

  // The XPT2046 IRQ pin is pulled LOW while the screen is being touched.
  // We use it as a cheap "is anything pressed?" probe so we can avoid
  // hammering the SPI bus with touch reads when nothing is happening.
  pinMode(TFT_IRQ, INPUT_PULLUP);

  delay(PRESS_DELAY);

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

  // Polling the touch screen from loop() (instead of doing it from the
  // TFT_IRQ interrupt handler) is important: the XPT2046 holds IRQ LOW
  // for as long as the user keeps a finger on the panel, which means the
  // ISR would re-fire continuously. That, combined with calling
  // Keyboard.press()/release() and delay() from interrupt context (where
  // the TinyUSB stack does not run), made released arrow keys look
  // "stuck" to the host. Edge-detecting the press here guarantees one
  // press + release per tap.
  bool irqLow = digitalRead(TFT_IRQ) == LOW;
  bool pressed = false;
  uint16_t t_x = 0, t_y = 0;

  if (irqLow) {
    pressed = display.getTouch(&t_x, &t_y);
  }

  unsigned long currentMillis = millis();

  if (pressed && !wasPressed && (currentMillis - previousMillis) >= PRESS_DELAY) {
    previousMillis = currentMillis;
    display.onTouch(t_x, t_y);
  }

  wasPressed = pressed;

  delay(1);
}
