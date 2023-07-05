#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include <ArduinoOTA.h>
#include "WifiConfig.hpp"

WifiConfig::WifiConfig(
  String ssid,
  String password,
  String name_default,
  String hostname_default,
  bool reconnect,
  bool useOTA
): ssid(ssid),
  password(password),
  name(name_default),
  hostname(hostname_default),
  reconnect(reconnect),
  useOTA(useOTA)
{}

void WifiConfig::setup() {
  if (debug) {
    Serial.begin(115200);
    delay(10);
    Serial.println("\n\nInit ...");
  }

  setupSensorId();

  WiFi.mode(WIFI_STA);
  connectWifi();
}

void WifiConfig::loop() {
  if (useOTA) ArduinoOTA.handle();
  if (!isWifiConnected() && reconnect && millis() - lastConnectAttempt > WIFI_RECONNECT_INTERVAL) {
    connectWifi();
  }
}

bool WifiConfig::isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WifiConfig::setDebug(bool debug) {
  this->debug = debug;
}

void WifiConfig::connectWifi() {
  if (debug) Serial.printf("Connecting to WiFi: %s ", ssid.c_str());
  
  long connectStart = millis();
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED && millis() - connectStart < WIFI_TIMEOUT) {
    delay(100);
    if (debug) Serial.print(".");
  }

  lastConnectAttempt = millis();
  if (WiFi.status() == WL_CONNECTED) {
    if (debug) Serial.printf("\nConnected, IP address: %s\n", WiFi.localIP().toString().c_str());

    if (useOTA) {
      ArduinoOTA.begin();
      if (debug) Serial.println("OTA started");
    }
  } else {
    if (debug) Serial.println("\nConnection failed.");
  }
}

void WifiConfig::setupSensorId() {
  char tmp[64]; // setupid from hostname-mac
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(tmp, "%s-%02x%02x%02x%02x%02x%02x", hostname.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  sensorId = tmp;
  if (debug) Serial.println("sensorId set to:" + sensorId);
  if (useOTA) {
    ArduinoOTA.setHostname(hostname.c_str());
    if (debug) Serial.println("mDNS set to: " + hostname);
  }
}
