# HelldiversTerminal
RPi Pico based Hellpad from Helldivers 2

## OOP Features

This project demonstrates three key Object-Oriented Programming concepts:

### 1. Class Inheritance

**Where:** `Screen` base class → derived screen classes

**How it works:**
- `Screen` is an abstract base class defined in `Screen.h` with two pure virtual methods:
  ```cpp
  virtual TFT_eSPI& render() = 0;
  virtual String onClick(uint16_t x, uint16_t y) = 0;
  ```

- Four classes inherit from `Screen`:
  - `MainMenu` in `MainMenu.h`
  - `VoidScreen` in `VoidScreen.h`
  - `SplashScreen` in `SplashScreen.h`
  - `MusicPlayer` in `MusicPlayer.h`

- Each derived class overrides these virtual methods with their own implementation:
  ```cpp
  class MainMenu : public Screen {
      TFT_eSPI& render() override;
      String onClick(uint16_t x, uint16_t y) override;
  };
  ```

- The `Display` class can work with any `Screen*` pointer polymorphically in `setScreen()`, calling the correct `render()` method regardless of which screen type it is.

---

### 2. Friend Functions

**Where:** `AudioManager` class in `AudioManager.h`

**How it works:**
- A friend function `handleAudioDuringScreenChange()` is declared inside `AudioManager`:
  ```cpp
  class AudioManager {
      friend void handleAudioDuringScreenChange(AudioManager& audio, bool pause);
      // ... private members like _paused, _currentMode, etc.
  };
  ```

- This function is defined in `AudioManager.cpp` and can access private members of `AudioManager`

- **Why it's needed:** The `Display` class needs to pause/resume audio during screen transitions to prevent SPI bus conflicts (which cause static noise). Instead of making pause/resume public or making `Display` a friend class, this single function provides controlled access.

- Used in `Display.cpp` inside `setScreen()`:
  ```cpp
  void Display::setScreen(String title) {
      handleAudioDuringScreenChange(*audioManager, true);  // pause
      // ... render new screen ...
      handleAudioDuringScreenChange(*audioManager, false); // resume
  }
  ```

---

### 3. Operator Overloading

**Where:** `Button` class in `Buttons.h` and `Buttons.cpp`

**Three overloaded operators:**

1. **`operator==`** - Equality comparison
   ```cpp
   bool Button::operator==(const Button& other) const {
       return posX == other.posX && 
              posY == other.posY && 
              width == other.width && 
              height == other.height;
   }
   ```
   *Usage:* Compare if two buttons occupy the same position/size

2. **`operator!=`** - Inequality comparison
   ```cpp
   bool Button::operator!=(const Button& other) const {
       return !(*this == other);
   }
   ```
   *Usage:* Check if two buttons are different

3. **`operator()`** - Function call operator (hit testing)
   ```cpp
   bool Button::operator()(uint16_t x, uint16_t y) const {
       return x >= posX && x < (posX + width) && 
              y >= posY && y < (posY + height);
   }
   ```
   *Usage:* Check if a touch point is within button bounds:
   ```cpp
   Button myButton(...);
   if (myButton(touchX, touchY)) {
       // Point is inside button
   }
   ```

---

### Summary Table

| OOP Feature | Location | Purpose |
|-------------|----------|---------|
| **Inheritance** | `Screen` → `MainMenu`, `VoidScreen`, `SplashScreen`, `MusicPlayer` | Polymorphic screen rendering and click handling |
| **Friend Function** | `handleAudioDuringScreenChange()` in `AudioManager` | Allow `Display` to control audio without exposing internals |
| **Operator Overloading** | `==`, `!=`, `()` in `Button` | Button comparison and hit testing |
