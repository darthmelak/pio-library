// Testbed for the libraries, led on
#include <Arduino.h>
#include <arduino-timer.h>
#include "Configurations.hpp"
#include "WifiConfig.hpp"
#include "secrets.h"

bool debug = true;
bool echoCount = false;

Configuration config("/test", debug);
WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed", true, true, debug);
#ifdef ESP8266
int pin = D7;
int pin2 = D6;
#else
int pin = 7;
int pin2 = 6;
#endif
Timer<2> timer;

void handleSerial();

void setup() {
  if (debug) {
    Serial.begin(115200);
    delay(10);
  }

  config
    .add("ssid", WIFI_SSID)
    .add("password", WIFI_PASSWORD)
    .add("name", "Testbed")
    .add("hostname", "testbed", [](String value) {
      Serial.printf("Hostname changed to: %s\n", value.c_str());
    })
    .add("counter", 0)
    .add("light", 0, [](int value) {
      digitalWrite(pin, value);
    })
  ;

  wifiConfig.registerConfigApi(config, [](bool changed) {
    if (changed) {
      Serial.println("Config updated");
    } else {
      Serial.println("Config unchanged");
    }
  });

  pinMode(pin, OUTPUT);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin2, LOW);
  timer.every(1000, [](void*) -> bool {
    digitalWrite(pin2, !digitalRead(pin2));
    IntConfig *counter = config.getInt("counter");
    counter->setValue(counter->getIntVal() + 1);
    if (debug && echoCount) Serial.println(counter->getValue());
    return true;
  });
  // config.setup();

  wifiConfig.setup();
  digitalWrite(pin, LOW);
}

void loop() {
  handleSerial();
  wifiConfig.loop();
  digitalWrite(pin, wifiConfig.isWifiConnected() ? LOW : HIGH);
  timer.tick();
  delay(1);
}

void handleSerial() {
  static String buffer = "";
  if (!debug || !Serial.available()) return;

  char c = Serial.read();
  if (c != '\n' && c != '\r') {
    buffer += c;
  } else {
    if (buffer == "echo") {
      echoCount = !echoCount;
      Serial.println("Echo counter: " + String(echoCount));
    }
    if (buffer == "test") {
      StringConfig *item = config.getFirst();
      while (item != NULL) {
        Serial.println(item->getName() + ": " + item->getValue());
        item = item->getNext();
      }
    }
    if (buffer == "json") {
      DynamicJsonDocument json(256);
      config.toJson(json);
      String response;
      serializeJson(json, response);
      Serial.println(response);
    }

    buffer = "";
  }
}
