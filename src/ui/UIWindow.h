#ifndef __UI_WINDOW__
#define __UI_WINDOW__

#include <unordered_map>

#include "events/Event.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils/Logger.h"

// Pure virtual class for all UI elements
class UIWindow {
   public:
    UIWindow(const std::string& name, bool open = false) : m_Open(open), m_Name(name), m_ParentMenu(nullptr) {}
    enum Type { None = 0, Debug = 1 << 0, Pause = 1 << 1 };

    virtual ~UIWindow() = default;  // Virtual destructor

    virtual void Show() = 0;
    void Open() { m_Open = true; }
    void Close() { m_Open = false; }
    void Toggle() { m_Open = !m_Open; }
    void AddChild(UIWindow* window) {
        if (window->GetParent() != nullptr) {
            LOG_ERROR("The menu {0} already has a parent menu, can't be added as child for {1}", window->GetName(), m_Name);
            return;
        }
        m_ChildMenus[window->GetName()] = window;
        window->SetParentMenu(this);
    }

    virtual void OnEvent(const Event& event) {}

    /* Getters */
    virtual Type GetType() = 0;
    const std::string& GetName() const { return m_Name; }
    const UIWindow* GetParent() const { return m_ParentMenu; }

    /* Setters */
    void SetParentMenu(UIWindow* window) { m_ParentMenu = window; }

   protected:
    bool m_Open;
    std::string m_Name;
    UIWindow* m_ParentMenu;
    std::unordered_map<std::string, UIWindow*> m_ChildMenus;
};

#endif  // __UI_WINDOW__