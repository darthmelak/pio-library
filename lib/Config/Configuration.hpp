#ifndef Configuration_h
#define Configuration_h

#include "ConfigTypes.hpp"

class Configuration {
  public:
    Configuration(bool debug = false);
    void setup();
    int getSize();
    Configuration& add(String name, String defaultValue);
    Configuration& add(String name, int defaultValue);
    Configuration& addSaved(String name, String defaultValue);
    Configuration& addSaved(String name, int defaultValue);
    StringConfig *get(String name);
  private:
    bool debug;
    int size;
    StringConfig *first;
    StringConfig *last;
    void chain(StringConfig *config);
};

#endif
