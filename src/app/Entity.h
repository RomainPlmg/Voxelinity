#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <glm/glm.hpp>

#include "utils/Box.h"

class Entity {
   public:
    Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet);
    Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet, const glm::vec3& position);

    virtual ~Entity() = default;
    virtual void Update() = 0;
    void Move(glm::vec3 delta);

    /* Getters */
    Box GetBoundingBox() const { return m_BoundingBox; }
    glm::vec3 GetVelocityVector() const { return m_Velocity; }
    glm::vec3 GetPosition() const { return m_Position; }

    /* Setters */
    void SetBoundingBox(const Box& box) { m_BoundingBox = box; }
    void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; }
    void SetPosition(const glm::vec3& position) { m_Position = position; }

   protected:
    Box m_BoundingBox;
    glm::vec3 m_Velocity;
    glm::vec3 m_Position;
};

#endif  // __ENTITY_H__