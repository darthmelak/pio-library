#include "Configurations.hpp"

Configuration::Configuration(String path, bool debug): path(path), debug(debug), first(NULL), last(NULL) {}

String Configuration::getPath() {
  return path;
}

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

StringConfig *Configuration::get(String name) const {
  StringConfig *current = first;
  while (current != NULL) {
    if (current->getName() == name) {
      return current;
    }
    current = current->getNext();
  }
  return NULL;
}

IntConfig *Configuration::getInt(String name) const {
  StringConfig *current = first;
  while (current != NULL) {
    if (current->getName() == name) {
      return (IntConfig *) current;
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

StringConfig *Configuration::getFirst() const {
  return first;
}

void Configuration::toJson(JsonDocument& json) const {
  JsonObject obj = json.to<JsonObject>();
  StringConfig *item = first;
  while (item != NULL) {
    obj[item->getName()] = item->getValue();
    item = item->getNext();
  }
}

SavedConfiguration::SavedConfiguration(String path, bool debug): Configuration(path, debug), size(0) {}

void SavedConfiguration::setup() {
  #ifdef ESP8266
    EEPROM.begin(size+32);
  #else
    preferences.begin(PREFS_NAMESPACE);
  #endif
  SavedStringConfig *current = first;
  while (current != NULL) {
    current->setup();
    if (debug) Serial.printf("%s: %s\n", current->getName().c_str(), current->getValue().c_str());
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

SavedIntConfig *SavedConfiguration::getInt(String name) {
  SavedStringConfig *current = first;
  while (current != NULL) {
    if (current->getName() == name) {
      return (SavedIntConfig *) current;
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

SavedStringConfig *SavedConfiguration::getFirst() {
  return first;
}
