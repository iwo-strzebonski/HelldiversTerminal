#include "SDManager.h"

SDManager::SDManager(int miso, int mosi, int sclk, int cs) 
  : _miso(miso), _mosi(mosi), _sclk(sclk), _cs(cs) {}

bool SDManager::begin() {
  SPI1.setRX(_miso);
  SPI1.setTX(_mosi);
  SPI1.setSCK(_sclk);
  return SD.begin(_cs, SPI1);
}

File SDManager::open(const char* path, const char* mode) {
  if (!exists(path)) {
    Serial.println("No file available");
  } else {
    Serial.print("Opening ");
    Serial.println(path);
  }

  return SD.open(path, mode);
}

bool SDManager::exists(const char* path) {
  return SD.exists(path);
}

bool SDManager::remove(const char* path) {
  return SD.remove(path);
}
