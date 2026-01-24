#include "AudioManager.h"

AudioManager::AudioManager(int bclkPin, int dinPin, SDManager* sdManager)
  : _bclkPin(bclkPin), _dinPin(dinPin), _i2s(OUTPUT), _sdManager(sdManager), _mp3(_i2s), _playing(false), _paused(false) {}

bool AudioManager::begin(uint32_t sampleRate, uint8_t bitsPerSample) {
  _i2s.setBCLK(_bclkPin);
  _i2s.setDATA(_dinPin);
  _i2s.setBitsPerSample(bitsPerSample);

  // if (!_i2s.begin(sampleRate)) return false;

  return true;

  // return _mp3.begin();
}

bool AudioManager::isPlaying() const {
  return _playing && !_paused;
}

bool AudioManager::isPaused() const {
  return _paused;
}

void AudioManager::playTone(float freq, int duration_ms) {
  stop(); // Ensure MP3 and I2S are reset
    _i2s.setBCLK(_bclkPin);
    _i2s.setDATA(_dinPin);
    _i2s.setBitsPerSample(16);
    _i2s.begin(44100);
    _mode = MODE_TONE;

    unsigned long samples = (44100UL * duration_ms) / 1000UL;
    float phase = 0;
    float phaseInc = 2 * PI * freq / 44100.0;

    for (unsigned long i = 0; i < samples; ++i) {
        int16_t sample = (int16_t)(32767 * sin(phase));
        phase += phaseInc;
        if (phase > 2 * PI) phase -= 2 * PI;
        _i2s.write(sample); // Left
        _i2s.write(sample); // Right
    }
}

bool AudioManager::playMP3(const char* path) {
  stop();
  _mp3.begin();
  _mode = MODE_MP3;
  _currentPath = path;

  if (_mp3file) _mp3file.close();
  _mp3file = _sdManager -> open(path, "r");

  if (!_mp3file) {
    _playing = false;
    _paused = false;
    return false;
  }

  _playing = true;
  _paused = false;
  return true;
}

void AudioManager::loop() {
  if (_playing && !_paused && _mp3file) {
    static uint8_t buffer[512];
    // Feed data while decoder has room and file has data
    while (_mp3.availableForWrite() > 512 && _mp3file.available()) {
      int len = _mp3file.read(buffer, 512);
      if (len > 0) {
        _mp3.write(buffer, len);
      }
      // If less than 512 bytes, we've hit EOF
      if (len != 512) {
        _mp3file.close();
        _playing = false;
        break;
      }
    }
    // If file is closed and buffer is empty, stop playing
    if (!_mp3file && _mp3.available() == 0) {
      _playing = false;
    }
  }
}

void AudioManager::stop() {
  if (_mode == MODE_MP3) {
    if (_mp3file) _mp3file.close();
    _playing = false;
    _paused = false;
    _currentPath = "";
  }

  _mode = MODE_NONE;
}

void AudioManager::pause() {
  if (_playing && !_paused) {
    _paused = true;
  }
}

void AudioManager::resume() {
  if (_playing && _paused) {
    _paused = false;
  }
}

void AudioManager::loadTracksFromDirectory(const char* path) {
  _tracks.clear();
  std::vector<String> files = _sdManager->listDirectory(path, LIST_FILES);
  
  for (const String& file : files) {
    // Check if file has .mp3 extension (case insensitive)
    String lower = file;
    lower.toLowerCase();
    if (lower.endsWith(".mp3")) {
      // Store full path
      String fullPath = String(path);
      if (!fullPath.endsWith("/")) fullPath += "/";
      fullPath += file;
      _tracks.push_back(fullPath);
    }
  }
  
  Serial.print("Loaded ");
  Serial.print(_tracks.size());
  Serial.println(" MP3 tracks");
}

const std::vector<String>& AudioManager::getTracks() const {
  return _tracks;
}

size_t AudioManager::getTrackCount() const {
  return _tracks.size();
}

// Friend function implementation - for use during screen transitions
void handleAudioDuringScreenChange(AudioManager& audio, bool pause) {
  if (pause) {
    audio.pause();
  } else {
    audio.resume();
  }
}
