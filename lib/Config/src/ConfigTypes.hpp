#ifndef ConfigTypes_h
#define ConfigTypes_h

#include <Arduino.h>
#include <EEPROM.h>

#define CONF_T_STR "str"
#define CONF_T_INT "int"

typedef std::function<void(String)> str_update_cb;
typedef std::function<void(int)> int_update_cb;

class StringConfig {
  public:
    StringConfig(const String& name, const String& defaultValue, str_update_cb cb = NULL);
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
    IntConfig(const String& name, int defaultValue, int_update_cb cb = NULL);
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
    SavedStringConfig(const String& name, const String& defaultValue, str_update_cb cb = NULL, int offset = 0, int length = 64);
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
    String value;
    int offset;
    int length;
    str_update_cb cb;
    #ifdef ESP32
    #endif
};

class SavedIntConfig: public SavedStringConfig {
  public:
    SavedIntConfig(const String& name, int defaultValue, int_update_cb cb = NULL, int offset = 0, int length = 6);
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

#endif
