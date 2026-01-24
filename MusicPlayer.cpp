#include "MusicPlayer.h"
#include "MainMenu.h"

MusicPlayer::MusicPlayer(TFTFunc tft, KeyboardManager* km, AudioManager* audioManager)
  : Screen(tft, km),
    _audioManager(audioManager),
    _selectedTrack(-1),
    _scrollOffset(0),
    _loopEnabled(false),
    backButton(tft, BUTTON_HOME, BUTTON_PRESSED, BUTTON_WIDTH, BUTTON_HEIGHT, 386, 0, false)
{}

TFT_eSPI& MusicPlayer::render() {
  TFT_eSPI& tft = Screen::render();
  
  // Draw title
  tft.setTextColor(MP_HIGHLIGHT_COLOR);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("MUSIC PLAYER");
  
  // Draw back button
  backButton.draw();
  
  // Draw track list
  drawTrackList();
  
  // Draw playback controls
  drawControls();
  
  return tft;
}

void MusicPlayer::drawTrackList() {
  TFT_eSPI& tft = getTFT();
  
  const std::vector<String>& tracks = _audioManager->getTracks();
  
  if (tracks.empty()) {
    tft.setTextColor(MP_TEXT_COLOR);
    tft.setTextSize(1);
    tft.setCursor(10, 60);
    tft.print("No MP3 files found in /audio");
    return;
  }
  
  int startY = 50;
  int maxTracks = min((int)tracks.size() - _scrollOffset, MP_TRACKS_PER_PAGE);
  
  for (int i = 0; i < maxTracks; i++) {
    int trackIndex = i + _scrollOffset;
    bool isSelected = (trackIndex == _selectedTrack);
    drawTrackTile(trackIndex, startY + i * (MP_TRACK_HEIGHT + MP_TRACK_MARGIN), isSelected);
  }
  
  // Draw scroll indicators if needed
  if (_scrollOffset > 0) {
    tft.setTextColor(MP_HIGHLIGHT_COLOR);
    tft.setCursor(400, 50);
    tft.print("^");
  }
  
  if (_scrollOffset + MP_TRACKS_PER_PAGE < (int)tracks.size()) {
    tft.setTextColor(MP_HIGHLIGHT_COLOR);
    tft.setCursor(400, 200);
    tft.print("v");
  }
}

void MusicPlayer::drawTrackTile(int index, int yPos, bool selected) {
  TFT_eSPI& tft = getTFT();
  
  const std::vector<String>& tracks = _audioManager->getTracks();
  if (index < 0 || index >= (int)tracks.size()) return;
  
  uint16_t bgColor = selected ? MP_HIGHLIGHT_COLOR : MP_BUTTON_COLOR;
  uint16_t textColor = selected ? MP_BG_COLOR : MP_TEXT_COLOR;
  
  // Draw tile background
  tft.fillRoundRect(10, yPos, MP_TRACK_WIDTH, MP_TRACK_HEIGHT, 5, bgColor);
  
  // Draw track name
  tft.setTextColor(textColor);
  tft.setTextSize(1);
  tft.setCursor(20, yPos + 18);
  
  String trackName = getTrackName(tracks[index]);
  // Truncate if too long
  if (trackName.length() > 40) {
    trackName = trackName.substring(0, 37) + "...";
  }
  tft.print(trackName);
  
  // Draw playing indicator if this track is currently playing
  if (_audioManager->isPlaying() && index == _selectedTrack) {
    tft.fillTriangle(
      370, yPos + 15,
      370, yPos + 35,
      385, yPos + 25,
      textColor
    );
  }
}

void MusicPlayer::drawControls() {
  TFT_eSPI& tft = getTFT();
  
  // Control buttons area
  int btnWidth = 70;
  int btnHeight = 35;
  int spacing = 15;
  int startX = 40;
  
  // Play/Pause button
  bool isPlaying = _audioManager->isPlaying();
  uint16_t playColor = isPlaying ? MP_HIGHLIGHT_COLOR : MP_BUTTON_COLOR;
  tft.fillRoundRect(startX, MP_CONTROLS_Y, btnWidth, btnHeight, 5, playColor);
  tft.setTextColor(isPlaying ? MP_BG_COLOR : MP_TEXT_COLOR);
  tft.setTextSize(1);
  tft.setCursor(startX + 10, MP_CONTROLS_Y + 12);
  tft.print(isPlaying ? "PAUSE" : "PLAY");
  
  // Stop button
  tft.fillRoundRect(startX + btnWidth + spacing, MP_CONTROLS_Y, btnWidth, btnHeight, 5, MP_BUTTON_COLOR);
  tft.setTextColor(MP_TEXT_COLOR);
  tft.setCursor(startX + btnWidth + spacing + 15, MP_CONTROLS_Y + 12);
  tft.print("STOP");
  
  // Loop button
  uint16_t loopColor = _loopEnabled ? MP_HIGHLIGHT_COLOR : MP_BUTTON_COLOR;
  tft.fillRoundRect(startX + 2 * (btnWidth + spacing), MP_CONTROLS_Y, btnWidth, btnHeight, 5, loopColor);
  tft.setTextColor(_loopEnabled ? MP_BG_COLOR : MP_TEXT_COLOR);
  tft.setCursor(startX + 2 * (btnWidth + spacing) + 15, MP_CONTROLS_Y + 12);
  tft.print("LOOP");
  
  // Prev button
  tft.fillRoundRect(startX + 3 * (btnWidth + spacing), MP_CONTROLS_Y, btnWidth, btnHeight, 5, MP_BUTTON_COLOR);
  tft.setTextColor(MP_TEXT_COLOR);
  tft.setCursor(startX + 3 * (btnWidth + spacing) + 15, MP_CONTROLS_Y + 12);
  tft.print("PREV");
  
  // Next button
  tft.fillRoundRect(startX + 4 * (btnWidth + spacing), MP_CONTROLS_Y, btnWidth, btnHeight, 5, MP_BUTTON_COLOR);
  tft.setTextColor(MP_TEXT_COLOR);
  tft.setCursor(startX + 4 * (btnWidth + spacing) + 15, MP_CONTROLS_Y + 12);
  tft.print("NEXT");
}

