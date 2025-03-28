#ifndef __BOX_H__
#define __BOX_H__

#include <glm/glm.hpp>

struct Box {
    glm::vec3 min;  // Lower left corner
    glm::vec3 max;  // Upper right corner

    Box(const glm::vec3& position, const glm::vec3& size) : min(position), max(position + size) {}

    // Update box position
    void Move(const glm::vec3& delta) {
        min += delta;
        max += delta;
    }

    // Checks if this box collides with another box
    bool Intersects(const Box& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) && (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

#endif  // __BOX_H__