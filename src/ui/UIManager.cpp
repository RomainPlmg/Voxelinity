#include "UIManager.h"

#include "DebugOverlay.h"
#include "OptionMenu.h"
#include "PauseMenu.h"
#include "core/Application.h"
#include "core/Window.h"
#include "events/Event.h"
#include "gfx/GraphicContext.h"
#include "pch.h"
#include "utils/Logger.h"

UIManager::UIManager() {
    m_Windows["DebugOverlay"] = new DebugOverlay;
    m_Windows["PauseMenu"] = new PauseMenu;
    m_Windows["OptionMenu"] = new OptionMenu;

    m_Windows["PauseMenu"]->AddChild(m_Windows["OptionMenu"]);
}

void UIManager::Init(GLFWwindow* handler) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.Fonts->AddFontFromFileTTF(ASSET_DIRECTORY "fonts/RobotoMono-VariableFont_wght.ttf", 18);

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 2.f;
    style.ScrollbarRounding = 0.f;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(handler,
                                 true);  // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

#ifndef _NDEBUG
    m_Windows["DebugOverlay"]->Open();
#endif
}

void UIManager::Update() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto& [name, window] : m_Windows) {
        window->Show();
    }
}

void UIManager::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Shutdown() {
    for (auto& [name, window] : m_Windows) {
        window->Close();
    }
    m_Windows.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

std::unique_ptr<UIManager> UIManager::Create() { return std::make_unique<UIManager>(); }
