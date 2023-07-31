#ifndef EVENTLISTENER_HPP_
#define EVENTLISTENER_HPP_

#include <string>

template <class T>
class EventListener
{
public:
    virtual void handleEvent(const std::string &eventName, const T &eventData) = 0;
};

#endif // EVENTLISTENER_HPP_