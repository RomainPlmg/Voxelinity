#include "Player.h"

#include "core/Input.h"
#include "gfx/GraphicContext.h"
#include "utils/Time.h"

Player::Player() : m_Position(glm::vec3(0)), m_FyingVelocity(0.0f), m_FlyingAcceleration(5.0f) {}

void Player::Init() { m_Camera.Init(); }

void Player::Update() {
    // Project Front & Right vectors on the XZ plan
    glm::vec3 frontXZ = glm::normalize(glm::vec3(m_Camera.GetFrontVector().x, 0.0f, m_Camera.GetFrontVector().z));
    glm::vec3 rightXZ = glm::normalize(glm::vec3(m_Camera.GetRightVector().x, 0.0f, m_Camera.GetRightVector().z));

    glm::vec3 accelerationDir(0.0f);
    float acc = m_FlyingAcceleration * static_cast<float>(Time::Get().GetDeltaTime());

    if (Input::IsKeyPressed(GLFW_KEY_W)) accelerationDir += frontXZ;
    if (Input::IsKeyPressed(GLFW_KEY_S)) accelerationDir -= frontXZ;
    if (Input::IsKeyPressed(GLFW_KEY_A)) accelerationDir -= rightXZ;
    if (Input::IsKeyPressed(GLFW_KEY_D)) accelerationDir += rightXZ;
    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) m_Position += glm::vec3(0.0f, 1.0f, 0.0f) * 10.0f * static_cast<float>(Time::Get().GetDeltaTime());
    if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) m_Position -= glm::vec3(0.0f, 1.0f, 0.0f) * 10.0f * static_cast<float>(Time::Get().GetDeltaTime());

    if (glm::length(accelerationDir) > 0.0f) {
        accelerationDir = glm::normalize(accelerationDir);
        m_FyingVelocity += accelerationDir * acc;
    }

    if (glm::length(m_FyingVelocity)) {
        m_FyingVelocity -= m_FyingVelocity * acc;
    }

    if (glm::length(m_FyingVelocity) > PLAYER_MAX_SPEED) {
        m_FyingVelocity = glm::normalize(m_FyingVelocity) * static_cast<float>(PLAYER_MAX_SPEED);
    }

    Move(m_FyingVelocity);

    m_Camera.SetPosition(m_Position);
    m_Camera.Update();
}

void Player::Move(glm::vec3 delta) { m_Position += delta; }
