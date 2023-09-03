#include "ConfigTypes.hpp"

/* --== StringConfig ==-- */
StringConfig::StringConfig(const String& name, const String& defaultValue, str_update_cb cb): name(name), next(nullptr), value(defaultValue), cb(cb) {}

String StringConfig::getName() const {
  return name;
}

void StringConfig::setNext(StringConfig *next) {
  this->next = next;
}

StringConfig *StringConfig::getNext() const {
  return next;
}

bool StringConfig::setValue(const String& value) {
  if (this->value == value) return false;

  this->value = value;
  if (cb != nullptr) cb(value);
  return true;
}

String StringConfig::getValue() const {
  return value;
}

const char *StringConfig::getType() const {
  return CONF_T_STR;
}

void StringConfig::setCb(str_update_cb cb) {
  this->cb = cb;
}

/* --== IntConfig ==-- */
IntConfig::IntConfig(const String& name, int defaultValue, int_update_cb cb): StringConfig(name, String(defaultValue)), value(defaultValue), cb(cb) {}

bool IntConfig::setValue(int value) {
  if (this->value == value) return false;

  this->value = value;
  if (cb != nullptr) cb(value);
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

void IntConfig::setCb(int_update_cb cb) {
  this->cb = cb;
}

/* --== SavedStringConfig ==-- */
SavedStringConfig::SavedStringConfig(
  const String& name,
  const String& defaultValue,
  str_update_cb cb,
  int offset,
  int length
): name(name),
  next(nullptr),
  value(defaultValue),
  offset(offset),
  length(length),
  cb(cb) {}

void SavedStringConfig::setup() {
  char buffer[length];
  char charbuff;

  for (int i = 0; i < length; i++) {
    charbuff = char(EEPROM.read(offset + i));
    buffer[i] = isPrintable(charbuff) ? charbuff : 0;
  }
  String res = buffer;
  value = (res.length() > 0) ? res : value;
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

bool SavedStringConfig::setValue(const String& value) {
  if (this->value == value) return false;

  this->value = value;
  save(value);

  if (cb != nullptr) cb(value);
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

void SavedStringConfig::setCb(str_update_cb cb) {
  this->cb = cb;
}

void SavedStringConfig::save(const String& value) {
  for (int i = 0; i < length; i++) {
    if ((unsigned)i < value.length()) {
      EEPROM.write(offset + i, value[i]);
    } else {
      EEPROM.write(offset + i, 0);
    }
  }
  EEPROM.commit();
}

/* --== SavedIntConfig ==-- */
SavedIntConfig::SavedIntConfig(
  const String& name,
  int defaultValue,
  int_update_cb cb,
  int offset,
  int length
): SavedStringConfig(name, String(defaultValue), nullptr, offset, length), value(defaultValue), cb(cb) {}

void SavedIntConfig::setup() {
  char buffer[length];
  char charbuff;

  for (int i = 0; i < length; i++) {
    charbuff = char(EEPROM.read(offset + i));
    buffer[i] = isPrintable(charbuff) ? charbuff : 0;
  }
  String res = buffer;
  value = (res.length() > 0) ? res.toInt() : value;
}

bool SavedIntConfig::setValue(int value) {
  if (this->value == value) return false;

  this->value = value;
  save(String(value));

  if (cb != nullptr) cb(value);
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

void SavedIntConfig::setCb(int_update_cb cb) {
  this->cb = cb;
}

SavedJsonConfig::SavedJsonConfig(
  const String& name,
  const String& defaultValue,
  json_update_cb cb,
  int offset,
  int length
): SavedStringConfig(name, defaultValue, nullptr, offset, length), value(length + 16), cb(cb) {
  deserializeJson(value, defaultValue);
}

void SavedJsonConfig::setup() {
  char buffer[length];
  char charbuff;

  for (int i = 0; i < length; i++) {
    charbuff = char(EEPROM.read(offset + i));
    buffer[i] = isPrintable(charbuff) ? charbuff : 0;
  }

  if (strlen(buffer) > 0) {
    deserializeJson(value, (const char*) buffer, length);
  }
}

bool SavedJsonConfig::setValue(const JsonDocument& value) {
  String strVal;
  serializeJson(value, strVal);
  String innerVal;
  serializeJson(this->value, innerVal);
  if (innerVal == strVal) return false;

  this->value = value;
  save(strVal);

  if (cb != nullptr) cb(value);
  return true;
}

String SavedJsonConfig::getValue() const {
  String strVal;
  serializeJson(value, strVal);
  return strVal;
}

JsonVariantConst SavedJsonConfig::getJsonVal() const {
  return value.as<JsonVariantConst>();
}

const char *SavedJsonConfig::getType() const {
  return CONF_T_JSON;
}

void SavedJsonConfig::setCb(json_update_cb cb) {
  this->cb = cb;
}
