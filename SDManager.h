#ifndef SDMANAGER_H
#define SDMANAGER_H

#include <SD.h>

class SDManager {
  public:
    SDManager(int miso, int mosi, int sclk, int cs);
    bool begin();
    File open(const char* path, const char* mode);
    bool exists(const char* path);
    bool remove(const char* path);

  private:
    int _miso;
    int _mosi;
    int _sclk;
    int _cs;
};

#endif
