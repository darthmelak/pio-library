#include "ConfigTypes.hpp"

/* --== StringConfig ==-- */
StringConfig::StringConfig(String name, String defaultValue, str_update_cb cb): name(name), next(NULL), value(defaultValue), cb(cb) {}

String StringConfig::getName() const {
  return name;
}

void StringConfig::setNext(StringConfig *next) {
  this->next = next;
}

StringConfig *StringConfig::getNext() const {
  return next;
}

bool StringConfig::setValue(String value) {
  if (this->value == value) return false;

  this->value = value;
  if (cb != NULL) cb(value);
  return true;
}

String StringConfig::getValue() const {
  return value;
}

const char *StringConfig::getType() const {
  return CONF_T_STR;
}

/* --== IntConfig ==-- */
IntConfig::IntConfig(String name, int defaultValue, int_update_cb cb): StringConfig(name, String(defaultValue)), value(defaultValue), cb(cb) {}

bool IntConfig::setValue(int value) {
  if (this->value == value) return false;

  this->value = value;
  if (cb != NULL) cb(value);
  return true;
}

String IntConfig::getValue() const {
  return String(value);
}

int IntConfig::getIntVal() const {
  return value;
}

const char *IntConfig::getType() const {
  return CONF_T_INT;
}

/* --== SavedStringConfig ==-- */
SavedStringConfig::SavedStringConfig(
  String name,
  String defaultValue,
  str_update_cb cb,
  int offset,
  int length
): name(name),
  next(NULL),
  value(defaultValue),
  offset(offset),
  length(length),
  cb(cb) {}

void SavedStringConfig::setup() {
  #ifdef ESP8266
  char buffer[64];
  char charbuff;

  for (int i = 0; i < length; i++) {
    charbuff = char(EEPROM.read(offset + i));
    buffer[i] = isPrintable(charbuff) ? charbuff : 0;
  }
  String res = buffer;
  value = (res.length() > 0) ? res : value;
  #else
  value = prefs->getString(getName().c_str(), value);
  #endif
}

String SavedStringConfig::getName() const {
  return name;
}

void SavedStringConfig::setNext(SavedStringConfig *next) {
  this->next = next;
}

SavedStringConfig *SavedStringConfig::getNext() const {
  return next;
}

bool SavedStringConfig::setValue(String value) {
  if (this->value == value) return false;

  this->value = value;
  #ifdef ESP8266
  for (int i = 0; i < length; i++) {
    if ((unsigned)i < value.length()) {
      EEPROM.write(offset + i, value[i]);
    } else {
      EEPROM.write(offset + i, 0);
    }
  }
  EEPROM.commit();
  #else
  prefs->putString(getName().c_str(), value);
  #endif
  if (cb != NULL) cb(value);
  return true;
}

String SavedStringConfig::getValue() const {
  return value;
}

int SavedStringConfig::getLength() const {
  return length;
}

const char *SavedStringConfig::getType() const {
  return CONF_T_STR;
}

#ifdef ESP32
void SavedStringConfig::setPreferences(Preferences *prefs) {
  SavedStringConfig::prefs = prefs;
}
#endif

/* --== SavedIntConfig ==-- */
SavedIntConfig::SavedIntConfig(
  String name,
  int defaultValue,
  int_update_cb cb,
  int offset,
  int length
): SavedStringConfig(name, String(defaultValue), NULL, offset, length), value(defaultValue), cb(cb) {}

void SavedIntConfig::setup() {
  #ifdef ESP8266
  char buffer[64];
  char charbuff;

  for (int i = 0; i < length; i++) {
    charbuff = char(EEPROM.read(offset + i));
    buffer[i] = isPrintable(charbuff) ? charbuff : 0;
  }
  String res = buffer;
  value = (res.length() > 0) ? res.toInt() : value;
  #else
  value = prefs->getInt(getName().c_str(), value);
  #endif
}

bool SavedIntConfig::setValue(int value) {
  if (this->value == value) return false;

  #ifdef ESP8266
  SavedStringConfig::setValue(String(value));
  #else
  prefs->putInt(getName().c_str(), value) > 0;
  #endif
  this->value = value;
  if (cb != NULL) cb(value);
  return true;
}

String SavedIntConfig::getValue() const {
  return String(value);
}

int SavedIntConfig::getIntVal() const {
  return value;
}

const char *SavedIntConfig::getType() const {
  return CONF_T_INT;
}
