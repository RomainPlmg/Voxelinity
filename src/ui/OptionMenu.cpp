#include "OptionMenu.h"

#include "core/Application.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/Camera.h"
#include "gfx/GraphicContext.h"
#include "pch.h"

int OptionMenu::m_MSAA = 0;

OptionMenu::OptionMenu(bool open) : UIWindow("OptionMenu", open) {}

void OptionMenu::Show() {
    if (!m_Open) return;

    // Center the window
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowBgAlpha(1.0f);  // Transparent background
    if (ImGui::Begin("OPTIONS", &m_Open, window_flags)) {
        ImGui::Text("OPTIONS");
        ImGui::Separator();
        ImGui::Separator();

        static bool vsyncEnabled = Application::GetStatus().vsync;
        ImGui::Checkbox("Enable V-Sync", &vsyncEnabled);
        if (vsyncEnabled) {
            ToggleVsyncEvent event(true);
            EventDispatcher::Get().Dispatch(event);
        } else {
            ToggleVsyncEvent event(false);
            EventDispatcher::Get().Dispatch(event);
        }

        static bool wireframeEnabled = false;
        ImGui::Checkbox("Enable wireframe", &wireframeEnabled);
        if (wireframeEnabled) {
            ToggleWireframeViewEvent event(true);
            EventDispatcher::Get().Dispatch(event);
        } else {
            ToggleWireframeViewEvent event(false);
            EventDispatcher::Get().Dispatch(event);
        }

        static float mouseSensitivity = CAMERA_SENSITIVITY;                               // Initial slider value
        ImGui::SliderFloat("Mouse sensitivity", &mouseSensitivity, 0.01f, 5.0f, "%.2f");  // Slider float
        SetMouseSensitivityEvent event(mouseSensitivity);
        EventDispatcher::Get().Dispatch(event);

        if (ImGui::Button("Back")) {
            Close();
            if (m_ParentMenu != nullptr) m_ParentMenu->Open();
        }
    }
    ImGui::End();
}
