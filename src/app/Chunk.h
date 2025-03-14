#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <FastNoiseLite.h>

#include <array>
#include <atomic>
#include <functional>
#include <glm/glm.hpp>

#include "Voxel.h"
#include "gfx/Renderable.h"

class Event;

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 32;

class Chunk : public Renderable {
   public:
    Chunk(glm::ivec3 position);

    void Load(const FastNoiseLite& noise);
    void Unload();
    void Update() override;

    void OnEvent(const Event& event);

    /* Getters */
    const bool IsGenerated() const { return m_Generated; }

   private:
    std::atomic<bool> m_Generated;
    std::array<Voxel*, CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT> m_Voxels = {nullptr};

    // Uniforms for shader
    bool m_UniformToggleWireframe = false;
    glm::vec3 m_UniformWireframeColor = glm::vec3(0.0f);
    int m_UniformFogStart = 0;
    int m_UniformFogEnd = 0;
    glm::vec3 m_UniformGofColor = glm::vec3(0.0f);

    static const std::unordered_map<Voxel::Face, std::array<float, 16>> m_VoxelVertices;  // A map of arrays for cube vertices

    // Private methods
    void RemoveInternalFaces();
    glm::vec3 GetVoxelCoord(int index);
    int GetVoxelIndex(const glm::vec3& coord);
    Voxel* GetVoxelatCoord(const glm::vec3& coord);
};

#endif  // __CHUNK_H__