#ifndef HAfanHelper_h
#define HAfanHelper_h

#include "HAlightHelper.hpp"

class HAfanHelper : public HAlightHelper {
  public:
    HAfanHelper(WifiConfig& wifiConfig, const char* suffix, int pin, int maxLevel = 8, int minOffset = 0, int maxOffset = 0, bool invertState = false, bool debug = false);
    virtual ~HAfanHelper() {};
    virtual void begin();
    virtual void onMqttConnect();
    virtual void onMqttMessage(const String& topic, const String& payload);
    virtual void tick();
    void setCb(int_update_cb cb);
  protected:
    int delta;
    unsigned int nextDelay;
    unsigned long lastOscillate;
    String oscillateStateTopic;
    String oscillateCmdTopic;
    virtual void oscillateStep();
    int_update_cb rawCb;
};

#endif
