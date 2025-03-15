//
// Created by Romain on 22/12/2024.
//

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "GraphicContext.h"
#include "core/Input.h"
#include "core/Window.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "utils/Logger.h"
#include "utils/Time.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(Camera::OnEvent));
    m_Position = position;
    m_WorldUp = up;
    m_Yaw = yaw;
    m_Pitch = pitch;

    m_MouseSensitivity = CAMERA_SENSITIVITY;
    m_Zoom = CAMERA_ZOOM;

    UpdateCameraVectors();
}

Camera::Camera(const float posX, const float posY, const float posZ, const float upX, const float upY, const float upZ, const float yaw,
               const float pitch) {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(Camera::OnEvent));
    m_Position = glm::vec3(posX, posY, posZ);
    m_WorldUp = glm::vec3(upX, upY, upZ);
    m_Yaw = yaw;
    m_Pitch = pitch;

    m_MouseSensitivity = CAMERA_SENSITIVITY;
    m_Zoom = CAMERA_ZOOM;

    UpdateCameraVectors();
}

void Camera::Init() { m_LastMousePosition = Input::GetMousePosition(); }

void Camera::OnEvent(const Event& event) {
    if (event.GetType() == EventType::AppPause) {
        const auto* pauseEvent = dynamic_cast<const PauseEvent*>(&event);
        if (!pauseEvent->isPaused) Init();
    }

    if (event.GetType() == EventType::SetMouseSensitivity) {
        const auto* setMouseSensitivityEvent = dynamic_cast<const SetMouseSensitivityEvent*>(&event);
        m_MouseSensitivity = setMouseSensitivityEvent->value;
    }
}

glm::mat4 Camera::GetViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

glm::vec3 Camera::GetPosition() const { return m_Position; }

std::shared_ptr<Camera> Camera::Create(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    return std::make_shared<Camera>(position, up, yaw, pitch);
}

std::shared_ptr<Camera> Camera::Create(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) {
    return std::make_shared<Camera>(posX, posY, posZ, upX, upY, upZ, yaw, pitch);
}

void Camera::Update() {
    // Project m_Front & m_Right on the XZ plan
    glm::vec3 frontXZ = glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z));
    glm::vec3 rightXZ = glm::normalize(glm::vec3(m_Right.x, 0.0f, m_Right.z));

    double xOffset = m_LastMousePosition.x - Input::GetMousePosition().x;
    double yOffset = m_LastMousePosition.y - Input::GetMousePosition().y;

    xOffset *= m_MouseSensitivity;
    yOffset *= m_MouseSensitivity;

    m_Yaw -= xOffset;
    m_Pitch += yOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();

    m_LastMousePosition.x = Input::GetMousePosition().x;
    m_LastMousePosition.y = Input::GetMousePosition().y;
}

void Camera::UpdateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front(0.0f);
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you
                                                               // Look up or down which results in slower movement.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
