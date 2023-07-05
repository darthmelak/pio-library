// Testbed for the libraries, led on 
#include <Arduino.h>
#include "WifiConfig.hpp"
#include "secrets.h"

WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed");
#ifdef ESP8266
int pin = D7;
#else
int pin = 7;
#endif

void setup() {
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
