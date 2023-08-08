// Testbed for the libraries, led on
#include <Arduino.h>
#include <arduino-timer.h>
#include <OneButton.h>
#include "Configurations.hpp"
#include "WifiConfig.hpp"
#include "SerialHandler.hpp"
#include "secrets.h"

bool debug = true;
bool echoCount = false;

Configuration config("/test", debug);
WifiConfig wifiConfig(WIFI_SSID, WIFI_PASSWORD, "Testbed", "testbed", AUTH_USER, AUTH_PASS, true, true, debug);
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
Timer<2> timer;
OneButton btn(button);

void serialCb(String);

void setup() {
  if (debug) {
    Serial.begin(115200);
    delay(10);
  }

  config
    .add("ssid", WIFI_SSID)
    .add("password", WIFI_PASSWORD)
    .add("name", "Testbed")
    .add("hostname", "testbed", [](String value) {
      Serial.printf("Hostname test changed to: %s\n", value.c_str());
    })
    .add("counter", 0, [](int value) {
      wifiConfig.publish("/state/counter", String(value));
    })
    .add("light", 0, [](int value) {
      Serial.printf("Red changed to: %d\n", value);
      analogWrite(red, value);
      wifiConfig.publish("/state/led", String(value));
    })
  ;

  IntConfig *light = config.getInt("light");

  btn.attachClick([](void *ctx) {
    IntConfig *lght = (IntConfig *)ctx;
    lght->setValue(lght->getIntVal() > 0 ? 0 : 255);
  }, light);

  wifiConfig.registerConfigApi(config, [](bool changed) {
    if (changed) {
      Serial.println("Config updated");
    } else {
      Serial.println("Config unchanged");
    }
  });

  pinMode(yellow, OUTPUT);
  digitalWrite(yellow, LOW);
  pinMode(red, OUTPUT);
  digitalWrite(red, LOW);
  pinMode(green, OUTPUT);
  digitalWrite(green, LOW);
  pinMode(white, OUTPUT);
  digitalWrite(white, LOW);
  pinMode(button, INPUT_PULLUP);
  timer.every(1000, [](void*) -> bool {
    digitalWrite(green, !digitalRead(green));
    IntConfig *counter = config.getInt("counter");
    counter->setValue(counter->getIntVal() + 1);
    if (debug && echoCount) Serial.println(counter->getValue());
    return true;
  });
  // config.setup();

  wifiConfig.setupMQTT(
    MQTT_SERVER,
    1883,
    MQTT_USER,
    MQTT_PASS,
    "testbed/",
    MQTTConnectProps([]() {
      wifiConfig.subscribe("/cmd/led");
    }, [](String topic, String data) {
      if (topic == wifiConfig.getPrefixedTopic("/cmd/led")) {
        int state = data.toInt();
        config.getInt("light")->setValue(state);
      }
    })
  );
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
