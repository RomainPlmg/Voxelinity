#ifndef __VOXEL_H__
#define __VOXEL_H__

#include <array>
#include <glm/glm.hpp>
#include <unordered_map>

#include "utils/Box.h"

class Voxel {
   public:
    enum Face { Front = 0, Back, Left, Right, Top, Bottom };

    Voxel(glm::vec3 position);

    void Update();

    /* Getters */
    glm::vec3 GetPosition() const { return m_Position; }
    bool IsTransparent() const { return m_IsTransparent; }
    bool NeedToRenderFace(const Face& face);
    const Box& GetBoundingBox() const { return m_BoundingBox; }

    /* Setters */
    void SetTransparent(bool transparent);
    void SetFaceVisible(const Face face);
    void SetFaceInvisible(const Face face);
    void SetBoundingBox(const Box& box);

   private:
    bool m_IsTransparent;
    glm::vec3 m_Position;
    char m_FaceToRender;
    Box m_BoundingBox;
};

#endif  // __VOXEL_H__