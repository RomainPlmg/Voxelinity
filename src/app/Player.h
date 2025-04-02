#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <glm/glm.hpp>
#include <memory>

#include "Entity.h"
#include "gfx/Camera.h"

constexpr int PLAYER_MAX_SPEED = 30.0f;
constexpr int PLAYER_JUMP_STRENGTH = 10.0f;

class Event;

class Player : public Entity {
   public:
    Player();

    void Init();
    void Update();
    void Move(const glm::vec3& delta) override;

    void OnEvent(const Event& event);

    /* Setters */
    void SetPosition(const glm::vec3& position) override;

    /* Getters */
    const Camera& GetCamera() const { return m_Camera; }
    Camera& GetCamera() { return m_Camera; }

   private:
    bool m_GodMode;
    Camera m_Camera;
};

#endif  // __PLAYER_H__