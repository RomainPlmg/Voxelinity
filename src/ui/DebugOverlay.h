#ifndef __DEBUG_OVERLAY_H__
#define __DEBUG_OVERLAY_H__

#include <memory>

#include "UIWindow.h"

class Application;

class DebugOverlay : public UIWindow {
   public:
    DebugOverlay(bool open = false);
    ~DebugOverlay() override = default;

    void Show() override;

    /* Getters */
    UIWindow::Type GetType() override { return UIWindow::Type::Debug; }

   private:
    double m_FPSRefreshTime = 0.0;
    double m_DeltaTime = 0.0, m_FPS = 0.0;
};

#endif  // __DEBUG_OVERLAY_H__