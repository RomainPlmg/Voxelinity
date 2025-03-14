#ifndef __UI_MANAGER_H__
#define __UI_MANAGER_H__

#include <memory>
#include <string>
#include <unordered_map>

#include "UIWindow.h"

class Application;
struct GLFWwindow;

class UIManager {
   public:
    UIManager();

    void Init(GLFWwindow* handler);
    void Update();
    void Render();
    void Shutdown();

    static std::unique_ptr<UIManager> Create();

   private:
    std::unordered_map<std::string, UIWindow*> m_Windows;
};

#endif  // __UI_MANAGER_H__