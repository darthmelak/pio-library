// Testbed for the libraries, led on
#include <Arduino.h>
#include <arduino-timer.h>
#include <OneButton.h>
#include "Configurations.hpp"
#include "WifiConfig.hpp"
#include "HAswitchHelper.hpp"
#include "HAlightHelper.hpp"
#include "SerialHandler.hpp"
#include "secrets.h"

#ifdef ESP8266
int yellow = D7;
int red = D6;
int green = D1;
int white = D2;
int button = D5;
#else
int yellow = 7;
int red = 6;
int green = 5;
int white = 4;
int button = 3;
#endif

bool debug = true;
bool echoCount = false;

Configuration config("/test", debug);
WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed", AUTH_USER, AUTH_PASS, true, true, debug);
HAswitchHelper sw_1(wifiConfig, "whiteled", white, true, debug);
HAlightHelper light_1(wifiConfig, "redled", red, 255, 0, 100, true, debug);
Timer<2> timer;
OneButton btn(button);

void serialCb(String);

void setup() {
  if (debug) {
    Serial.begin(115200);
    delay(10);
  }

  analogWriteFreq(4000);

  config
    .add("counter", 0, [](int value) {
      // wifiConfig.publish("state/counter", String(value));
    })
    .add("green", 0, [](int value) {
      Serial.printf("Green changed to: %d\n", value);
      analogWrite(green, value);
    })
  ;

  btn.attachClick([]() {
    IntConfig *state = light_1.getConfig().getInt("state");
    IntConfig *level = light_1.getConfig().getInt("level");
    if (state->getIntVal()) {
      if (level->getIntVal() < 255) {
        level->setValue(255);
      } else {
        state->setValue(0);
      }
    } else {
      level->setValue(255);
      state->setValue(1);
    }
  });

  wifiConfig.registerConfigApi(config, [](bool changed) {
    if (changed) {
      Serial.println("Config updated");
    } else {
      Serial.println("Config unchanged");
    }
  });

  pinMode(yellow, OUTPUT);
  digitalWrite(yellow, LOW);
  pinMode(green, OUTPUT);
  digitalWrite(green, LOW);
  // timer.every(1000, [](void*) -> bool {
  //   digitalWrite(green, !digitalRead(green));
  //   IntConfig *counter = config.getInt("counter");
  //   counter->setValue(counter->getIntVal() + 1);
  //   if (debug && echoCount) Serial.println(counter->getValue());
  //   return true;
  // });

  wifiConfig.beginMQTT(
    MQTT_SERVER,
    1883,
    MQTT_USER,
    MQTT_PASS,
    "homeassistant/",
    MQTTConnectProps([]() {
      // wifiConfig.subscribe("/cmd/led");
      sw_1.onMqttConnect();
      light_1.onMqttConnect();
    }, [](String topic, String data) {
      // if (topic == wifiConfig.getPrefixedTopic("/cmd/led")) {
      //   int state = data.toInt();
      //   config.getInt("light")->setValue(state);
      // }
      sw_1.onMqttMessage(topic, data);
      light_1.onMqttMessage(topic, data);
    })
  );
  sw_1.begin();
  light_1.begin();
}

void loop() {
  handleSerial(debug, serialCb);
  wifiConfig.loop();
  digitalWrite(yellow, wifiConfig.isWifiConnected() ? LOW : HIGH);
  timer.tick();
  btn.tick();
  delay(1);
}

void serialCb(String buffer) {
  if (buffer == "echo") {
    echoCount = !echoCount;
    Serial.println("Echo counter: " + String(echoCount));
  }
  if (buffer == "test") {
    StringConfig *item = config.getFirst();
    while (item != NULL) {
      Serial.println(item->getName() + ": " + item->getValue());
      item = item->getNext();
    }
  }
  if (buffer == "json") {
    DynamicJsonDocument json(256);
    config.toJson(json);
    String response;
    serializeJson(json, response);
    Serial.println(response);
  }
  if (buffer == "reset") {
    Serial.println("Resetting config");
    SavedConfiguration wconf = wifiConfig.getConfig();
    wconf.get("ssid")->setValue(WIFI_SSID);
    wconf.get("password")->setValue(WIFI_PASSWORD);
    wconf.get("auth_user")->setValue(AUTH_USER);
    wconf.get("auth_pass")->setValue(AUTH_PASS);
    ESP.restart();
  }
}
