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

// Software debounce window for the resistive touch panel. The XPT2046 IRQ
// line can briefly chatter at the moment a finger first contacts the
// panel; we ignore further press events that arrive within this window.
long PRESS_DELAY = 50;
unsigned long previousMillis = 0;
bool setupOk = false;

// Set from the TFT_IRQ falling-edge ISR (touchISR) and consumed in
// loop(). `volatile` because it crosses the ISR/main boundary; on the
// RP2040 a single byte read/write is atomic so no critical section is
// needed around it.
volatile bool touchPending = false;

// ISR: deliberately does the absolute minimum. We do NOT touch the SPI
// bus (the XPT2046 share with the TFT/SD), call any USB HID function, or
// use delay() from here -- TinyUSB does not get to run inside an ISR, and
// SPI transactions started from interrupt context can collide with ones
// already in flight on the main thread. Just flag the event and let
// loop() handle it on the next iteration.
void touchISR() {
  touchPending = true;
}

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
  // We attach a falling-edge interrupt so we get notified the instant a
  // finger lands on the panel instead of having to poll every loop
  // iteration. The ISR itself only sets a flag; coordinate reads (which
  // need the SPI bus) and the USB HID dispatch happen back in loop().
  pinMode(TFT_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TFT_IRQ), touchISR, FALLING);

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

  // The TFT_IRQ falling-edge ISR (touchISR) sets touchPending the instant
  // the user touches the panel. We do the actual SPI read and USB HID
  // dispatch here in loop() -- not in the ISR -- because:
  //  * the XPT2046 shares the SPI bus with the TFT and the SD card, so a
  //    transaction started from interrupt context could collide with one
  //    already in flight on the main thread;
  //  * TinyUSB's task does not run inside an ISR, so press()/release()
  //    from there would queue HID reports without ever flushing them and
  //    the host would see "stuck" keys.
  // Falling-edge triggering also gives us natural one-event-per-tap
  // semantics: the IRQ stays LOW for as long as the finger is down, so it
  // does NOT re-fire while held.
  if (touchPending) {
    touchPending = false;

    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= PRESS_DELAY) {
      previousMillis = currentMillis;

      uint16_t t_x = 0, t_y = 0;
      if (display.getTouch(&t_x, &t_y)) {
        display.onTouch(t_x, t_y);
      }
    }
  }

  delay(1);
}
