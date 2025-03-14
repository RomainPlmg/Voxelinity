#ifndef __PAUSE_MENU_H__
#define __PAUSE_MENU_H__

#include <string>

#include "UIWindow.h"

class Application;

class PauseMenu : public UIWindow {
   public:
    PauseMenu(bool open = false);

    void Show() override;
    void OnEvent(const Event& event) override;

    /* Getters */
    UIWindow::Type GetType() override { return UIWindow::Type::Pause; }
};

#endif  // __PAUSE_MENU_H__