#ifndef SDMANAGER_H
#define SDMANAGER_H

#include <SD.h>
#include <vector>

// Enumeration for listing types
enum ListType { LIST_FILES, LIST_DIRECTORIES, LIST_ALL };

class SDManager {
  public:
    SDManager(int miso, int mosi, int sclk, int cs);
    bool begin();
    File open(const char* path, const char* mode);
    bool exists(const char* path);
    bool remove(const char* path);
    
    // List directory contents non-recursively
    std::vector<String> listDirectory(const char* path, ListType type = LIST_ALL);

  private:
    int _miso;
    int _mosi;
    int _sclk;
    int _cs;
};

#endif
