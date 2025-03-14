//
// Created by Romain on 21/12/2024.
//

#include "EventDispatcher.h"

#include "pch.h"

/*static*/ EventDispatcher* s_EventDispatcherInst = nullptr;

EventDispatcher::~EventDispatcher() { m_Listeners.clear(); }

void EventDispatcher::Init() { s_EventDispatcherInst = new EventDispatcher(); }

void EventDispatcher::Shutdown() {
    delete s_EventDispatcherInst;
    s_EventDispatcherInst = nullptr;
}

EventDispatcher& EventDispatcher::Get() {
    assert(s_EventDispatcherInst != nullptr);
    return *s_EventDispatcherInst;
}

void EventDispatcher::Subscribe(const EventCategory category, const Listener& listener) { m_Listeners[category].push_back(listener); }

void EventDispatcher::Unsubscribe(const EventCategory category, const Listener& listener) {
    auto& listeners = m_Listeners[category];
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        if (it->target_type() == listener.target_type() && it->target<void()>() == listener.target<void>()) {
            listeners.erase(it);
            break;
        }
    }
}

void EventDispatcher::Dispatch(Event& event) {
    for (const auto& listeners : m_Listeners) {
        if (event.IsInCategory(listeners.first)) {
            for (const auto& listener : listeners.second) {
                listener(event);
            }
        }
    }
}

std::shared_ptr<EventDispatcher> EventDispatcher::Create() { return std::make_shared<EventDispatcher>(); }
