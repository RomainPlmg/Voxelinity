#ifndef __EVENTMOUSE_H__
#define __EVENTMOUSE_H__

#include "Event.h"

class MouseEvent : public Event {
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
};

class MouseMotionEvent final : public MouseEvent {
   public:
    MouseMotionEvent(const double x, const double y) : posX(x), posY(y) {}
    ~MouseMotionEvent() override = default;

    double posX, posY;

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Mouse moved at: x=" << posX << " | y=" << posY;
        return ss.str();
    }

    EVENT_CLASS_TYPE(EventType::MouseMoved)
};

class MouseButtonPressedEvent final : public MouseEvent {
   public:
    explicit MouseButtonPressedEvent(const int mouseButtonCode) : m_ButtonCode(mouseButtonCode) {}
    ~MouseButtonPressedEvent() override = default;

    int m_ButtonCode;

    std::string ToString() const override {
        std::stringstream ss;
        ss << "MouseButtonPressed: " << m_ButtonCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
};

class MouseButtonReleasedEvent final : public MouseEvent {
   public:
    explicit MouseButtonReleasedEvent(const int mouseButtonCode) : m_ButtonCode(mouseButtonCode) {}
    ~MouseButtonReleasedEvent() override = default;

    int m_ButtonCode;

    std::string ToString() const override {
        std::stringstream ss;
        ss << "MouseButtonReleased: " << m_ButtonCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
};

#endif  // __EVENTMOUSE_H__
