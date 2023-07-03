// Testbed for the libraries, led on 
#include <Arduino.h>
#include "WifiConfig.hpp"
#include "secrets.h"

WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed");

void setup() {
    pinMode(D7, OUTPUT);
    digitalWrite(D7, HIGH);

    wifiConfig.setDebug(true);
    wifiConfig.setup();
    digitalWrite(D7, LOW);
}

void loop() {
    wifiConfig.loop();
    delay(5);
}
