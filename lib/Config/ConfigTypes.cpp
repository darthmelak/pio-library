#include <EEPROM.h>
#include "ConfigTypes.hpp"

/* --== StringConfig ==-- */
StringConfig::StringConfig(String name, String defaultValue): name(name), value(defaultValue) {}

StringConfig::~StringConfig() {}

void StringConfig::setup() {}

String StringConfig::getName() const {
  return name;
}

void StringConfig::setNext(StringConfig *next) {
  this->next = next;
}

StringConfig *StringConfig::getNext() const {
  return next;
}

void StringConfig::setValue(String value) {
  this->value = value;
}

String StringConfig::getValue() const {
  return value;
}

/* --== IntConfig ==-- */
IntConfig::IntConfig(String name, int defaultValue): StringConfig(name, String(defaultValue)), value(defaultValue) {}

void IntConfig::setValue(int value) {
  this->value = value;
}

int IntConfig::getValue() const {
  return value;
}

/* --== SavedStringConfig ==-- */
SavedStringConfig::SavedStringConfig(
  String name,
  String defaultValue,
  int offset,
  int length
): StringConfig(name, defaultValue),
  offset(offset),
  length(length)
{}

void SavedStringConfig::setup() {
  char buffer[64];
  char charbuff;

  for (int i = 0; i < length; i++) {
    charbuff = char(EEPROM.read(offset + i));
    buffer[i] = isPrintable(charbuff) ? charbuff : 0;
  }
  String res = buffer;
  value = (res.length() > 0) ? res : value;
}

void SavedStringConfig::setValue(String value) {
  this->value = value;
  for (int i = 0; i < length; i++) {
    if (i < value.length()) {
      EEPROM.write(offset + i, value[i]);
    } else {
      EEPROM.write(offset + i, 0);
    }
  }
  EEPROM.commit();
}

int SavedStringConfig::getLength() const {
  return length;
}

/* --== SavedIntConfig ==-- */
SavedIntConfig::SavedIntConfig(
  String name,
  int defaultValue,
  int offset,
  int length
): SavedStringConfig(name, String(defaultValue), offset, length),
  value(defaultValue)
{}

void SavedIntConfig::setValue(int value) {
  SavedStringConfig::setValue(String(value));
  this->value = value;
}

int SavedIntConfig::getValue() const {
  return value;
}
