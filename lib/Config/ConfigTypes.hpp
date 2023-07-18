#ifndef ConfigTypes_h
#define ConfigTypes_h

#include <Arduino.h>
#ifdef ESP32
  #include <Preferences.h>
#endif

class StringConfig {
  public:
    StringConfig(String name, String defaultValue);
    virtual ~StringConfig() {};
    String getName() const;
    void setNext(StringConfig *next);
    StringConfig *getNext() const;
    virtual void setValue(String value);
    virtual String getValue() const;
  private:
    String name;
    StringConfig *next;
  protected:
    String value;
};

class IntConfig: public StringConfig {
  public:
    IntConfig(String name, int defaultValue);
    void setValue(int value);
    String getValue() const;
    int getIntVal() const;
  protected:
    int value;
};

class SavedStringConfig {
  public:
    SavedStringConfig(String name, String defaultValue, int offset = 0, int length = 64);
    virtual ~SavedStringConfig() {};
    virtual void setup();
    String getName() const;
    void setNext(SavedStringConfig *next);
    SavedStringConfig *getNext() const;
    virtual void setValue(String value);
    virtual String getValue() const;
    int getLength() const;
    #ifdef ESP32
    static void setPreferences(Preferences *prefs);
    #endif
  private:
    String name;
    SavedStringConfig *next;
  protected:
    String value;
    int offset;
    int length;
    #ifdef ESP32
    static Preferences *prefs;
    #endif
};

class SavedIntConfig: public SavedStringConfig {
  public:
    SavedIntConfig(String name, int defaultValue, int offset = 0, int length = 6);
    void setup();
    void setValue(int value);
    String getValue() const;
    int getIntVal() const;
  protected:
    int value;
};

#endif
