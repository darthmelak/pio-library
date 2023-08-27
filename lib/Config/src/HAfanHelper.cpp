#include <Arduino.h>
#include "HAfanHelper.hpp"

#define RANDOM_UPDATE_INTERVAL 15
#define RANDOM_UPDATE_JITTER 5

HAfanHelper::HAfanHelper(WifiConfig& wifiConfig, const char* suffix, int pin, int maxLevel, int minOffset, int maxOffset, bool invertState, bool debug): HAlightHelper(wifiConfig, suffix, pin, maxLevel, minOffset, maxOffset, invertState, debug), nextDelay(0), lastOscillate(0) {
  delta = maxLevel / 2;
}

void HAfanHelper::begin() {
  pinMode(pin, OUTPUT);
  #ifdef ESP32
  ledcAttachPin(pin, pwmChannel);
  int ledStatus = ledcSetup(pwmChannel, 25000, 8);
  if (debug) Serial.printf("ledcSetup channel: %d, status: %d\n", pwmChannel, ledStatus);
  pwmChannel++;
  #endif
  digitalWrite(pin, invertState ? HIGH : LOW);

  wifiConfig.getPrefixedTopic(cmdTopic, "fan/{sensorId}_{suffix}/cmd/state");
  cmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(stateTopic, "fan/{sensorId}_{suffix}/status/state");
  stateTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(levelCmdTopic, "fan/{sensorId}_{suffix}/cmd/level");
  levelCmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(levelStateTopic, "fan/{sensorId}_{suffix}/status/level");
  levelStateTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(oscillateCmdTopic, "fan/{sensorId}_{suffix}/cmd/oscillate");
  oscillateCmdTopic.replace("{suffix}", suffix);
  wifiConfig.getPrefixedTopic(oscillateStateTopic, "fan/{sensorId}_{suffix}/status/oscillate");
  oscillateStateTopic.replace("{suffix}", suffix);

  config
    .add("state", LOW, [this](int value) {
      if (value == LOW) {
        config.getInt("actualLevel")->setValue(0);
        config.getInt("oscillate")->setValue(0);
      } else {
        if (config.getInt("oscillate")->getIntVal()) {
          oscillateStep();
        } else {
          int lvl = config.getInt("level")->getIntVal();
          config.getInt("actualLevel")->setValue(lvl);
        }
      }
      String state = value ? "ON" : "OFF";
      wifiConfig.publish(stateTopic, state, true, false);
    })
    .add("level", 1, [this](int value) {
      if (
        invertState ?
        level < 255 :
        level > 0
      ) {
        config.getInt("actualLevel")->setValue(value);
      }
      wifiConfig.publish(levelStateTopic, String(value), true, false);
    })
    .add("oscillate", 0, [this](int value) {
      int state = config.getInt("state")->getIntVal();

      if (state && value) {
        oscillateStep();
      } else {
        nextDelay = 0;
      }
      wifiConfig.publish(oscillateStateTopic, state && value ? "oscillate_on" : "oscillate_off", true, false);
    })
    .add("actualLevel", 0, [this](int value) {
      level = invertState ?
        mapRng(value, 0, maxLevel, 255 - maxOffset, minOffset) :
        mapRng(value, 0, maxLevel, minOffset, 255 - maxOffset);
      // if off,ignore min/max offset
      if (value == 0) {
        level = invertState ? 255 : 0;
      }
      if (debug) Serial.printf("Level: %d, PWMlevel: %d\n", value, level);
      analogWrite(pin, level);
    })
  ;
}

void HAfanHelper::onMqttConnect() {
  String configTopic;
  wifiConfig.getPrefixedTopic(configTopic, "fan/{sensorId}_{suffix}/config");
  configTopic.replace("{suffix}", suffix);

  wifiConfig.publish(configTopic, wifiConfig.fanConfigPayload(suffix, cmdTopic, stateTopic, levelCmdTopic, levelStateTopic, oscillateCmdTopic, oscillateStateTopic, maxLevel), true, false);
  wifiConfig.subscribe(cmdTopic, false);
  wifiConfig.publish(stateTopic, "OFF", true, false);
  wifiConfig.subscribe(levelCmdTopic, false);
  wifiConfig.publish(levelStateTopic, "1", true, false);
  wifiConfig.subscribe(oscillateCmdTopic, false);
  wifiConfig.publish(oscillateStateTopic, "oscillate_off", true, false);
}

void HAfanHelper::onMqttMessage(const String& topic, const String& payload) {
  if (topic == cmdTopic) {
    config.getInt("state")->setValue(payload == "ON" ? HIGH : LOW);
  } else if (topic == levelCmdTopic) {
    config.getInt("level")->setValue(payload.toInt());
  } else if (topic == oscillateCmdTopic) {
    config.getInt("oscillate")->setValue(payload == "oscillate_on" ? HIGH : LOW);
  }
}

void HAfanHelper::tick() {
  if (nextDelay) {
    if (millis() - lastOscillate > nextDelay) {
      oscillateStep();
    }
  }
}

void HAfanHelper::oscillateStep() {
  lastOscillate = millis();

  int lvl = config.getInt("level")->getIntVal();
  int lvlFrom = lvl < delta ? 1 : lvl - delta;
  int randLvl = random(lvlFrom, lvl + 1);
  config.getInt("actualLevel")->setValue(randLvl);

  int jitter = random(-RANDOM_UPDATE_JITTER, RANDOM_UPDATE_JITTER);
  nextDelay = (RANDOM_UPDATE_INTERVAL + jitter) * 1000;

  if (debug) Serial.printf("OscillateStep, delay: %d\n", nextDelay);
}
