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
#ifndef CONFIG_JSON_SIZE
#define CONFIG_JSON_SIZE 512
#endif
#ifndef MQTT_BUFFER_SIZE
#define MQTT_BUFFER_SIZE 768
#endif
#ifndef MQTT_RECONNECT_INTERVAL
#define MQTT_RECONNECT_INTERVAL 10000
#endif

typedef std::function<void(bool)> post_update_cb;
typedef std::function<void()> mqtt_connect_cb;
typedef std::function<void(String, JsonDocument&)> mqtt_cb;

struct ConnectionStatus {
  unsigned long from = 0;
  bool connecting = false;
};

struct MQTTConnectProps {
  MQTTConnectProps(
    mqtt_connect_cb connect_cb = NULL,
    mqtt_cb cb = NULL,
    String willTopic = "",
    String willMessage = ""
  ): connect_cb(connect_cb), cb(cb), willTopic(willTopic), willMessage(willMessage) {}

  mqtt_connect_cb connect_cb = NULL;
  mqtt_cb cb = NULL;
  String willTopic = "";
  String willMessage = "";
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
    void setupMQTT(
      String mqtt_server_default = "test.mosquitto.org",
      int mqtt_port_default = 1883,
      String mqtt_user_default = "test",
      String mqtt_password_default = "pass",
      String mqtt_prefix_default = "/homeassistant/test",
      MQTTConnectProps props = MQTTConnectProps()
    );
    void loop();
    bool isWifiConnected();
    void registerConfigApi(Configuration& configuration, post_update_cb cb = NULL);
    bool subscribe(String topic, bool prefix = true);
    void publish(String topic, String payload, bool retain = false, bool prefix = true);
    String getPrefixedTopic(String topic);
    SavedConfiguration getConfig();
  protected:
    void checkWifiConnection();
    void setupSensorId();
    void setupWebServer();
    void setupMosquitto();
    void checkMQTTConnection();
    void respondJson(const JsonDocument& json, int code = 200);
    ConnectionStatus wifiStatus;
    SavedConfiguration config;
    String sensorId;
    bool useOTA;
    bool runWebServer;
    bool debug;
    bool runMQTT;
    unsigned long mqttFrom;
    String mqttPrefix;
    MQTTConnectProps mqttProps;
};

#endif
