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
constexpr int NB_VOXELS_IN_CHUNK = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;
constexpr int NB_BOUNDARY_VOXELS_IN_CHUNK =
    2 * CHUNK_WIDTH * CHUNK_HEIGHT + 2 * (CHUNK_WIDTH - 2) * CHUNK_HEIGHT + 2 * (CHUNK_WIDTH - 2) * (CHUNK_WIDTH - 2);

class Chunk : public Renderable {
   public:
    Chunk(glm::ivec3 position);

    void GenerateData(const FastNoiseLite& noise);
    void GenerateMesh();
    void Unload();
    void Update() override;

    void OnEvent(const Event& event);

    /* Getters */
    const bool IsDataGenerated() const { return m_DataGenerated; }
    const bool IsMeshGenerated() const { return m_MeshGenerated; }
    Voxel* GetVoxelatCoord(const glm::vec3& coord);
    std::array<Voxel*, NB_BOUNDARY_VOXELS_IN_CHUNK>& GetBoundaryVoxels() { return m_BoundaryVoxels; }

   private:
    std::atomic<bool> m_DataGenerated;
    std::atomic<bool> m_MeshGenerated;
    std::array<Voxel*, CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT> m_Voxels = {nullptr};
    std::array<Voxel*, NB_BOUNDARY_VOXELS_IN_CHUNK> m_BoundaryVoxels;

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
};

#endif  // __CHUNK_H__