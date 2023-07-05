#ifndef ConfigTypes_h
#define ConfigTypes_h

#include <Arduino.h>

class StringConfig {
  public:
    StringConfig(String name, String defaultValue);
    virtual ~StringConfig();
    virtual void setup();
    String getName() const;
    void setNext(StringConfig *next);
    StringConfig *getNext() const;
    void setValue(String value);
    String getValue() const;
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
    int getValue() const;
  protected:
    int value;
};

class SavedStringConfig: public StringConfig {
  public:
    SavedStringConfig(String name, String defaultValue, int offset = 0, int length = 64);
    void setup();
    void setValue(String value);
    int getLength() const;
  protected:
    int offset;
    int length;
};

class SavedIntConfig: public SavedStringConfig {
  public:
    SavedIntConfig(String name, int defaultValue, int offset = 0, int length = 6);
    void setValue(int value);
    int getValue() const;
  protected:
    int value;
};

#endif
