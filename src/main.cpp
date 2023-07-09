// Testbed for the libraries, led on
#include <Arduino.h>
#include <arduino-timer.h>
#include "Configurations.hpp"
#include "WifiConfig.hpp"
#include "secrets.h"

bool debug = true;

Configuration config(debug);
WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed");
#ifdef ESP8266
int pin = D7;
int pin2 = D6;
#else
int pin = 7;
int pin2 = 6;
#endif
Timer<2> timer;

void setup() {
  if (debug) {
    Serial.begin(115200);
    delay(10);
  }

  config
    .add("ssid", WIFI_SSID)
    .add("password", WIFI_PASSWORD)
    .add("name", "Testbed")
    .add("hostname", "testbed")
    .add("counter", 0)
  ;

    pinMode(pin, OUTPUT);
    pinMode(pin2, OUTPUT);
    digitalWrite(pin, HIGH);
    digitalWrite(pin2, LOW);
    timer.every(500, [](void*) -> bool {
        digitalWrite(pin2, !digitalRead(pin2));
        IntConfig *counter = (IntConfig *) config.get("counter");
        counter->setValue(counter->getValue() + 1);
        if (debug) Serial.println(config.get("counter")->getValue());
        return true;
    });

    wifiConfig.setDebug(true);
    wifiConfig.setup();
    digitalWrite(pin, LOW);
}

void loop() {
    wifiConfig.loop();
    timer.tick();
    delay(1);
}
