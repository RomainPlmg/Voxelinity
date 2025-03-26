#include "Player.h"

#include "core/Input.h"
#include "gfx/GraphicContext.h"
#include "utils/Time.h"

Player::Player() : m_Position(glm::vec3(0)), m_FyingVelocity(0.0f), m_FlyingAcceleration(5.0f) {
    m_BoundingBox.Resize(glm::vec3(0.5f, 1.7f, 0.5f));
    m_BoundingBox.Move(glm::vec3(-(m_BoundingBox.Xmax - m_BoundingBox.Xmin) / 2.0f, 0.0f, -(m_BoundingBox.Zmax - m_BoundingBox.Zmin) / 2.0f));
    m_BoundingBox.Move(glm::vec3(m_Position.x, m_Position.y, m_Position.z));
}

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
    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) m_FyingVelocity = glm::vec3(0.0f, 1.0f, 0.0f) * 30.0f * static_cast<float>(Time::Get().GetDeltaTime());
    if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        m_FyingVelocity = -glm::vec3(0.0f, 1.0f, 0.0f) * 30.0f * static_cast<float>(Time::Get().GetDeltaTime());

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

    m_Camera.Update();
}

void Player::Move(glm::vec3 delta) {
    glm::vec3 lastPosition = m_Position;
    m_Position += delta;
    m_BoundingBox.Move(m_Position - lastPosition);
    m_Camera.SetPosition(m_Position);
}
