#include <Arduino.h>
#include "HAswitchHelper.hpp"

HAswitchHelper::HAswitchHelper(WifiConfig& wifiConfig, const char* suffix, int pin, bool invertState, bool debug): wifiConfig(wifiConfig), suffix(suffix), pin(pin), invertState(invertState), config(debug), debug(debug) {
  char tmp[64];
  snprintf(tmp, 64, "/%s", suffix);
  config.setPath(tmp);
}

void HAswitchHelper::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, invertState ? HIGH : LOW);

  wifiConfig.getPrefixedTopic(cmdTopic, "switch/{sensorId}_{suffix}/cmd/state");
  cmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(stateTopic, "switch/{sensorId}_{suffix}/status/state");
  stateTopic.replace("{suffix}", suffix);

  config.add("state", LOW, [this](int value) {
    digitalWrite(pin, invertState ? !value : value);
    String state = value ? "ON" : "OFF";
    wifiConfig.publish(stateTopic, state, true, false);
  });
}

Configuration& HAswitchHelper::getConfig() {
  return config;
}

void HAswitchHelper::onMqttConnect() {
  String configTopic;
  wifiConfig.getPrefixedTopic(configTopic, "switch/{sensorId}_{suffix}/config");
  configTopic.replace("{suffix}", suffix);

  wifiConfig.publish(configTopic, wifiConfig.switchConfigPayload(suffix, cmdTopic, stateTopic), true, false);
  wifiConfig.subscribe(cmdTopic, false);
  wifiConfig.publish(stateTopic, "OFF", true, false);
}

void HAswitchHelper::onMqttMessage(const String& topic, const String& payload) {
  if (topic == cmdTopic) {
    config.getInt("state")->setValue(payload == "ON" ? HIGH : LOW);
  }
}
