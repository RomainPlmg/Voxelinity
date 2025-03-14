#include "Input.h"

#include "Window.h"
#include "gfx/GraphicContext.h"
#include "pch.h"
#include "utils/Logger.h"

GLFWwindow* Input::m_Handler = nullptr;

void Input::Init(const std::unique_ptr<Window>& window) {
    m_Handler = window->GetHandler();
    if (m_Handler == nullptr) {
        LOG_FATAL("Input: Window handler is null");
    }
}

bool Input::IsKeyPressed(const int keycode) { return glfwGetKey(m_Handler, keycode) == GLFW_PRESS; }

bool Input::IsMouseButtonPressed(const int button) { return glfwGetMouseButton(m_Handler, button) == GLFW_PRESS; }

glm::dvec2 Input::GetMousePosition() {
    glm::dvec2 pos;
    glfwGetCursorPos(m_Handler, &pos.x, &pos.y);
    return pos;
}
