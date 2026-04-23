#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <Keyboard.h>

#define KEY_DELAY 100

class KeyboardManager {
  public:
    KeyboardManager();

    // Initializes the USB HID keyboard interface. Must be called from
    // setup() (not the constructor) so that USB enumeration does not
    // start before the rest of the system is ready. Starting HID too
    // early can leave the device enumerated as HID-only with a stalled
    // CDC endpoint, which prevents the host from triggering the 1200bps
    // BOOTSEL reset used to re-flash the Pico.
    void begin();

    void sendKey(char key);
    void holdKey(char key);
    void releaseKey(char key);
    void toggleKey(char key, bool newState);

    void sendArrow(int dir);
};

#endif
