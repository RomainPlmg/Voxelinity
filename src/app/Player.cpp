#include "Player.h"

#include "core/Input.h"
#include "gfx/GraphicContext.h"
#include "utils/Time.h"

Player::Player() : m_Position(glm::vec3(0)), m_Velocity(10.0f) {}

void Player::Init() { m_Camera.Init(); }

void Player::Update() {
    // Project Front & Right vectors on the XZ plan
    glm::vec3 frontXZ = glm::normalize(glm::vec3(m_Camera.GetFrontVector().x, 0.0f, m_Camera.GetFrontVector().z));
    glm::vec3 rightXZ = glm::normalize(glm::vec3(m_Camera.GetRightVector().x, 0.0f, m_Camera.GetRightVector().z));

    float velocity = m_Velocity * Time::Get().GetDeltaTime();

    if (Input::IsKeyPressed(GLFW_KEY_W)) m_Position += frontXZ * velocity;
    if (Input::IsKeyPressed(GLFW_KEY_S)) m_Position -= frontXZ * velocity;
    if (Input::IsKeyPressed(GLFW_KEY_A)) m_Position -= rightXZ * velocity;
    if (Input::IsKeyPressed(GLFW_KEY_D)) m_Position += rightXZ * velocity;
    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) m_Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
    if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) m_Position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;

    m_Camera.SetPosition(m_Position);
    m_Camera.Update();
}

void Player::Move(glm::vec3 delta) { m_Position += delta; }
