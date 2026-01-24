#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <I2S.h>
#include <BackgroundAudioMP3.h>
#include "SDManager.h" // Use your SDManager class
#include <vector>

enum AudioMode { MODE_NONE, MODE_TONE, MODE_MP3 };

// Forward declaration for friend function
class Display;

class AudioManager {
  public:
    AudioManager(int bclkPin, int dinPin, SDManager* sdManager);
    bool begin(uint32_t sampleRate = 44100, uint8_t bitsPerSample = 16);

    void playTone(float freq, int duration_ms);

    bool playMP3(const char* path);
    void loop(); // Call this in your main loop if playing MP3
    void stop();
    void pause();
    void resume();
    bool isPlaying() const;
    bool isPaused() const;
    
    // Initialize with available MP3 files from directory
    void loadTracksFromDirectory(const char* path);
    const std::vector<String>& getTracks() const;
    size_t getTrackCount() const;

    // Friend function for Display to access audio state during transitions
    friend void handleAudioDuringScreenChange(AudioManager& audio, bool pause);

  private:
    AudioMode _mode = MODE_NONE;

    int _bclkPin, _dinPin;
    bool _playing;
    bool _paused;
    String _currentPath; // Store path for resume

    I2S _i2s;
    File _mp3file;
    BackgroundAudioMP3 _mp3;

    SDManager* _sdManager;
    std::vector<String> _tracks; // List of available MP3 files
};

// Friend function declaration
void handleAudioDuringScreenChange(AudioManager& audio, bool pause);

#endif
