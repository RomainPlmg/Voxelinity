#ifndef __EVENTDISPATCHER_H__
#define __EVENTDISPATCHER_H__

#include <functional>
#include <memory>
#include <shared_mutex>

#include "Event.h"

class EventDispatcher {
   public:
    EventDispatcher() = default;
    ~EventDispatcher();

    static void Init();
    static void Shutdown();

    static EventDispatcher& Get();

    using Listener = std::function<void(const Event&)>;

    // Add a listener to a specific event category
    void Subscribe(EventCategory category, const Listener& listener);
    // Delete a listener to a specific event category
    void Unsubscribe(EventCategory category, const Listener& listener);
    // Push an event on the bus
    void Dispatch(Event& event);

    static std::shared_ptr<EventDispatcher> Create();

   private:
    std::unordered_map<EventCategory, std::vector<Listener>> m_Listeners;
    mutable std::shared_mutex m_Mutex;

    static std::shared_mutex s_InitMutex;
};

#endif  // __EVENTDISPATCHER_H__
