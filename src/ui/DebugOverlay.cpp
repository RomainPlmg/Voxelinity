#include "DebugOverlay.h"

#include "app/World.h"
#include "core/Application.h"
#include "core/Input.h"
#include "core/Window.h"
#include "gfx/GraphicContext.h"
#include "gfx/Renderer.h"
#include "pch.h"
#include "utils/Logger.h"
#include "utils/Time.h"

// FPS print refresh rate in second;
constexpr double FPSRefreshRate = 0.2;

DebugOverlay::DebugOverlay(bool open) : UIWindow("DebugOverlay", open) {}

void DebugOverlay::Show() {
    if (!m_Open) return;

    static int location = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0) {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    } else if (location == -2) {
        // Center window
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(.8f);  // Transparent background
    if (ImGui::Begin("DEBUG", &m_Open, window_flags)) {
        ImGui::Text(
            "DEBUG\n"
            "(right-click to change position)");
        ImGui::Separator();
        ImGui::Separator();

        // Performance
        m_FPSRefreshTime += Time::Get().GetDeltaTime();
        if (m_FPSRefreshTime >= FPSRefreshRate) {
            m_DeltaTime = Time::Get().GetDeltaTime();
            m_FPS = Time::Get().GetFPS();
            m_FPSRefreshTime = 0.0;
        }
        static int samples;
        glGetIntegerv(GL_SAMPLES, &samples);
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Text("Resolution: %dx%d", Application::GetStatus().resolution.x, Application::GetStatus().resolution.y);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", m_DeltaTime, m_FPS);
        ImGui::Text("VSync: %s", Application::GetStatus().vsync ? "Enable" : "Disable");
        ImGui::Text("MSAA: %dx", samples);
        ImGui::Text("Draw calls: %d", Application::GetStatus().drawcalls);
        ImGui::Text("Triangles: %d", Application::GetStatus().nbTrianglesToRender);
        ImGui::Separator();

        // World info
        auto pos = World::GetStatus().playerPos;
        ImGui::Text("XYZ: %.3f / %.3f / %.3f", pos.x, pos.y, pos.z);

        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
            if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
            if (&m_Open && ImGui::MenuItem("Close")) m_Open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
