#include "Configurations.hpp"

Configuration::Configuration(bool debug): debug(debug), first(NULL), last(NULL) {}

Configuration& Configuration::add(String name, String defaultValue) {
  StringConfig *config = new StringConfig(name, defaultValue);
  chain(config);

  return *this;
}

Configuration& Configuration::add(String name, int defaultValue) {
  IntConfig *config = new IntConfig(name, defaultValue);
  chain(config);

  return *this;
}

StringConfig *Configuration::get(String name) {
  StringConfig *current = first;
  while (current != NULL) {
    if (current->getName() == name) {
      return current;
    }
    current = current->getNext();
  }
  return NULL;
}

void Configuration::chain(StringConfig *config) {
  if (first == NULL) {
    first = config;
    last = config;
  } else {
    last->setNext(config);
    last = config;
  }
}

SavedConfiguration::SavedConfiguration(bool debug): Configuration(debug), size(0) {}

void SavedConfiguration::setup() {
  #ifdef ESP8266
    EEPROM.begin(size);
  #else
    preferences.begin(PREFS_NAMESPACE);
  #endif
  SavedStringConfig *current = first;
  while (current != NULL) {
    current->setup();
    if (debug) Serial.println(current->getName() + ": " + current->getValue());
    current = current->getNext();
  }
}

int SavedConfiguration::getSize() {
  return size;
}

SavedConfiguration& SavedConfiguration::add(String name, String defaultValue) {
  SavedStringConfig *config = new SavedStringConfig(name, defaultValue, size);
  size += config->getLength();
  chain(config);

  return *this;
}

SavedConfiguration& SavedConfiguration::add(String name, int defaultValue) {
  SavedIntConfig *config = new SavedIntConfig(name, defaultValue, size);
  size += config->getLength();
  chain(config);

  return *this;
}

SavedStringConfig *SavedConfiguration::get(String name) {
  SavedStringConfig *current = first;
  while (current != NULL) {
    if (current->getName() == name) {
      return current;
    }
    current = current->getNext();
  }
  return NULL;
}

void SavedConfiguration::chain(SavedStringConfig *config) {
  if (first == NULL) {
    first = config;
    last = config;
  } else {
    last->setNext(config);
    last = config;
  }
}
