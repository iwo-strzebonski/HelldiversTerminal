#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <Keyboard.h>

#define KEY_DELAY 100

class KeyboardManager {
  public:
    KeyboardManager();

    void sendKey(char key);
    void holdKey(char key);
    void releaseKey(char key);
    void toggleKey(char key, bool newState);

    void sendArrow(int dir);
};

#endif
