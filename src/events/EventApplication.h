#ifndef __EVENTAPPLICATION_H__
#define __EVENTAPPLICATION_H__

#include "Event.h"
#include "gfx/Renderable.h"

class ApplicationEvent : public Event {
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class WindowResizeEvent final : public ApplicationEvent {
   public:
    int width, height;

    WindowResizeEvent(const int width, const int height) : width(width), height(height) {}

    EVENT_CLASS_TYPE(EventType::WindowResize)

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Window resized to: " << width << "x" << height;
        return ss.str();
    }
};

class WindowCloseEvent final : public ApplicationEvent {
   public:
    EVENT_CLASS_TYPE(EventType::WindowClose)

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Window closed";
        return ss.str();
    }
};

class WindowMovedEvent final : public ApplicationEvent {
   public:
    int xpos, ypos;

    WindowMovedEvent(const int xpos, const int ypos) : xpos(xpos), ypos(ypos) {}

    EVENT_CLASS_TYPE(EventType::WindowMoved)

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Window moved at: x=" << xpos << " | y=" << ypos;
        return ss.str();
    }
};

class PauseEvent final : public ApplicationEvent {
   public:
    bool isPaused;

    PauseEvent(bool isPaused) : isPaused(isPaused) {}

    EVENT_CLASS_TYPE(EventType::AppPause)

    std::string ToString() const override {
        std::stringstream ss;
        if (isPaused)
            ss << "App pause";
        else
            ss << "App resume";
        return ss.str();
    }
};

class ToggleVsyncEvent final : public ApplicationEvent {
   public:
    bool enable;

    ToggleVsyncEvent(bool enable) : enable(enable) {}

    EVENT_CLASS_TYPE(EventType::ToggleVsync)

    std::string ToString() const override {
        std::stringstream ss;
        if (enable)
            ss << "Vsync enable";
        else
            ss << "Vsync disable";
        return ss.str();
    }
};

class SetMSAAEvent final : public ApplicationEvent {
   public:
    int value;

    SetMSAAEvent(int value) : value(value) {}

    EVENT_CLASS_TYPE(EventType::SetMSAA)

    std::string ToString() const override {
        std::stringstream ss;
        ss << "MSAA = " << value << "x";
        return ss.str();
    }
};

class ToggleWireframeViewEvent final : public ApplicationEvent {
   public:
    bool enable;

    ToggleWireframeViewEvent(bool enable) : enable(enable) {}

    EVENT_CLASS_TYPE(EventType::ToggleWireframeView)

    std::string ToString() const override {
        std::stringstream ss;
        if (enable)
            ss << "Wireframe enable";
        else
            ss << "Wireframe disable";
        return ss.str();
    }
};

class SetMouseSensitivityEvent final : public ApplicationEvent {
   public:
    float value;

    SetMouseSensitivityEvent(float value) : value(value) {}

    EVENT_CLASS_TYPE(EventType::SetMouseSensitivity)

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Mouse sensitivity = " << value;
        return ss.str();
    }
};

class AddNewRenderableEvent final : public ApplicationEvent {
   public:
    const Renderable* GetRenderable() const { return m_Renderable; }

    AddNewRenderableEvent(Renderable* renderable) : m_Renderable(renderable) {}

    EVENT_CLASS_TYPE(EventType::AddNewRenderable);

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Add renderable ID = " << m_Renderable->GetID();
        return ss.str();
    }

   private:
    Renderable* m_Renderable;
};

class ChunkDataGeneratedEvent final : public ApplicationEvent {
   public:
    ChunkDataGeneratedEvent() {}

    EVENT_CLASS_TYPE(EventType::ChunkDataGenerated);

    std::string ToString() const override {
        std::stringstream ss;
        ss << "Chunk data generated";
        return ss.str();
    }
};

class SetGodModeEvent final : public ApplicationEvent {
    public:
     bool god;
 
     SetGodModeEvent(bool god) : god(god) {}
 
     EVENT_CLASS_TYPE(EventType::GodMode)
 
     std::string ToString() const override {
         std::stringstream ss;
         if (god)
             ss << "God mode enable";
         else
             ss << "God mode disable";
         return ss.str();
     }
 };

#endif  // __EVENTAPPLICATION_H__
