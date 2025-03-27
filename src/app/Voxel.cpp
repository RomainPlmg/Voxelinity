#include "Voxel.h"

#include "gfx/Buffer.h"
#include "gfx/GraphicContext.h"
#include "gfx/Shader.h"
#include "gfx/VertexArray.h"
#include "pch.h"

Voxel::Voxel(glm::vec3 position)
    : m_Position(position), m_IsTransparent(false), m_FaceToRender(0b00111111), m_BoundingBox(glm::vec3(0.0f), glm::vec3(1.0f)) {}

void Voxel::Update() {}

bool Voxel::NeedToRenderFace(const Face& face) { return (1 << face) & m_FaceToRender; }

void Voxel::SetTransparent(bool transparent) { m_IsTransparent = transparent; }

void Voxel::SetFaceVisible(const Face face) { m_FaceToRender |= 1 << face; }

void Voxel::SetFaceInvisible(const Face face) { m_FaceToRender &= ~(1 << face); }

void Voxel::SetBoundingBox(const Box& box) { m_BoundingBox = box; }
