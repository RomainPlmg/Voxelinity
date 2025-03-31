#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>

#include "utils/Box.h"

class Entity {
   public:
    Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet);
    Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet, const glm::vec3& position);
    virtual ~Entity();

    virtual void Update() = 0;
    virtual void Move(const glm::vec3& delta);

    /* Getters */
    Box GetBoundingBox() const { return m_BoundingBox; }
    glm::vec3 GetVelocityVector() const { return m_Velocity; }
    glm::vec3 GetPosition() const { return m_Position; }
    bool Grounded() const { return m_Grounded; }

    static std::unordered_set<Entity*> GetEntities() { return m_Entities; }

    /* Setters */
    void SetBoundingBox(const Box& box) { m_BoundingBox = box; }
    void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; }
    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetGrounded(bool grounded) { m_Grounded = grounded; }

   protected:
    Box m_BoundingBox;
    glm::vec3 m_Velocity;
    glm::vec3 m_Position;
    bool m_Grounded;

   private:
    static std::unordered_set<Entity*> m_Entities;
};

#endif  // __ENTITY_H__