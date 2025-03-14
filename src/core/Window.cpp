#include "Window.h"

#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "events/EventKeyboard.h"
#include "events/EventMouse.h"
#include "gfx/GraphicContext.h"
#include "gfx/Renderer.h"
#include "pch.h"
#include "utils/Logger.h"

Window::Window() : m_Handler(nullptr) { m_Props = std::make_unique<WindowProps>(); }

void Window::Init() {
    // Initialize the library
    if (!glfwInit()) {
        LOG_FATAL("Failed to initialize GLFW");
    };

    /* Setup error callback */
    glfwSetErrorCallback(_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a windowed mode window and its OpenGL context
    m_Handler = glfwCreateWindow(m_Props->width, m_Props->height, "Voxelinity", NULL, NULL);
    if (!m_Handler) {
        LOG_ERROR("Unable to create the GLFW window.");
        glfwTerminate();
        return;
    }

    /* Setup window callbacks */
    glfwSetFramebufferSizeCallback(m_Handler, _framebuffer_size_callback);
    glfwSetKeyCallback(m_Handler, _key_callback);
    glfwSetCursorPosCallback(m_Handler, _mouse_cursor_pos_callback);
    glfwSetMouseButtonCallback(m_Handler, _mouse_button_callback);
    glfwSetWindowPosCallback(m_Handler, _window_pos_callback);

    m_GraphicContext = GraphicContext::Create(m_Handler);
    m_GraphicContext->Init();

    // VSYNC, to enable after context creation
    if (m_Props->vsync) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }

    m_Renderer = Renderer::Create(m_Props->width, m_Props->height);
    m_Renderer->Init();

    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(Window::OnEvent));
    CaptureMouse(true);
}

void Window::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void Window::Update() {
    /* Poll for and process events */
    glfwPollEvents();
}

void Window::SwapBuffers() {
    /* Swap front and back buffers */
    glfwSwapBuffers(m_Handler);
}

void Window::Shutdown() {
    glfwDestroyWindow(m_Handler);
    glfwTerminate();
}

const bool Window::ShouldClose() const { return glfwWindowShouldClose(m_Handler); }

void Window::Close() { glfwSetWindowShouldClose(m_Handler, true); }

void Window::OnEvent(const Event& event) {
    if (event.GetType() == EventType::WindowResize) {
        const auto* winResizeEvent = dynamic_cast<const WindowResizeEvent*>(&event);
        m_Props->width = winResizeEvent->width;
        m_Props->height = winResizeEvent->height;
        if (winResizeEvent->width > 0 || winResizeEvent->height > 0) m_Renderer->SetViewport(winResizeEvent->width, winResizeEvent->height);
    }
    if (event.GetType() == EventType::AppPause) {
        const auto* pauseEvent = dynamic_cast<const PauseEvent*>(&event);
        if (pauseEvent->isPaused) {
            CaptureMouse(false);
            SetMousePosition(glm::dvec2(GetCornerPosition(Window::Position::Center)));
        } else {
            CaptureMouse(true);
        }
    }
    if (event.GetType() == EventType::ToggleVsync) {
        const auto* vsyncEvent = dynamic_cast<const ToggleVsyncEvent*>(&event);
        SetVsync(vsyncEvent->enable);
    }
}

glm::ivec2 Window::GetCornerPosition(const Position position) const {
    if (position == Position::Center) return glm::ivec2(m_Props->width / 2, m_Props->height / 2);
    if (position == Position::TopLeft) return glm::ivec2(0, 0);
    if (position == Position::TopRight) return glm::ivec2(m_Props->width, 0);
    if (position == Position::BottomLeft) return glm::vec2(0, m_Props->height);
    if (position == Position::BottomRight)
        return glm::ivec2(m_Props->width, m_Props->height);
    else {
        LOG_ERROR("Unkmown corner position");
        return glm::ivec2(0, 0);
    }
}

void Window::SetClearColor(const float r, const float g, const float b, const float a) { glClearColor(r, g, b, a); }

void Window::CaptureMouse(bool capture) {
    if (capture) {
        glfwSetInputMode(m_Handler, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_Handler, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::SetMousePosition(glm::dvec2 position) { glfwSetCursorPos(m_Handler, position.x, position.y); }

void Window::SetVsync(bool enable) {
    if (enable) {
        glfwSwapInterval(1);
        m_Props->vsync = true;
    } else {
        glfwSwapInterval(0);
        m_Props->vsync = false;
    }
}

std::unique_ptr<Window> Window::Create() { return std::make_unique<Window>(); }

/* Callbacks */
void Window::_error_callback(int error, const char* description) { LOG_ERROR("GLFW Error (\"{00}\"): {1}", error, description); }

void Window::_framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    WindowResizeEvent event(width, height);
    EventDispatcher::Get().Dispatch(event);
}

void Window::_key_callback(GLFWwindow* window, const int key, int scancode, const int action, int mods) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    KeyEvent* event = nullptr;
    switch (action) {
        case GLFW_PRESS:
            event = new KeyPressedEvent(key);
            EventDispatcher::Get().Dispatch(*event);
            break;
        case GLFW_REPEAT:
            event = new KeyPressedEvent(key, true);
            EventDispatcher::Get().Dispatch(*event);
            break;
        case GLFW_RELEASE:
            event = new KeyReleasedEvent(key);
            EventDispatcher::Get().Dispatch(*event);
            break;
        default:
            break;
    }
    if (event != nullptr) {
        delete event;
    }
}

void Window::_mouse_cursor_pos_callback(GLFWwindow* window, const double xpos, const double ypos) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    MouseMotionEvent event(xpos, ypos);
    EventDispatcher::Get().Dispatch(event);
}

void Window::_mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    MouseEvent* event = nullptr;
    switch (action) {
        case GLFW_PRESS:
            event = new MouseButtonPressedEvent(button);
            EventDispatcher::Get().Dispatch(*event);
            break;
        case GLFW_RELEASE:
            event = new MouseButtonReleasedEvent(button);
            EventDispatcher::Get().Dispatch(*event);
            break;
        default:
            break;
    }
    if (event != nullptr) {
        delete event;
    }
}

void Window::_window_pos_callback(GLFWwindow* window, const int xpos, const int ypos) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    WindowMovedEvent event(xpos, ypos);
    EventDispatcher::Get().Dispatch(event);
}
