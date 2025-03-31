#include "Entity.h"

#include "utils/Time.h"

std::unordered_set<Entity*> Entity::m_Entities;

Entity::Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet)
    : m_Position(glm::vec3(0.0f)), m_Velocity(glm::vec3(0.0f)), m_BoundingBox(glm::vec3(0.0f) - boxoffet, boxsize), m_Grounded(false) {
    m_Entities.insert(this);
}

Entity::Entity(const glm::vec3& boxsize, const glm::vec3& boxoffet, const glm::vec3& position)
    : m_Position(position), m_Velocity(glm::vec3(0.0f)), m_BoundingBox(position - boxoffet, boxsize), m_Grounded(false) {
    m_Entities.insert(this);
}

Entity::~Entity() { m_Entities.erase(this); }

void Entity::Move(const glm::vec3& delta) {
    m_Position += delta;
    m_BoundingBox.Move(delta);
}
