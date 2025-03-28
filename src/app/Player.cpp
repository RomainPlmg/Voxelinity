#include "Player.h"

#include "core/Input.h"
#include "gfx/GraphicContext.h"
#include "utils/Time.h"

Player::Player() : Entity(glm::vec3(.7f, 1.7f, .7f), glm::vec3(.35f, 1.7f, .35f)) {}

void Player::Init() { m_Camera.Init(); }

void Player::Update() {
    // Project Front & Right vectors on the XZ plan
    glm::vec3 frontXZ = glm::normalize(glm::vec3(m_Camera.GetFrontVector().x, 0.0f, m_Camera.GetFrontVector().z));
    glm::vec3 rightXZ = glm::normalize(glm::vec3(m_Camera.GetRightVector().x, 0.0f, m_Camera.GetRightVector().z));

    float m_FlyingAcceleration = 2.0f;
    glm::vec3 accelerationDir(0.0f);
    float acc = m_FlyingAcceleration * static_cast<float>(Time::Get().GetDeltaTime());

    if (Input::IsKeyPressed(GLFW_KEY_W)) accelerationDir += frontXZ;
    if (Input::IsKeyPressed(GLFW_KEY_S)) accelerationDir -= frontXZ;
    if (Input::IsKeyPressed(GLFW_KEY_A)) accelerationDir -= rightXZ;
    if (Input::IsKeyPressed(GLFW_KEY_D)) accelerationDir += rightXZ;
    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) Move(glm::vec3(0.0f, 1.0f, 0.0f) * 15.0f * static_cast<float>(Time::Get().GetDeltaTime()));
    if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) Move(-glm::vec3(0.0f, 1.0f, 0.0f) * 15.0f * static_cast<float>(Time::Get().GetDeltaTime()));

    if (glm::length(accelerationDir) > 0.0f) {
        accelerationDir = glm::normalize(accelerationDir);
        m_Velocity += accelerationDir * acc;
    }

    if (glm::length(m_Velocity)) {
        m_Velocity -= m_Velocity * acc * 2.0f;
    }

    if (glm::length(m_Velocity) > PLAYER_MAX_SPEED) {
        m_Velocity = glm::normalize(m_Velocity) * static_cast<float>(PLAYER_MAX_SPEED);
    }

    Move(m_Velocity);

    m_Camera.SetPosition(m_Position);
    m_Camera.Update();
}
