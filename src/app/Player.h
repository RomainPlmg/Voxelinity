#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <glm/glm.hpp>
#include <memory>

#include "gfx/Camera.h"
#include "Entity.h"

constexpr int PLAYER_MAX_SPEED = 10.0f;

class Player : public Entity {
   public:
    Player();

    void Init();
    void Update();

    /* Getters */
    const Camera& GetCamera() const { return m_Camera; }

   private:
    Camera m_Camera;
};

#endif  // __PLAYER_H__