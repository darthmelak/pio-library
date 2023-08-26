#ifndef HAnumberHelper_h
#define HAnumberHelper_h

#include "WifiConfig.hpp"

class HAnumberHelper {
  public:
    HAnumberHelper(WifiConfig& wifiConfig, const char* suffix, int_update_cb onUpdate, int default_value = 50, int min = 1, int max = 100, int step = 1, bool debug = false);
    virtual ~HAnumberHelper() {};
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
    int_update_cb onUpdate;
    int default_value;
    int min;
    int max;
    int step;
    Configuration config;
    bool debug;
    String cmdTopic;
    String stateTopic;
};

#endif
