#include "Configuration.hpp"

Configuration::Configuration(bool debug): debug(debug), size(0), first(NULL), last(NULL) {}

void Configuration::setup() {
  StringConfig *current = first;
  while (current != NULL) {
    current->setup();
    if (debug) Serial.println(current->getName() + ": " + current->getValue());
    current = current->getNext();
  }
}

int Configuration::getSize() {
  return size;
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

Configuration& Configuration::addSaved(String name, String defaultValue) {
  SavedStringConfig *config = new SavedStringConfig(name, defaultValue, size);
  size += config->getLength();
  chain(config);

  return *this;
}

Configuration& Configuration::addSaved(String name, int defaultValue) {
  SavedIntConfig *config = new SavedIntConfig(name, defaultValue, size);
  size += config->getLength();
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
