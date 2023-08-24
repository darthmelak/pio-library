#ifndef HAlightHelper_h
#define HAlightHelper_h

#include "HAswitchHelper.hpp"

class HAlightHelper : public HAswitchHelper {
  public:
    HAlightHelper(WifiConfig& wifiConfig, const char* suffix, int pin, int maxLevel = 255, int minOffset = 0, int maxOffset = 0, bool invertState = false, bool debug = false);
    virtual ~HAlightHelper() {};
    virtual void begin();
    virtual void onMqttConnect();
    virtual void onMqttMessage(const String& topic, const String& payload);
  protected:
    int level;
    int maxLevel;
    int minOffset;
    int maxOffset;
    String levelStateTopic;
    String levelCmdTopic;
};

#endif
