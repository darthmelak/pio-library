#ifndef MqttHAHelpers_h
#define MqttHAHelpers_h

#include "WifiConfig.hpp"

class HAswitchHelper {
  public:
    HAswitchHelper(WifiConfig& wifiConfig, const char* suffix, int pin, bool invertState = false, bool debug = false);
    virtual ~HAswitchHelper() {};
    /**
     * to be called after wifiConfig.setupMqtt()
     */
    virtual void begin();
    Configuration& getConfig();
    virtual void onMqttConnect();
    virtual void onMqttMessage(const String& topic, const String& payload);
  protected:
    WifiConfig& wifiConfig;
    String suffix;
    int pin;
    bool invertState;
    Configuration config;
    bool debug;
    String cmdTopic;
    String stateTopic;
};

#endif
