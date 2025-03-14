#include "PauseMenu.h"

#include "OptionMenu.h"
#include "core/Application.h"
#include "core/Window.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "pch.h"
#include "utils/Logger.h"

PauseMenu::PauseMenu(bool open) : UIWindow("PauseMenu", open) {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(PauseMenu::OnEvent));
}

void PauseMenu::Show() {
    if (!m_Open) return;

    // Center the window
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowBgAlpha(1.0f);  // Transparent background
    if (ImGui::Begin("PAUSE", &m_Open, window_flags)) {
        ImGui::Text("PAUSE");
        ImGui::Separator();
        ImGui::Separator();

        if (ImGui::Button("Resume")) {
            PauseEvent event(false);
            EventDispatcher::Get().Dispatch(event);
        }

        if (ImGui::Button("Options")) {
            Close();
            m_ChildMenus["OptionMenu"]->Open();
        }

        if (ImGui::Button("Exit")) {
            WindowCloseEvent event;
            EventDispatcher::Get().Dispatch(event);
        }
    }
    ImGui::End();
}

void PauseMenu::OnEvent(const Event& event) {
    if (event.GetType() == EventType::AppPause) {
        const auto* pauseEvent = dynamic_cast<const PauseEvent*>(&event);
        m_Open = pauseEvent->isPaused;
        if (!m_Open) {
            for (auto& [name, window] : m_ChildMenus) {
                window->Close();
            }
        }
    }
}
