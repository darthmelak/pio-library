#include <Arduino.h>

typedef std::function<void(const String&)> serial_cmd_cb;

void handleSerial(bool debug = true, serial_cmd_cb cb = NULL) {
  static String buffer = "";
  if (!debug || !Serial.available()) return;

  char c = Serial.read();
  if (c != '\n' && c != '\r') {
    buffer += c;
  } else {
    if (cb != NULL) cb(buffer);

    buffer = "";
  }
}
