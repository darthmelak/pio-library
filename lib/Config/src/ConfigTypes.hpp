#ifndef ConfigTypes_h
#define ConfigTypes_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define CONF_T_STR "str"
#define CONF_T_INT "int"
#define CONF_T_JSON "json"

typedef std::function<void(const String&)> str_update_cb;
typedef std::function<void(int)> int_update_cb;
typedef std::function<void(const JsonDocument&)> json_update_cb;

class StringConfig {
  public:
    StringConfig(const String& name, const String& defaultValue, str_update_cb cb = nullptr);
    virtual ~StringConfig() {};
    String getName() const;
    void setNext(StringConfig *next);
    StringConfig *getNext() const;
    virtual bool setValue(const String& value);
    virtual String getValue() const;
    virtual const char *getType() const;
    void setCb(str_update_cb cb);
  private:
    String name;
    StringConfig *next;
  protected:
    String value;
    str_update_cb cb;
};

class IntConfig: public StringConfig {
  public:
    IntConfig(const String& name, int defaultValue, int_update_cb cb = nullptr);
    bool setValue(int value);
    String getValue() const;
    int getIntVal() const;
    const char *getType() const;
    void setCb(int_update_cb cb);
  protected:
    int value;
    int_update_cb cb;
};

class SavedStringConfig {
  public:
    SavedStringConfig(const String& name, const String& defaultValue, str_update_cb cb = nullptr, int offset = 0, int length = 64);
    virtual ~SavedStringConfig() {};
    virtual void setup();
    String getName() const;
    void setNext(SavedStringConfig *next);
    SavedStringConfig *getNext() const;
    virtual bool setValue(const String& value);
    virtual String getValue() const;
    int getLength() const;
    virtual const char *getType() const;
    void setCb(str_update_cb cb);
  private:
    String name;
    SavedStringConfig *next;
  protected:
    void save(const String& value);
    String value;
    int offset;
    int length;
    str_update_cb cb;
};

class SavedIntConfig: public SavedStringConfig {
  public:
    SavedIntConfig(const String& name, int defaultValue, int_update_cb cb = nullptr, int offset = 0, int length = 6);
    void setup();
    bool setValue(int value);
    String getValue() const;
    int getIntVal() const;
    const char *getType() const;
    void setCb(int_update_cb cb);
  protected:
    int value;
    int_update_cb cb;
};

class SavedJsonConfig: public SavedStringConfig {
  public:
    SavedJsonConfig(const String& name, const String& defaultValue, json_update_cb cb = nullptr, int offset = 0, int length = 64);
    void setup();
    bool setValue(const JsonDocument& value);
    String getValue() const;
    JsonVariantConst getJsonVal() const;
    const char *getType() const;
    void setCb(json_update_cb cb);
  protected:
    DynamicJsonDocument value;
    json_update_cb cb;
};

#endif
