#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <ArduinoOTA.h>
#include "WifiConfig.hpp"

#ifdef ESP8266
ESP8266WebServer server;
#else
WebServer server;
#endif


WifiConfig::WifiConfig(
  String ssid,
  String password,
  String name_default,
  String hostname_default,
  bool useOTA,
  bool runWebServer
): ssid(ssid),
  password(password),
  name(name_default),
  hostname(hostname_default),
  useOTA(useOTA),
  runWebServer(runWebServer)
{}

void WifiConfig::setup() {
  if (debug) Serial.println("\n\nInit ...");
  setupSensorId();
  if (runWebServer) setupWebServer();
  WiFi.mode(WIFI_STA);
}

void WifiConfig::loop() {
  checkWifiConnection();
  if (isWifiConnected() && useOTA) ArduinoOTA.handle();
  if (isWifiConnected() && runWebServer) server.handleClient();
}

bool WifiConfig::isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WifiConfig::setDebug(bool debug) {
  this->debug = debug;
}

void WifiConfig::checkWifiConnection() {
  if (WiFi.status() == WL_CONNECTED && !wifiStatus.connecting) return;

  if (!wifiStatus.connecting) {
    if (
      wifiStatus.from > 0 &&
      millis() - wifiStatus.from < WIFI_RECONNECT_INTERVAL
    ) return;

    if (debug) Serial.printf("Connecting to WiFi: %s ", ssid.c_str());

    wifiStatus.from = millis();
    wifiStatus.connecting = true;
    WiFi.begin(ssid, password);
  } else {
    unsigned long elapsed = millis() - wifiStatus.from;
    if (WiFi.status() != WL_CONNECTED && elapsed < WIFI_TIMEOUT) {
      if (debug && elapsed % 100 < 4) Serial.print(".");
    } else {
      wifiStatus.connecting = false;
      wifiStatus.from = millis();

      if (WiFi.status() == WL_CONNECTED) {
        if (debug) Serial.printf("\nConnected, IP address: %s\n", WiFi.localIP().toString().c_str());

        if (useOTA) {
          ArduinoOTA.begin();
          if (debug) Serial.println("OTA started");
        }

        if (runWebServer) {
          server.begin();
          if (debug) Serial.println("WebServer started");
        }
      } else {
        if (debug) Serial.println("\nConnection failed.");
      }
    }
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

void WifiConfig::setupWebServer()
{
  server.on("/", HTTP_GET, [this]() {
    Serial.println("Request: GET /");
    StaticJsonDocument<256> json;
    json["name"] = name;
    json["hostname"] = hostname;
    respondJson(json);
  });
}

void WifiConfig::respondJson(const JsonDocument& json, int code) {
  String response;
  serializeJson(json, response);
  server.send(code, "application/json", response);
}
