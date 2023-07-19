#ifndef WifiConfig_h
#define WifiConfig_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Configurations.hpp"

#ifndef WIFI_TIMEOUT
#define WIFI_TIMEOUT 5000
#endif
#ifndef WIFI_RECONNECT_INTERVAL
#define WIFI_RECONNECT_INTERVAL 10000
#endif

using post_update_cb = void (*)();

struct ConnectionStatus {
  unsigned long from = 0;
  bool connecting = false;
};

class WifiConfig {
  public:
    WifiConfig(
      String ssid,
      String password,
      String name_default,
      String hostname_default,
      bool useOTA = true,
      bool runWebServer = true,
      bool debug = false
    );
    void setup();
    void loop();
    bool isWifiConnected();
  protected:
    void checkWifiConnection();
    void setupSensorId();
    void setupWebServer();
    void registerConfigApi(Configuration& config, post_update_cb cb = NULL);
    void respondJson(const JsonDocument& json, int code = 200);
    ConnectionStatus wifiStatus;
    Configuration config;
    String sensorId;
    bool useOTA;
    bool runWebServer;
    bool debug;
};

#endif
