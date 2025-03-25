#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <glm/glm.hpp>
#include <memory>

#include "gfx/Camera.h"

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

    /* Setters */
    void SetPosition(glm::vec3 position) { m_Position = position; }

   private:
    glm::vec3 m_Position;
    glm::vec3 m_FyingVelocity;
    float m_FlyingAcceleration;

    Camera m_Camera;
};

#endif  // __PLAYER_H__