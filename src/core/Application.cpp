#include "Application.h"

#include "Input.h"
#include "ThreadPool.h"
#include "Window.h"
#include "app/Player.h"
#include "app/World.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/Camera.h"
#include "gfx/Renderer.h"
#include "pch.h"
#include "ui/UIManager.h"
#include "utils/Logger.h"
#include "utils/Time.h"

AppStatus Application::m_AppStatus;

Application::Application() : m_Renderer(nullptr) {}

Application::~Application() {}

void Application::Init() {
    // Init event dispatcher and subscribe
    EventDispatcher::Init();
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(Application::OnEvent));

    // Init the threadpool
    ThreadPool::Init(16);

    // Init window
    m_Window = Window::Create();
    m_Window->Init();

    Input::Init(m_Window);  // Init input system
    Time::Init();           // Init timer

    // Create the world
    m_World = World::Create();
    m_World->Init();

    // Init the GUI
    m_UIManager = UIManager::Create();
    m_UIManager->Init(m_Window->GetHandler());

    // Provide a camera and rendererables to the renderer
    m_Renderer = m_Window->GetRenderer();
    if (m_Renderer == nullptr) {
        LOG_FATAL("Renderer initialization failed");
    } else {
        // Setup the camera for the renderer
        m_Renderer->SetCamera(m_World->GetPlayer().GetCamera());
    }
}

void Application::Run() {
    /* Loop until the user closes the window */
    m_Window->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (!m_Window->ShouldClose()) {
        // Clear the content of the window
        m_Window->Clear();

        // Updates
        Time::Get().Update();
        m_World->Update();

        m_Window->Update();
        m_UIManager->Update();

        m_Renderer->Render();   // My Renderer
        m_UIManager->Render();  // ImGUI Renderer, last called because overlay

        m_Window->SwapBuffers();  // Swap buffers so after any other render method

        // Update status
        m_AppStatus.FPS = Time::Get().GetFPS();
        m_AppStatus.drawcalls = m_Window->GetRenderer()->GetDrawcalls();
        m_AppStatus.nbTrianglesToRender = m_Window->GetRenderer()->GetNbTrianglesRendered();
        m_AppStatus.vsync = m_Window->GetProps()->vsync;
        m_AppStatus.resolution = glm::ivec2(m_Window->GetProps()->width, m_Window->GetProps()->height);
    }
}

void Application::Close() {
    m_UIManager->Shutdown();
    m_Window->Shutdown();
    LOG_INFO("Application closed.");
}

void Application::OnEvent(const Event& event) {
    if (event.GetType() == EventType::WindowClose) {
        const auto* closeEvent = dynamic_cast<const WindowCloseEvent*>(&event);
        m_Window->Close();
    }
}

Application* Application::Create() { return new Application(); }