String MusicPlayer::getTrackName(const String& path) {
  // Extract filename from full path
  int lastSlash = path.lastIndexOf('/');
  String filename = (lastSlash >= 0) ? path.substring(lastSlash + 1) : path;
  
  // Remove .mp3 extension
  if (filename.endsWith(".mp3") || filename.endsWith(".MP3")) {
    filename = filename.substring(0, filename.length() - 4);
  }
  
  return filename;
}

String MusicPlayer::onClick(uint16_t x, uint16_t y) {
  // Check back button
  if (backButton.onClick(x, y, true)) {
    backButton.draw(true);
    delay(BUTTON_PRESSED_DELAY);
    return MainMenu::SCREEN_TITLE;
  }
  
  const std::vector<String>& tracks = _audioManager->getTracks();
  
  // Check track tile clicks
  int startY = 50;
  for (int i = 0; i < MP_TRACKS_PER_PAGE && (i + _scrollOffset) < (int)tracks.size(); i++) {
    int trackIndex = i + _scrollOffset;
    int tileY = startY + i * (MP_TRACK_HEIGHT + MP_TRACK_MARGIN);
    
    if (x >= 10 && x <= 10 + MP_TRACK_WIDTH &&
        y >= tileY && y <= tileY + MP_TRACK_HEIGHT) {
      _selectedTrack = trackIndex;
      // Start playing the selected track
      _audioManager->playMP3(tracks[trackIndex].c_str());
      render();
      return "";
    }
  }
  
  // Check scroll up (top-right area)
  if (x >= 400 && y >= 40 && y < 100 && _scrollOffset > 0) {
    _scrollOffset--;
    render();
    return "";
  }
  
  // Check scroll down (bottom-right area)  
  if (x >= 400 && y >= 180 && y < 250 && _scrollOffset + MP_TRACKS_PER_PAGE < (int)tracks.size()) {
    _scrollOffset++;
    render();
    return "";
  }
  
  // Control buttons
  int btnWidth = 70;
  int btnHeight = 35;
  int spacing = 15;
  int startX = 40;
  
  if (y >= MP_CONTROLS_Y && y <= MP_CONTROLS_Y + btnHeight) {
    // Play/Pause
    if (x >= startX && x <= startX + btnWidth) {
      if (_audioManager->isPlaying()) {
        _audioManager->pause();
      } else if (_audioManager->isPaused()) {
        _audioManager->resume();
      } else if (_selectedTrack >= 0 && _selectedTrack < (int)tracks.size()) {
        _audioManager->playMP3(tracks[_selectedTrack].c_str());
      }
      render();
      return "";
    }
    
    // Stop
    if (x >= startX + btnWidth + spacing && x <= startX + 2 * btnWidth + spacing) {
      _audioManager->stop();
      render();
      return "";
    }
    
    // Loop
    if (x >= startX + 2 * (btnWidth + spacing) && x <= startX + 3 * btnWidth + 2 * spacing) {
      _loopEnabled = !_loopEnabled;
      render();
      return "";
    }
    
    // Prev
    if (x >= startX + 3 * (btnWidth + spacing) && x <= startX + 4 * btnWidth + 3 * spacing) {
      if (!tracks.empty()) {
        _selectedTrack = (_selectedTrack - 1 + tracks.size()) % tracks.size();
        _audioManager->playMP3(tracks[_selectedTrack].c_str());
        // Adjust scroll if needed
        if (_selectedTrack < _scrollOffset) {
          _scrollOffset = _selectedTrack;
        }
        render();
      }
      return "";
    }
    
    // Next
    if (x >= startX + 4 * (btnWidth + spacing) && x <= startX + 5 * btnWidth + 4 * spacing) {
      if (!tracks.empty()) {
        _selectedTrack = (_selectedTrack + 1) % tracks.size();
        _audioManager->playMP3(tracks[_selectedTrack].c_str());
        // Adjust scroll if needed
        if (_selectedTrack >= _scrollOffset + MP_TRACKS_PER_PAGE) {
          _scrollOffset = _selectedTrack - MP_TRACKS_PER_PAGE + 1;
        }
        render();
      }
      return "";
    }
  }
  
  return "";
}
