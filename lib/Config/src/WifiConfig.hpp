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
      const String& ssid,
      const String& password,
      const String& name_default,
      const String& hostname_default,
      const String& auth_user_default = AUTH_USER_DEFAULT,
      const String& auth_pass_default = AUTH_PASS_DEFAULT,
      bool useOTA = true,
      bool runWebServer = true,
      bool debug = false
    );
    void begin();
    void beginMQTT(
      const String& mqtt_server_default = "test.mosquitto.org",
      int mqtt_port_default = 1883,
      const String& mqtt_user_default = "test",
      const String& mqtt_password_default = "pass",
      const String& mqtt_prefix_default = "homeassistant/",
      MQTTConnectProps props = MQTTConnectProps()
    );
    void loop();
    bool isWifiConnected();
    void registerConfigApi(Configuration& configuration, post_update_cb cb = NULL, bool isPublic = true);
    bool subscribe(const String& topic, bool prefix = true);
    void publish(const String& topic, const String& payload, bool retain = false, bool prefix = true);
    String getPrefixedTopic(const String& topic);
    void getPrefixedTopic(String& prefixedTopic, const String& topic);
    SavedConfiguration getConfig();
    String getSensorId();
    /**
     * generates binary sensor config payload
     * state topic: `binary_sensor/{sensorId}_{suffix}/state`
     */
    String binarySensorConfigPayload(const String& suffix, const String& deviceClass);
    String binarySensorConfigPayload(const String& suffix, String& statTopic, const String& deviceClass);
    /**
     * generates sensor config payload
     * state topic: `sensor/{sensorId}_{suffix}/state`
     */
    String sensorConfigPayload(const String& suffix, const String& deviceClass, const String& unit);
    String sensorConfigPayload(const String& suffix, String& statTopic, const String& deviceClass, const String& unit);
    /**
     * generates switch config payload
     * cmd topic: `switch/{sensorId}_{suffix}/cmd`
     * state topic: `switch/{sensorId}_{suffix}/state`
     */
    String switchConfigPayload(const String& suffix);
    String switchConfigPayload(const String& suffix, String& cmdTopic, String& statTopic);
    /**
     * generates number config payload
     * cmd topic: `number/{sensorId}_{suffix}/cmd/level`
     * state topic: `number/{sensorId}_{suffix}/status/level`
     */
    String numberConfigPayload(const String& suffix, int min = 1, int max = 100, int step = 1);
    String numberConfigPayload(const String& suffix, String& cmdTopic, String& statTopic, int min = 1, int max = 100, int step = 1);
    /**
     * generates light config payload (brightness, on/off)
     * cmd topic: `light/{sensorId}_{suffix}/cmd/state`
     * state topic: `light/{sensorId}_{suffix}/status/state`
     * brightness cmd topic: `light/{sensorId}_{suffix}/cmd/brightness`
     * brightness state topic: `light/{sensorId}_{suffix}/status/brightness`
     */
    String lightConfigPayload(const String& suffix, int maxlevel = 255);
    String lightConfigPayload(const String& suffix, String& cmdTopic, String& statTopic, String& levelCmdTopic, String& levelStatTopic, int maxlevel = 255);
    /**
     * generates fan config payload
     * cmd topic: `fan/{sensorId}_{suffix}/cmd/state`
     * state topic: `fan/{sensorId}_{suffix}/status/state`
     * speed cmd topic: `fan/{sensorId}_{suffix}/cmd/speed`
     * speed state topic: `fan/{sensorId}_{suffix}/status/speed`
     * oscillate cmd topic: `fan/{sensorId}_{suffix}/cmd/oscillate`
     * oscillate state topic: `fan/{sensorId}_{suffix}/status/oscillate`
     */
    String fanConfigPayload(const String& suffix, bool speed = true, bool oscillate = true, int maxSpeed = 8);
    String fanConfigPayload(const String& suffix, String& cmdTopic, String& statTopic, String& spdCmdTopic, String& spdStatTopic, String& oscCmdTopic, String& oscStatTopic, bool speed = true, bool oscillate = true, int maxSpeed = 8);

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
