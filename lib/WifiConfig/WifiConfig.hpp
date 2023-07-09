#ifndef WifiConfig_h
#define WifiConfig_h

#include <Arduino.h>

#ifndef WIFI_TIMEOUT
#define WIFI_TIMEOUT 5000
#endif
#ifndef WIFI_RECONNECT_INTERVAL
#define WIFI_RECONNECT_INTERVAL 10000
#endif

struct ConnectionStatus {
  unsigned long from = 0;
  bool connecting = false;
};

class WifiConfig {
  public:
    WifiConfig(String ssid, String password, String name_default, String hostname_default, bool useOTA = true);
    void setDebug(bool value);
    void setup();
    void loop();
    bool isWifiConnected();
  private:
    void checkWifiConnection();
    void setupSensorId();
    void setupWebServer();
  protected:
    bool debug = false;
    ConnectionStatus wifiStatus;
    String ssid;
    String password;
    String name;
    String hostname;
    String sensorId;
    bool useOTA;
};

#endif
