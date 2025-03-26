#ifndef __BOX_H__
#define __BOX_H__

#include <glm/glm.hpp>

struct Box {
    float Xmin = 0.0f, Xmax = 0.0f;
    float Ymin = 0.0f, Ymax = 0.0f;
    float Zmin = 0.0f, Zmax = 0.0f;

    void Resize(const glm::vec3& scale) {
        Xmax = scale.x;
        Ymax = scale.y;
        Zmax = scale.z;
    }

    void Move(const glm::vec3& delta) {
        Xmin += delta.x;
        Xmax += delta.x;
        Ymin += delta.y;
        Ymax += delta.y;
        Zmin += delta.z;
        Zmax += delta.z;
    }

    // Test AABB collision
    bool IsColliding(const Box& collider) const {
        return Xmin < collider.Xmax && Xmax > collider.Xmin && Ymin < collider.Ymax && Ymax > collider.Ymin && Zmin < collider.Zmax &&
               Zmax > collider.Zmin;
    }
};

#endif  // __BOX_H__