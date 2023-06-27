#ifndef EVENTEMITTER_HPP_
#define EVENTEMITTER_HPP_

#include <functional>

#include "EventListener.hpp"

class EventEmitter
{
public:
    virtual void addEventListener(const std::string &eventName, EventListener *eventListener) = 0;
};

#endif // EVENTEMITTER_HPP_