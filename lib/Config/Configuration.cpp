#include "Configuration.hpp"

Configuration::Configuration() {}

void Configuration::setup() {
    StringConfig *current = first;
    while (current != NULL) {
        current->setup();
        current = current->getNext();
    }
}

void Configuration::add(StringConfig *config) {
    if (first == NULL) {
        first = config;
        last = config;
    } else {
        last->setNext(config);
        last = config;
    }
}

StringConfig *Configuration::get(String name) {
    StringConfig *current = first;
    while (current != NULL) {
        if (current->getName() == name) {
            return current;
        }
        current = current->getNext();
    }
    return NULL;
}
