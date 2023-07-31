#ifndef EVENTEMITTER_HPP_
#define EVENTEMITTER_HPP_

#include <vector>
#include <unordered_map>

#include "EventListener.hpp"

template <class T>
class EventEmitter
{
public:
    void addEventListener(const std::string &eventName, EventListener<T> *eventListener)
    {
        _listeners[eventName].push_back(eventListener);
    }

    void dispatchEvent(const std::string &eventName, const T &eventData)
    {
        for (EventListener<T> *listener : _listeners[eventName])
        {
            listener->handleEvent(eventName, eventData);
        }
    }

protected:
    std::unordered_map<std::string, std::vector<EventListener<T> *>> _listeners;
};

#endif // EVENTEMITTER_HPP_