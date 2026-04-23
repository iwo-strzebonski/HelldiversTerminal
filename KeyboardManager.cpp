#include "KeyboardManager.h"

KeyboardManager::KeyboardManager() {}

void KeyboardManager::begin() {
  Keyboard.begin();
}

void KeyboardManager::sendKey(char key) {
  toggleKey(key, true);
  delay(KEY_DELAY);
  toggleKey(key, false);
}

void KeyboardManager::holdKey(char key) {
  Keyboard.press(key);
}

void KeyboardManager::releaseKey(char key) {
  Keyboard.release(key);
}

void KeyboardManager::toggleKey(char key, bool newState) {
  if (newState) {
    holdKey(key);
  } else {
    releaseKey(key);
  }
}

void KeyboardManager::sendArrow(int dir) {
  Serial.println(dir);

  switch (dir) {
    case 0:
      sendKey(KEY_UP_ARROW);
      break;

    case 1:
      sendKey(KEY_RIGHT_ARROW);
      break;

    case 2:
      sendKey(KEY_DOWN_ARROW);
      break;

    case 3:
      sendKey(KEY_LEFT_ARROW);
      break;
  }
}

/*void buttonpress(int button) {
  //Handle a button press. Buttons are 0 indexed, meaning that the first button is button 0.
  switch(button){
    case 0:
     // Sending a combination of CTRL, ALT and a Function key.
     Serial.println("Button 1 pressed");
     Keyboard.press(KEY_LEFT_CTRL);
     delay(keydelay);
     Keyboard.press(KEY_LEFT_ALT);
     delay(keydelay);
     Keyboard.press(KEY_F1);
     delay(keydelay);
     Keyboard.releaseAll();
     break;
    case 1:
     // Sending a combination of CTRL, ALT and a Function key.
     Serial.println("Button 2 pressed");
     Keyboard.press(KEY_LEFT_CTRL);
     delay(keydelay);
     Keyboard.press(KEY_LEFT_ALT);
     delay(keydelay);
     Keyboard.press(KEY_F2);
     delay(keydelay);
     Keyboard.releaseAll();
     break;
    case 2:
     // Sending a combination of CTRL, ALT and a Function key.
     Serial.println("Button 3 pressed");
     Keyboard.press(KEY_LEFT_CTRL);
     delay(keydelay);
     Keyboard.press(KEY_LEFT_ALT);
     delay(keydelay);
     Keyboard.press(KEY_F3);
     delay(keydelay);
     Keyboard.releaseAll();
     break;
    case 3:
     Serial.println("Button 4 pressed");
     // Typing something in the active window
     Keyboard.print("Can you type as fast as this?");
     break;
    case 4:
     Serial.println("Button 5 pressed");
     // Using the GUI key + R to start an application
     Keyboard.press(KEY_LEFT_GUI); // GUI is WIN on windows, CMD on MacOs and GUI on Linux
     delay(keydelay);
     Keyboard.print("r");
     delay(keydelay);
     Keyboard.releaseAll();
     delay(keydelay);
     Keyboard.print("cmd");
     delay(keydelay);
     Keyboard.write(KEY_RETURN);
     break;
    case 5:
     Serial.println("Button 6 pressed");
     // println() presses RETURN at the end of printing characters
     Keyboard.println("Can you type as fast as this?");
     break;
    case 6:
     Serial.println("Button 7 pressed");
     // Using println() we can skip a seperate RETURN press
     Keyboard.press(KEY_LEFT_GUI);
     delay(keydelay);
     Keyboard.print("r");
     delay(keydelay);
     Keyboard.releaseAll();
     delay(keydelay);
     Keyboard.println("cmd");
     break;
    case 7:
     Serial.println("Button 8 pressed");
     // ALT + TAB
     Keyboard.press(KEY_LEFT_ALT);
     delay(keydelay);
     Keyboard.write(KEY_TAB);
     Keyboard.releaseAll();
     break;
    case 8:
     Serial.println("Button 9 pressed");
     // Open an URL. Typing an URL in WIN + R (run) will use your default browser to open an URL
     Keyboard.press(KEY_LEFT_GUI);
     delay(keydelay);
     Keyboard.print("r");
     delay(keydelay);
     Keyboard.releaseAll();
     delay(keydelay);
     Keyboard.println("https://www.youtube.com/dustinwatts");
     break;
    case 9:
     Serial.println("Button 10 pressed");
     // Nothing for now
     break;
    case 10:
     Serial.println("Button 11 pressed");
     // Nothing for now
     break;
    case 11:
     Serial.println("Button 12 pressed");
     // Nothing for now
     break;
  }
}*/
