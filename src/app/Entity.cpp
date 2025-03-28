#include "Entity.h"

Entity::Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet) : m_Position(glm::vec3(0.0f)), m_Velocity(glm::vec3(0.0f)), m_BoundingBox(m_Position - boxoffet, boxsize) {}

Entity::Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet, const glm::vec3& position)
    : m_Position(position), m_Velocity(glm::vec3(0.0f)), m_BoundingBox(m_Position - boxoffet, boxsize) {
}

void Entity::Move(glm::vec3 delta) {
    m_Position += delta;
    m_BoundingBox.Move(delta);
}
