#ifndef __EVENTKEYBOARD_H__
#define __EVENTKEYBOARD_H__

#include "Event.h"

class KeyEvent : public Event {
   public:
    int GetKeyCode() const { return m_KeyCode; }
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

   protected:
    explicit KeyEvent(const int keycode) : m_KeyCode(keycode) {}

    int m_KeyCode;
};

class KeyPressedEvent final : public KeyEvent {
   public:
    explicit KeyPressedEvent(const int keycode, const bool isRepeat = false) : KeyEvent(keycode), m_IsRepeat(isRepeat) {}
    ~KeyPressedEvent() override = default;

    inline bool IsRepeat() const { return m_IsRepeat; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyPressed: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";

        return ss.str();
    }

    EVENT_CLASS_TYPE(EventType::KeyPressed)

   private:
    bool m_IsRepeat;
};

class KeyReleasedEvent final : public KeyEvent {
   public:
    explicit KeyReleasedEvent(const int keycode) : KeyEvent(keycode) {}
    ~KeyReleasedEvent() override = default;

    std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyReleased: " << m_KeyCode;

        return ss.str();
    }

    EVENT_CLASS_TYPE(EventType::KeyReleased)
};

#endif  // __EVENTKEYBOARD_H__
