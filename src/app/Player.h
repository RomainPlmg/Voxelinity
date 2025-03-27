#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <glm/glm.hpp>
#include <memory>

#include "gfx/Camera.h"
#include "utils/Box.h"

constexpr int PLAYER_MAX_SPEED = 10.0f;

class Player {
   public:
    Player();

    void Init();
    void Update();
    void Move(glm::vec3 delta);

    /* Getters */
    const Camera& GetCamera() const { return m_Camera; }
    const glm::vec3 GetPosition() const { return m_Position; }
    const Box& GetBoundingBox() const { return m_BoundingBox; }
    const glm::vec3& GetVelocityVector() const { return m_FyingVelocity; }

    /* Setters */
    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetVelocity(const glm::vec3& velocity) { m_FyingVelocity = velocity; }

   private:
    glm::vec3 m_Position;
    glm::vec3 m_FyingVelocity;
    float m_FlyingAcceleration;
    Box m_BoundingBox;

    Camera m_Camera;
};

#endif  // __PLAYER_H__