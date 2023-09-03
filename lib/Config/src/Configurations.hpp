#ifndef Configurations_h
#define Configurations_h

#include <ArduinoJson.h>
#include "ConfigTypes.hpp"

#define PREFS_NAMESPACE "settings"

class Configuration {
  public:
    Configuration(bool debug = false);
    Configuration(const char* path, bool debug = false);
    virtual ~Configuration() {};
    void setPath(const char* path);
    String getPath() const;
    Configuration& add(const String& name, const String& defaultValue, str_update_cb cb = nullptr);
    Configuration& add(const String& name, int defaultValue, int_update_cb cb = nullptr);
    StringConfig *get(const String& name) const;
    IntConfig *getInt(const String& name) const;
    StringConfig *getFirst() const;
    virtual void toJson(JsonDocument& json) const;
    virtual bool fromJson(JsonDocument& json);
    virtual String getStrVal(const String& name) const;
  protected:
    String path;
    bool debug;
    StringConfig *first;
    StringConfig *last;
    void chain(StringConfig *config);
};

class SavedConfiguration: public Configuration {
  public:
    SavedConfiguration(bool debug = false);
    SavedConfiguration(const char* path, bool debug = false);
    void begin();
    int getSize() const;
    SavedConfiguration& add(const String& name, const String& defaultValue, str_update_cb cb = nullptr, int length = 64);
    SavedConfiguration& add(const String& name, int defaultValue, int_update_cb cb = nullptr);
    SavedConfiguration& addJson(const String& name, const String& defaultValue, json_update_cb cb = nullptr, int length = 256);
    SavedStringConfig *get(const String& name) const;
    SavedIntConfig *getInt(const String& name) const;
    SavedJsonConfig *getJson(const String& name) const;
    SavedStringConfig *getFirst() const;
    void toJson(JsonDocument& json) const;
    bool fromJson(JsonDocument& json);
    String getStrVal(const String& name) const;
  protected:
    SavedStringConfig *first;
    SavedStringConfig *last;
    void chain(SavedStringConfig *config);
    static int size;
    static bool initialized;
};

#endif
