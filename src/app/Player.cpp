#include "Player.h"

#include "core/Input.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/GraphicContext.h"
#include "utils/Logger.h"
#include "utils/Time.h"
#include "World.h"

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

        float acc = 4.0f;

        // Acceleration
        if (glm::length(accelerationDir) > 0.0f) {
            accelerationDir = glm::normalize(accelerationDir);
            m_Velocity.x += accelerationDir.x * acc * static_cast<float>(Time::Get().GetDeltaTime());
            m_Velocity.z += accelerationDir.z * acc * static_cast<float>(Time::Get().GetDeltaTime());
        }

        // Friction
        if (glm::length(m_Velocity)) {
            m_Velocity.x -= m_Velocity.x * acc * static_cast<float>(Time::Get().GetDeltaTime());
            m_Velocity.z -= m_Velocity.z * acc * static_cast<float>(Time::Get().GetDeltaTime());
        }

        // Speed limit
        float maxSpeed = static_cast<float>(PLAYER_MAX_SPEED) * static_cast<float>(Time::Get().GetDeltaTime());
        if (glm::length(m_Velocity) > maxSpeed) {
            m_Velocity = glm::normalize(m_Velocity) * maxSpeed;
        }

        Move(m_Velocity);

        m_Velocity.y = 0.0f;
    } else {
        glm::vec3 position = m_Position;
        float speed = static_cast<float>(PLAYER_MAX_SPEED) / 2;
        if (glm::length(accelerationDir) > 0.0f) {
            accelerationDir = glm::normalize(accelerationDir);
            position.x += accelerationDir.x * speed * static_cast<float>(Time::Get().GetDeltaTime());
            position.z += accelerationDir.z * speed * static_cast<float>(Time::Get().GetDeltaTime());
        }

        if (Input::IsKeyPressed(GLFW_KEY_SPACE) && m_Grounded) {
            m_Velocity.y = PLAYER_JUMP_STRENGTH;
            m_Grounded = false;
        }

        if (!m_Grounded) {
            m_Velocity.y -= GRAVITY * static_cast<float>(Time::Get().GetDeltaTime());
        }

        position.y += m_Velocity.y * static_cast<float>(Time::Get().GetDeltaTime());

        SetPosition(position);

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

void Player::SetPosition(const glm::vec3& position) {
    glm::vec3 lastposition = m_Position;
    Entity::SetPosition(position);
    m_BoundingBox.Move(m_Position - lastposition);
    glm::vec3 camPos = m_Position;
    camPos.y = m_Position.y + 1.9f;
    m_Camera.SetPosition(camPos);
}
