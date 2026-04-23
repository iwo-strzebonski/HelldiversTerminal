# HelldiversTerminal
RPi Pico based Hellpad from Helldivers 2

## Setup & Installation

### 1. Board Support

Install the **Raspberry Pi Pico / RP2040** board package in the Arduino IDE:

1. Open **File → Preferences** and add this URL to *Additional Boards Manager URLs*:
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
2. Open **Tools → Board → Boards Manager**, search for **"Pico"**, and install **Raspberry Pi Pico/RP2040** by Earle F. Philhower.
3. Select **Tools → Board → Raspberry Pi RP2040 Boards → Raspberry Pi Pico**.

### 2. Required Libraries

Install all of the following libraries via **Sketch → Include Library → Manage Libraries** (or the PlatformIO equivalent):

| Library | Author | Notes |
|---------|--------|-------|
| **TFT_eSPI** | Bodmer | Display & touch driver — **requires extra configuration (see below)** |
| **SD** | Arduino | SD card file system |
| **I2S** | Arduino (RP2040 core) | I2S audio output — included with the RP2040 board package |
| **BackgroundAudioMP3** | Earle F. Philhower | MP3 decoding in the background |
| **Keyboard** | Arduino | USB HID keyboard emulation |

### 3. Configure TFT_eSPI (`User_Setup.h`)

TFT_eSPI does **not** auto-detect hardware — you must tell it which pins and driver to use.

The repository includes a pre-configured `User_Setup.h` for this project's hardware (ILI9488 display + XPT2046 touch on a Raspberry Pi Pico). You need to copy it into the installed library folder, **replacing** the default file:

```
<Arduino Libraries folder>/TFT_eSPI/User_Setup.h
```

The Arduino libraries folder is typically:
- **Windows**: `Documents\Arduino\libraries\TFT_eSPI\`
- **macOS**: `~/Documents/Arduino/libraries/TFT_eSPI/`
- **Linux**: `~/Arduino/libraries/TFT_eSPI/`

> **Why this is required:** the `HelldiversTerminal.ino` sketch sets `#define USER_SETUP_LOADED 1` at the top, which tells TFT_eSPI to skip its own internal configuration and use the file you provide. Without copying `User_Setup.h`, the library will use wrong defaults and the display will not work.

Key settings defined in `User_Setup.h`:

| Setting | Value | Purpose |
|---------|-------|---------|
| `ILI9488_DRIVER` | — | Selects the ILI9488 display driver |
| `TFT_MISO/MOSI/SCLK` | 0 / 3 / 2 | SPI0 pins for the display |
| `TFT_CS` | 20 | Display chip select |
| `TFT_DC` | 16 | Data/command pin |
| `TFT_RST` | 21 | Display reset |
| `TFT_IRQ` | 15 | Touch interrupt |
| `TOUCH_CS` | 14 | Touch controller chip select |
| `SPI_FREQUENCY` | 27 MHz | SPI clock for display writes |
| `SPI_TOUCH_FREQUENCY` | 2.5 MHz | Lower SPI clock required by XPT2046 |

### 4. SD Card Setup

Format an SD card as **FAT32** and create an `/audio/` directory. Place MP3 files inside it:

```
/audio/
  track1.mp3
  track2.mp3
  ...
```

The firmware auto-discovers all MP3 files in `/audio/` at startup via `AudioManager::loadTracksFromDirectory()`.

---

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
