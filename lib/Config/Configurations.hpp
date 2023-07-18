#ifndef Configuration_h
#define Configuration_h

#ifdef ESP8266
  #include <EEPROM.h>
#else
  #include <Preferences.h>
#endif
#include <ArduinoJson.h>
#include "ConfigTypes.hpp"

#define PREFS_NAMESPACE "settings"

class Configuration {
  public:
    Configuration(String path, bool debug = false);
    String getPath();
    Configuration& add(String name, String defaultValue);
    Configuration& add(String name, int defaultValue);
    StringConfig *get(String name) const;
    IntConfig *getInt(String name) const;
    StringConfig *getFirst() const;
    void toJson(JsonDocument& json) const;
  protected:
    String path;
    bool debug;
    StringConfig *first;
    StringConfig *last;
    void chain(StringConfig *config);
};

class SavedConfiguration: public Configuration {
  public:
    SavedConfiguration(String path, bool debug = false);
    void setup();
    int getSize();
    SavedConfiguration& add(String name, String defaultValue);
    SavedConfiguration& add(String name, int defaultValue);
    SavedStringConfig *get(String name);
    SavedIntConfig *getInt(String name);
    SavedStringConfig *getFirst();
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
