#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "WifiConfig.hpp"

#define C_SSID "ssid"
#define C_PASS "password"
#define C_NAME "name"
#define C_HNAM "hostname"
#define C_MQ_SERV "mqtt_server"
#define C_MQ_PORT "mqtt_port"
#define C_MQ_USER "mqtt_user"
#define C_MQ_PASS "mqtt_password"
#define C_MQ_PREF "mqtt_prefix"

#ifdef ESP8266
ESP8266WebServer server;
#else
WebServer server;
#endif
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
char mqttServer[64];

WifiConfig::WifiConfig(
  String ssid,
  String password,
  String name_default,
  String hostname_default,
  bool useOTA,
  bool runWebServer,
  bool debug
): config("/config", debug),
  useOTA(useOTA),
  runWebServer(runWebServer),
  debug(debug),
  runMQTT(false),
  mqttFrom(0),
  mqttPrefix("")
{
  config
    .add(C_SSID, ssid)
    .add(C_PASS, password)
    .add(C_NAME, name_default)
    .add(C_HNAM, hostname_default);
}

void WifiConfig::setup() {
  if (debug) Serial.println("\n\nWifiConfig init ...");
  config.setup();
  setupSensorId();
  if (runWebServer) setupWebServer();
  if (runMQTT) setupMosquitto();
  WiFi.mode(WIFI_STA);
}

void WifiConfig::setupMQTT(
  String mqtt_server_default,
  int mqtt_port_default,
  String mqtt_user_default,
  String mqtt_password_default,
  String mqtt_prefix_default,
  MQTTConnectProps props
) {
  runMQTT = true;
  config
    .add(C_MQ_SERV, mqtt_server_default)
    .add(C_MQ_PORT, mqtt_port_default)
    .add(C_MQ_USER, mqtt_user_default)
    .add(C_MQ_PASS, mqtt_password_default)
    .add(C_MQ_PREF, mqtt_prefix_default);
  mqttProps = props;
  setup();
}

void WifiConfig::loop() {
  checkWifiConnection();
  if (isWifiConnected() && useOTA) ArduinoOTA.handle();
  if (isWifiConnected() && runWebServer) server.handleClient();
  if (isWifiConnected() && runMQTT) checkMQTTConnection();
}

bool WifiConfig::isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WifiConfig::registerConfigApi(Configuration& configuration, post_update_cb cb) {

  server.on(configuration.getPath(), HTTP_GET, [this, &configuration]() {
    if (debug) Serial.printf("GET %s\n", configuration.getPath().c_str());

    StaticJsonDocument<CONFIG_JSON_SIZE> json;
    configuration.toJson(json);
    respondJson(json);
  });

  server.on(configuration.getPath(), HTTP_POST, [this, &configuration, cb]() {
    if (server.hasArg("plain") == false) return;
    if (debug) Serial.printf("POST %s\n", configuration.getPath().c_str());

    String body = server.arg("plain");
    StaticJsonDocument<CONFIG_JSON_SIZE> json;
    deserializeJson(json, body);
    bool changed = configuration.fromJson(json);

    json.clear();
    configuration.toJson(json);
    respondJson(json);
    if (cb != NULL) cb(changed);
  });
}

bool WifiConfig::subscribe(String topic, bool prefix) {
  if (!runMQTT || !mqtt.connected()) return false;

  String fullTopic = prefix ? getPrefixedTopic(topic) : topic;
  if (debug) Serial.printf("MQTT subscribe: %s\n", fullTopic.c_str());
  return mqtt.subscribe(fullTopic.c_str());
}

void WifiConfig::publish(String topic, String payload, bool retain, bool prefix) {
  if (!runMQTT || !mqtt.connected()) return;

  String fullTopic = prefix ? getPrefixedTopic(topic) : topic;
  if (debug) Serial.printf("MQTT publish: %s: %s\n", fullTopic.c_str(), payload.c_str());
  mqtt.publish(fullTopic.c_str(), payload.c_str(), retain);
}

String WifiConfig::getPrefixedTopic(String topic = "") {
  String prefixed = mqttPrefix + topic;
  prefixed.replace("{sensorId}", sensorId);
  return prefixed;
}

SavedConfiguration WifiConfig::getConfig() {
  return config;
}

String WifiConfig::getSensorId() {
  return sensorId;
}

void WifiConfig::checkWifiConnection() {
  if (WiFi.status() == WL_CONNECTED && !wifiStatus.connecting) return;

  if (!wifiStatus.connecting) {
    if (
      wifiStatus.from > 0 &&
      millis() - wifiStatus.from < WIFI_RECONNECT_INTERVAL
    ) return;

    String ssid = config.get(C_SSID)->getValue();
    String password = config.get(C_PASS)->getValue();
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
  String hostname = config.get(C_HNAM)->getValue().c_str();
  sprintf(tmp, "%s-%02x%02x%02x%02x%02x%02x", hostname.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  sensorId = tmp;
  if (debug) Serial.printf("sensorId set to: %s\n", sensorId.c_str());
  if (useOTA) {
    ArduinoOTA.setHostname(hostname.c_str());
    if (debug) Serial.printf("mDNS set to: %s\n", hostname.c_str());
  }
  if (runMQTT) {
    mqttPrefix = config.get(C_MQ_PREF)->getValue();
  }
}

void WifiConfig::setupWebServer() {
  registerConfigApi(config, [this](bool changed) {
    if (debug) Serial.printf("Wifi-config: %s\n", changed ? "saved" : "not changed");
  });
}

void WifiConfig::setupMosquitto() {
  strcpy(mqttServer, config.get(C_MQ_SERV)->getValue().c_str());
  mqtt.setServer(mqttServer, config.getInt(C_MQ_PORT)->getIntVal());
  mqtt.setBufferSize(MQTT_BUFFER_SIZE);
  mqtt.setCallback([this](char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    String data = String((char*) payload);

    if (debug) Serial.printf("MQTT CB: %s: %s\n", topic, data.c_str());

    if (mqttProps.cb != NULL) mqttProps.cb(topic, data);
  });
}

void WifiConfig::checkMQTTConnection() {
  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }

  if (mqttFrom > 0 && millis() - mqttFrom < MQTT_RECONNECT_INTERVAL) return;

  if (debug) Serial.printf("Connecting to MQTT on %s:%d\n", mqttServer, config.getInt(C_MQ_PORT)->getIntVal());
  bool success = mqtt.connect(sensorId.c_str(), config.get(C_MQ_USER)->getValue().c_str(), config.get(C_MQ_PASS)->getValue().c_str());
  if (debug) {
    if (success) Serial.println("MQTT connection successful");
    else Serial.printf("MQTT connection failed: %d\n", mqtt.state());
  }
  if (!success) mqttFrom = millis();
  else if (mqttProps.connect_cb != NULL) mqttProps.connect_cb();
}

void WifiConfig::respondJson(const JsonDocument& json, int code) {
  String response;
  serializeJson(json, response);
  server.send(code, "application/json", response);
}
