#ifndef Configuration_h
#define Configuration_h

#ifdef ESP8266
  #include <EEPROM.h>
#else
  #include <Preferences.h>
#endif
#include "ConfigTypes.hpp"

#define PREFS_NAMESPACE "settings"

class Configuration {
  public:
    Configuration(bool debug = false);
    Configuration& add(String name, String defaultValue);
    Configuration& add(String name, int defaultValue);
    StringConfig *get(String name);
    IntConfig *getInt(String name);
  protected:
    bool debug;
    StringConfig *first;
    StringConfig *last;
    void chain(StringConfig *config);
};

class SavedConfiguration: public Configuration {
  public:
    SavedConfiguration(bool debug = false);
    void setup();
    int getSize();
    SavedConfiguration& add(String name, String defaultValue);
    SavedConfiguration& add(String name, int defaultValue);
    SavedStringConfig *get(String name);
    SavedIntConfig *getInt(String name);
  protected:
    int size;
    SavedStringConfig *first;
    SavedStringConfig *last;
    void chain(SavedStringConfig *config);
    #ifdef ESP32
    Preferences preferences;
    #endif
};

#endif
