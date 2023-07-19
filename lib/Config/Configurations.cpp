#include "Configurations.hpp"

Configuration::Configuration(String path, bool debug): path(path), debug(debug), first(NULL), last(NULL) {}

String Configuration::getPath() const {
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
    if (strcmp(item->getType(), CONF_T_INT) == 0) {
      obj[item->getName()] = ((IntConfig *) item)->getIntVal();
    } else {
      obj[item->getName()] = item->getValue();
    }
    item = item->getNext();
  }
}

void Configuration::fromJson(JsonDocument& json) {
  JsonObject obj = json.as<JsonObject>();
  StringConfig *item = first;
  while (item != NULL) {
    if (obj.containsKey(item->getName())) {
      if (strcmp(item->getType(), CONF_T_INT) == 0) {
        ((IntConfig *) item)->setValue(obj[item->getName()].as<int>());
      } else {
        item->setValue(obj[item->getName()].as<String>());
      }
    }
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

int SavedConfiguration::getSize() const {
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

SavedStringConfig *SavedConfiguration::get(String name) const {
  SavedStringConfig *current = first;
  while (current != NULL) {
    if (current->getName() == name) {
      return current;
    }
    current = current->getNext();
  }
  return NULL;
}

SavedIntConfig *SavedConfiguration::getInt(String name) const {
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

SavedStringConfig *SavedConfiguration::getFirst() const {
  return first;
}

void SavedConfiguration::toJson(JsonDocument& json) const {
  JsonObject obj = json.to<JsonObject>();
  SavedStringConfig *item = first;
  while (item != NULL) {
    if (strcmp(item->getType(), CONF_T_INT) == 0) {
      obj[item->getName()] = ((SavedIntConfig *) item)->getIntVal();
    } else {
      obj[item->getName()] = item->getValue();
    }
    item = item->getNext();
  }
}
