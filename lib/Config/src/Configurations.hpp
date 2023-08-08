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
    virtual ~Configuration() {};
    String getPath() const;
    Configuration& add(String name, String defaultValue, str_update_cb cb = NULL);
    Configuration& add(String name, int defaultValue, int_update_cb cb = NULL);
    StringConfig *get(String name) const;
    IntConfig *getInt(String name) const;
    StringConfig *getFirst() const;
    virtual void toJson(JsonDocument& json) const;
    virtual bool fromJson(JsonDocument& json);
    virtual String getStrVal(String name) const;
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
    int getSize() const;
    SavedConfiguration& add(String name, String defaultValue, str_update_cb cb = NULL, int length = 64);
    SavedConfiguration& add(String name, int defaultValue, int_update_cb cb = NULL);
    SavedStringConfig *get(String name) const;
    SavedIntConfig *getInt(String name) const;
    SavedStringConfig *getFirst() const;
    void toJson(JsonDocument& json) const;
    bool fromJson(JsonDocument& json);
    String getStrVal(String name) const;
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
