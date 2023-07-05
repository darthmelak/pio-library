#ifndef Configuration_h
#define Configuration_h

#include "ConfigTypes.hpp"

class Configuration {
    public:
        Configuration();
        void setup();
        void add(StringConfig *config);
        StringConfig *get(String name);
    private:
        StringConfig *first;
        StringConfig *last;
};

#endif
