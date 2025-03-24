#ifndef __CHUNK_MANAGER_H__
#define __CHUNK_MANAGER_H__

#include <FastNoiseLite.h>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

class Event;
class Chunk;

class ChunkManager {
   public:
    ChunkManager();
    ~ChunkManager();
    void Init();
    void Update();

    std::array<std::shared_ptr<Chunk>, 4> GetNeighbors(glm::ivec3 pos);

    void OnEvent(const Event& event);

    /* Setters */
    void SetRenderDistance(int distance) { m_RenderDistance = distance; }

   private:
    int m_RenderDistance;
    FastNoiseLite m_Noise;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_Chunks;  // For direct access

    int m_NbChunksWithData;
    std::queue<Chunk*> m_ChunksToMesh;
    std::queue<Chunk*> m_ChunksToRender;
};

#endif  // __CHUNK_MANAGER_H__