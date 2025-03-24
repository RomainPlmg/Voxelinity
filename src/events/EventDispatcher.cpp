//
// Created by Romain on 21/12/2024.
//

#include "EventDispatcher.h"

#include "pch.h"

/*static*/ EventDispatcher* s_EventDispatcherInst = nullptr;
std::shared_mutex EventDispatcher::s_InitMutex;

EventDispatcher::~EventDispatcher() {
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    m_Listeners.clear();
}

void EventDispatcher::Init() {
    std::unique_lock<std::shared_mutex> lock(s_InitMutex);
    s_EventDispatcherInst = new EventDispatcher();
}

void EventDispatcher::Shutdown() {
    std::unique_lock<std::shared_mutex> lock(s_InitMutex);
    delete s_EventDispatcherInst;
    s_EventDispatcherInst = nullptr;
}

EventDispatcher& EventDispatcher::Get() {
    std::shared_lock<std::shared_mutex> lock(s_InitMutex);
    assert(s_EventDispatcherInst != nullptr);
    return *s_EventDispatcherInst;
}

void EventDispatcher::Subscribe(const EventCategory category, const Listener& listener) {
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    m_Listeners[category].push_back(listener);
}

void EventDispatcher::Unsubscribe(const EventCategory category, const Listener& listener) {
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    auto& listeners = m_Listeners[category];

    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        if (it->target_type() == listener.target_type() && it->target<void()>() == listener.target<void>()) {
            listeners.erase(it);
            break;
        }
    }
}

void EventDispatcher::Dispatch(Event& event) {
    std::shared_lock<std::shared_mutex> lock(m_Mutex);

    for (const auto& [category, listeners] : m_Listeners) {
        if (event.IsInCategory(category)) {  // Checks if the event belongs to this category
            for (const auto& listener : listeners) {
                listener(event);  // Calls the listeners without copying the list
            }
        }
    }
}

std::shared_ptr<EventDispatcher> EventDispatcher::Create() { return std::make_shared<EventDispatcher>(); }
