#ifndef __OPTION_MENU_H__
#define __OPTION_MENU_H__

#include "UIWindow.h"

class OptionMenu : public UIWindow {
   public:
    OptionMenu(bool open = false);
    void Show() override;

    /* Getters */
    UIWindow::Type GetType() override { return UIWindow::Type::Pause; }

   private:
    static int m_MSAA;
};

#endif