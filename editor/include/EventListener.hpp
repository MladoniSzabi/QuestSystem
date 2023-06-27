#ifndef EVENTLISTENER_HPP_
#define EVENTLISTENER_HPP_

#include <string>

class EventListener
{
public:
    virtual void handleEvent(const std::string &eventName, void *eventData) = 0;
};

#endif // EVENTLISTENER_HPP_