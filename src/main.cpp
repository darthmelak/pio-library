// Testbed for the libraries, led on
#include <Arduino.h>
#include "Configuration.hpp"
#include "WifiConfig.hpp"
#include "secrets.h"

Configuration config(true);
WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed");
#ifdef ESP8266
int pin = D7;
#else
int pin = 7;
#endif

void setup() {
  Serial.begin(115200);
  delay(10);

  config
    .add("ssid", WIFI_SSID)
    .add("password", WIFI_PASSWORD)
    .add("name", "Testbed")
    .add("hostname", "testbed")
    .add("counter", 0)
    .setup()
  ;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);

    wifiConfig.setDebug(true);
    wifiConfig.setup();
    digitalWrite(pin, LOW);
}

void loop() {
    wifiConfig.loop();
    delay(5);
}
