#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <I2S.h>
#include <BackgroundAudioMP3.h>
#include "SDManager.h" // Use your SDManager class

enum AudioMode { MODE_NONE, MODE_TONE, MODE_MP3 };

class AudioManager {
  public:
    AudioManager(int bclkPin, int dinPin, SDManager* sdManager);
    bool begin(uint32_t sampleRate = 44100, uint8_t bitsPerSample = 16);

    void playTone(float freq, int duration_ms);

    bool playMP3(const char* path);
    void loop(); // Call this in your main loop if playing MP3
    void stop();
    bool isPlaying() const;

  private:
    AudioMode _mode = MODE_NONE;

    int _bclkPin, _dinPin;
    bool _playing;

    I2S _i2s;
    File _mp3file;
    BackgroundAudioMP3 _mp3;

    SDManager* _sdManager;
};

#endif
