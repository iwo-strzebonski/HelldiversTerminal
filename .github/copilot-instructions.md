# HelldiversTerminal - Copilot Instructions

## Project Overview
This is an Arduino project for a Raspberry Pi Pico-based touchscreen terminal inspired by Helldivers. It features a TFT display with touch input, SD card storage for assets, I2S audio output, and USB HID keyboard emulation.

## Hardware Configuration
- **MCU**: Raspberry Pi Pico (RP2040)
- **Display**: ILI9488 480x320 TFT with XPT2046 touch controller
- **Audio**: I2S DAC output (PCM5102 or similar)
- **Storage**: SD card via SPI1
- **Input**: Touch screen with interrupt-driven detection

### Pin Definitions (see User_Setup.h)
- TFT: MISO=0, MOSI=3, SCLK=2, CS=20, DC=16, RST=21, IRQ=15
- Touch: CS=14
- Backlight: PWM on pin 5
- Audio I2S: BCLK=27, DIN=26
- SD Card: MISO=12, MOSI=11, SCLK=10, CS=13

## Architecture

### Screen System
The UI uses a screen-based architecture with inheritance:
- `Screen` - Abstract base class with virtual `render()` and `onClick()` methods
- Derived screens: `MainMenu`, `VoidScreen`, `SplashScreen`, `MusicPlayer`
- `Display` class manages screen switching and touch events
- Screens return the next screen's title from `onClick()` to trigger navigation

### Key Classes
- **AudioManager**: Handles I2S audio output for tones and MP3 playback. Use `pause()`/`resume()` around screen changes to prevent SPI bus contention.
- **SDManager**: Wrapper for SD card operations. Use `listDirectory()` to enumerate files.
- **Display**: Manages TFT instance and screen lifecycle. Pass AudioManager reference to handle audio during transitions.
- **Button**: Touch-enabled UI button with image support. Buttons use PROGMEM for image data.
- **KeyboardManager**: USB HID keyboard emulation for sending keystrokes.

### Audio Best Practices
- Always pause audio before screen transitions to prevent static from SPI bus conflicts
- Resume audio after rendering is complete
- Use `isPlaying()` to check playback state before pause/resume operations

## Code Conventions

### File Structure
- Header files (.h) contain class declarations and PROGMEM data declarations
- Implementation files (.cpp) contain method definitions
- `/buttons/` and `/screens/` directories contain design templates (source images)
- Button image data arrays are manually converted and stored in Buttons.h (declarations) and Buttons.cpp (PROGMEM data)

### Naming Conventions
- Class names: PascalCase (e.g., `AudioManager`, `MainMenu`)
- Methods: camelCase (e.g., `playMP3()`, `onClick()`)
- Constants: SCREAMING_SNAKE_CASE (e.g., `BUTTON_WIDTH`, `TFT_CS`)
- Screen title macros: ScreenName_TITLE (e.g., `MainMenu_TITLE`)

### Memory Management
- Use `PROGMEM` for large constant data (images, sounds)
- Prefer static allocation over dynamic allocation
- Button images are 94x94 pixels, 16-bit color (RGB565)

### Screen Implementation Pattern
```cpp
class NewScreen : public Screen {
public:
    static constexpr const char* SCREEN_TITLE = "NEW_SCREEN";
    
    NewScreen(TFTFunc tft, KeyboardManager* km);
    
    TFT_eSPI& render() override;
    String onClick(uint16_t x, uint16_t y) override;
};
```

### OOP Requirements
This project should demonstrate:
1. **Class Inheritance**: Screen base class with derived screens ✓
2. **Friend Functions**: Used for cross-class access (e.g., AudioManager ↔ Display)
3. **Operator Overloading**: Button comparison operators, AudioTrack stream operators

## Libraries Used
- TFT_eSPI - Display driver
- SD - SD card file system
- I2S - Audio output
- BackgroundAudioMP3 - MP3 decoding
- Keyboard - USB HID

## Common Tasks

### Adding a New Screen
1. Create ScreenName.h with class declaration inheriting from Screen
2. Create ScreenName.cpp with implementation
3. Add screen instance to Display class
4. Add screen to the `setScreen()` switch statement in Display.cpp

### Adding Audio Files
1. Place MP3 files in the `/audio/` directory on SD card
2. Files are auto-discovered using `SDManager::listDirectory()`
3. Use `AudioManager::playMP3()` with the file path

### Creating Button Images
1. Export as 94x94 RGB565 format
2. Convert to C array using image2cpp or similar tool
3. Store in PROGMEM using `const uint16_t name[] PROGMEM = {...};`
