#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "Screen.h"
#include "Buttons.h"
#include "AudioManager.h"

#define MusicPlayer_TITLE "MUSIC_PLAYER"

// Colors
#define MP_BG_COLOR 0x1947      // Dark blue background (matching other screens)
#define MP_TEXT_COLOR 0xFFFF    // White text
#define MP_HIGHLIGHT_COLOR 0xFD20  // Orange/yellow highlight
#define MP_BUTTON_COLOR 0x4208  // Gray button background

// Layout constants
#define MP_TRACKS_PER_PAGE 4
#define MP_TRACK_HEIGHT 50
#define MP_TRACK_WIDTH 380
#define MP_TRACK_MARGIN 10
#define MP_CONTROLS_Y 280

class MusicPlayer : public Screen {
  public:
    static constexpr const char* SCREEN_TITLE = MusicPlayer_TITLE;

    MusicPlayer(TFTFunc tft, KeyboardManager* km, AudioManager* audioManager);

    TFT_eSPI& render() override;
    String onClick(uint16_t x, uint16_t y) override;

  private:
    AudioManager* _audioManager;
    
    int _selectedTrack;      // Currently selected track index
    int _scrollOffset;       // For scrolling through tracks
    bool _loopEnabled;       // Loop current track
    
    Button backButton;

    void drawTrackList();
    void drawControls();
    void drawTrackTile(int index, int yPos, bool selected);
    
    // Get display name from full path
    String getTrackName(const String& path);
};

#endif // MUSICPLAYER_H
