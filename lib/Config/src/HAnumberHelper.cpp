#include "HAnumberHelper.hpp"

HAnumberHelper::HAnumberHelper(WifiConfig& wifiConfig, const char* suffix, int_update_cb onUpdate, int default_value, int min, int max, int step, bool debug): wifiConfig(wifiConfig), suffix(suffix), onUpdate(onUpdate), default_value(default_value), min(min), max(max), step(step), debug(debug) {
  char tmp[64];
  snprintf(tmp, 64, "/%s", suffix);
  config.setPath(tmp);
}

void HAnumberHelper::begin() {
  wifiConfig.getPrefixedTopic(cmdTopic, "number/{sensorId}_{suffix}/cmd");
  cmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(stateTopic, "number/{sensorId}_{suffix}/state");
  stateTopic.replace("{suffix}", suffix);

  config.add("nr", default_value, [this](int value) {
    if (onUpdate) {
      onUpdate(value);
    }
    wifiConfig.publish(stateTopic, String(value), true, false);
  });

  if (onUpdate) {
    onUpdate(default_value);
  }
}

Configuration& HAnumberHelper::getConfig() {
  return config;
}

void HAnumberHelper::onMqttConnect() {
  String configTopic;
  wifiConfig.getPrefixedTopic(configTopic, "number/{sensorId}_{suffix}/config");
  configTopic.replace("{suffix}", suffix);

  wifiConfig.publish(configTopic, wifiConfig.numberConfigPayload(suffix, cmdTopic, stateTopic, min, max, step), true, false);
  wifiConfig.subscribe(cmdTopic, false);
  wifiConfig.publish(stateTopic, String(default_value), true, false);
}

void HAnumberHelper::onMqttMessage(const String& topic, const String& payload) {
  if (topic == cmdTopic) {
    config.getInt("nr")->setValue(payload.toInt());
  }
}
