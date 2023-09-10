#include <Arduino.h>
#include "HAlightHelper.hpp"

HAlightHelper::HAlightHelper(WifiConfig& wifiConfig, const char* suffix, int pin, int maxLevel, int minOffset, int maxOffset, bool invertState, bool debug): HAswitchHelper(wifiConfig, suffix, pin, invertState, debug), maxLevel(maxLevel), minOffset(minOffset), maxOffset(maxOffset) {}

void HAlightHelper::begin() {
  pinMode(pin, OUTPUT);
  #ifdef ESP32
  int ledStatus = ledcSetup(pwmChannel, 25000, 8);
  if (debug) Serial.printf("ledcSetup channel: %d, status: %d\n", pwmChannel, ledStatus);
  ledcAttachPin(pin, pwmChannel);
  pwmChannel++;
  #endif
  digitalWrite(pin, invertState ? HIGH : LOW);

  wifiConfig.getPrefixedTopic(cmdTopic, "light/{sensorId}_{suffix}/cmd/state");
  cmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(stateTopic, "light/{sensorId}_{suffix}/status/state");
  stateTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(levelCmdTopic, "light/{sensorId}_{suffix}/cmd/level");
  levelCmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(levelStateTopic, "light/{sensorId}_{suffix}/status/level");
  levelStateTopic.replace("{suffix}", suffix);

  config
    .add("state", LOW, [this](int value) {
      if (value == LOW) {
        level = invertState ? 255 : 0;
      } else {
        int lvl = config.getInt("level")->getIntVal();
        level = invertState ?
          mapRng(lvl, 0, maxLevel, 255 - maxOffset, minOffset) :
          mapRng(lvl, 0, maxLevel, minOffset, 255 - maxOffset);
      }
      analogWrite(pin, level);
      String state = value ? "ON" : "OFF";
      wifiConfig.publish(stateTopic, state, true, false);
    })
    .add("level", 1, [this](int value) {
      if (
        invertState ?
        level < 255 :
        level > 0
      ) {
        level = invertState ?
          mapRng(value, 0, maxLevel, 255 - maxOffset, minOffset) :
          mapRng(value, 0, maxLevel, minOffset, 255 - maxOffset);
        analogWrite(pin, level);
      }
      wifiConfig.publish(levelStateTopic, String(value), true, false);
    })
  ;
}

void HAlightHelper::onMqttConnect() {
  String configTopic;
  wifiConfig.getPrefixedTopic(configTopic, "light/{sensorId}_{suffix}/config");
  configTopic.replace("{suffix}", suffix);

  wifiConfig.publish(configTopic, wifiConfig.lightConfigPayload(suffix, cmdTopic, stateTopic, levelCmdTopic, levelStateTopic, maxLevel), true, false);
  wifiConfig.subscribe(cmdTopic, false);
  wifiConfig.publish(stateTopic, "OFF", true, false);
  wifiConfig.subscribe(levelCmdTopic, false);
  wifiConfig.publish(levelStateTopic, "1", true, false);
}

void HAlightHelper::onMqttMessage(const String& topic, const String& payload) {
  if (topic == cmdTopic) {
    config.getInt("state")->setValue(payload == "ON" ? HIGH : LOW);
  } else if (topic == levelCmdTopic) {
    config.getInt("level")->setValue(payload.toInt());
  }
}

int HAlightHelper::pwmChannel = 0;
