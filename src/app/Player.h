#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <glm/glm.hpp>
#include <memory>

#include "Entity.h"
#include "gfx/Camera.h"

constexpr int PLAYER_MAX_SPEED = 10.0f;

class Event;

class Player : public Entity {
   public:
    Player();

    void Init();
    void Update();
    void Move(const glm::vec3& delta) override;

    /* Getters */
    const Camera& GetCamera() const { return m_Camera; }
    Camera& GetCamera() { return m_Camera; }

   private:
    Camera m_Camera;
};

#endif  // __PLAYER_H__