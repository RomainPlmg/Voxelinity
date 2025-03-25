#include "Voxel.h"

#include "gfx/Buffer.h"
#include "gfx/GraphicContext.h"
#include "gfx/Shader.h"
#include "gfx/VertexArray.h"
#include "pch.h"

Voxel::Voxel(glm::vec3 position) : m_Position(position), m_IsTransparent(false), m_FaceToRender(0b00111111) {
    m_BoundingBox.Resize(glm::vec3(1.0f, 1.0f, 1.0f));
    m_BoundingBox.Move(glm::vec3(m_Position.x + 0.5f, m_Position.y + 0.5f, m_Position.z + 0.5f));
}

void Voxel::Update() {}

bool Voxel::NeedToRenderFace(const Face& face) { return (1 << face) & m_FaceToRender; }

void Voxel::SetTransparent(bool transparent) { m_IsTransparent = transparent; }

void Voxel::SetFaceVisible(const Face face) { m_FaceToRender |= 1 << face; }

void Voxel::SetFaceInvisible(const Face face) { m_FaceToRender &= ~(1 << face); }
