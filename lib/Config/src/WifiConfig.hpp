#ifndef WifiConfig_h
#define WifiConfig_h

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
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
#ifndef AUTH_USER_DEFAULT
#define AUTH_USER_DEFAULT "admin"
#endif
#ifndef AUTH_PASS_DEFAULT
#define AUTH_PASS_DEFAULT "admin"
#endif

typedef std::function<void(bool)> post_update_cb;
typedef std::function<void()> mqtt_connect_cb;
typedef std::function<void(String, String)> mqtt_cb;

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
      String auth_user_default = AUTH_USER_DEFAULT,
      String auth_pass_default = AUTH_PASS_DEFAULT,
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
    void registerConfigApi(Configuration& configuration, post_update_cb cb = NULL, bool isPublic = true);
    bool subscribe(String topic, bool prefix = true);
    void publish(String topic, String payload, bool retain = false, bool prefix = true);
    String getPrefixedTopic(String topic);
    SavedConfiguration getConfig();
    String getSensorId();
    /**
     * generates binary sensor config payload
     * state topic: `binary_sensor/{sensorId}_{suffix}/state`
     */
    String binarySensorConfigPayload(String suffix, String deviceClass);
    /**
     * generates sensor config payload
     * state topic: `sensor/{sensorId}_{suffix}/state`
     */
    String sensorConfigPayload(String suffix, String deviceClass, String unit);
    /**
     * generates switch config payload
     * cmd topic: `switch/{sensorId}_{suffix}/cmd`
     * state topic: `switch/{sensorId}_{suffix}/state`
     */
    String switchConfigPayload(String suffix);
    /**
     * generates fan config payload
     * cmd topic: `fan/{sensorId}_{suffix}/cmd/state`
     * state topic: `fan/{sensorId}_{suffix}/status/state`
     * speed cmd topic: `fan/{sensorId}_{suffix}/cmd/speed`
     * speed state topic: `fan/{sensorId}_{suffix}/status/speed`
     * oscillate cmd topic: `fan/{sensorId}_{suffix}/cmd/oscillate`
     * oscillate state topic: `fan/{sensorId}_{suffix}/status/oscillate`
     */
    String fanConfigPayload(String suffix, bool speed = true, bool oscillate = true, int maxSpeed = 8);

    #ifdef ESP8266
    ESP8266WebServer* getServer();
    #else
    WebServer* getServer();
    #endif
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
