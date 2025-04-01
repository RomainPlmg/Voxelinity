#include "Player.h"

#include "core/Input.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/GraphicContext.h"
#include "utils/Logger.h"
#include "utils/Time.h"

Player::Player() : Entity(glm::vec3(.7f, 1.9f, .7f), glm::vec3(.35f, 0.0f, .35f)), m_GodMode(false) {}

void Player::Init() {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryAll, BIND_EVENT_FN(Player::OnEvent));
    m_Camera.Init();
}

void Player::Update() {
    // Project Front & Right vectors on the XZ plan
    glm::vec3 frontXZ = glm::normalize(glm::vec3(m_Camera.GetFrontVector().x, 0.0f, m_Camera.GetFrontVector().z));
    glm::vec3 rightXZ = glm::normalize(glm::vec3(m_Camera.GetRightVector().x, 0.0f, m_Camera.GetRightVector().z));

    glm::vec3 accelerationDir(0.0f);

    if (Input::IsKeyPressed(GLFW_KEY_W)) accelerationDir += frontXZ;
    if (Input::IsKeyPressed(GLFW_KEY_S)) accelerationDir -= frontXZ;
    if (Input::IsKeyPressed(GLFW_KEY_A)) accelerationDir -= rightXZ;
    if (Input::IsKeyPressed(GLFW_KEY_D)) accelerationDir += rightXZ;

    if (m_GodMode) {
        if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
            m_Velocity.y = 15.0f * static_cast<float>(Time::Get().GetDeltaTime());
            m_Grounded = false;
        }
        if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) m_Velocity.y = !m_Grounded ? -15.0f * static_cast<float>(Time::Get().GetDeltaTime()) : 0.0f;

        float acc = 2.0f * static_cast<float>(Time::Get().GetDeltaTime());

        if (glm::length(accelerationDir) > 0.0f) {
            accelerationDir = glm::normalize(accelerationDir);
            m_Velocity.x += accelerationDir.x * acc;
            m_Velocity.z += accelerationDir.z * acc;
        }

        if (glm::length(m_Velocity)) {
            m_Velocity.x -= m_Velocity.x * acc * 2.0f;
            m_Velocity.z -= m_Velocity.z * acc * 2.0f;
        }

        if (glm::length(m_Velocity) > PLAYER_MAX_SPEED) {
            m_Velocity = glm::normalize(m_Velocity) * static_cast<float>(PLAYER_MAX_SPEED);
        }

        Move(m_Velocity);

        m_Velocity.y = 0.0f;
    } else {
        if (glm::length(accelerationDir) > 0.0f) {
            accelerationDir = glm::normalize(accelerationDir);
            m_Velocity.x = accelerationDir.x * 10.0f * static_cast<float>(Time::Get().GetDeltaTime());
            m_Velocity.z = accelerationDir.z * 10.0f * static_cast<float>(Time::Get().GetDeltaTime());
        }

        if (Input::IsKeyPressed(GLFW_KEY_SPACE) && m_Grounded) {
            m_Velocity.y = 0.25f;
            m_Grounded = false;
        }

        if (!m_Grounded) {
            m_Velocity.y -= 0.8f * static_cast<float>(Time::Get().GetDeltaTime());
        }

        Move(m_Velocity);

        m_Velocity.x = 0.0f;
        m_Velocity.z = 0.0f;
    }
}

void Player::Move(const glm::vec3& delta) {
    Entity::Move(delta);
    glm::vec3 camPos = m_Position;
    camPos.y = m_Position.y + 1.9f;
    m_Camera.SetPosition(camPos);
}

void Player::OnEvent(const Event& event) {
    if (event.GetType() == EventType::GodMode) {
        const auto* godEvent = dynamic_cast<const SetGodModeEvent*>(&event);
        m_GodMode = godEvent->god;
    }
}
