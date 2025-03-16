#ifndef __WORLD_H__
#define __WORLD_H__

#include <FastNoiseLite.h>

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "Player.h"

class Renderable;
class Chunk;
class Event;

struct WorldStatus {
    glm::vec3 playerPos;

    WorldStatus() : playerPos(glm::vec3(0)) {}
};

struct Vec3Hash {
    std::size_t operator()(const glm::vec3& v) const noexcept {
        std::hash<float> hasher;
        std::size_t h1 = hasher(v.x);
        std::size_t h2 = hasher(v.y);
        std::size_t h3 = hasher(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class World {
   public:
    World();
    ~World();

    void Init();
    void Update();

    void OnEvent(const Event& event);

    /* Getters */
    const std::unordered_set<Renderable*>& GetRenderables() const { return m_Renderables; }
    const Player& GetPlayer() const { return m_Player; }
    const bool AllChunksGenerated() const;
    const Chunk* GetChunkAtPosition(glm::vec3 position);

    static const WorldStatus& GetStatus() { return m_Status; }
    static std::unique_ptr<World> Create();

   private:
    bool m_IsPaused;
    int m_RenderDistance;
    std::unordered_set<Renderable*> m_Renderables;
    Player m_Player;
    FastNoiseLite m_Noise;

    std::unordered_map<glm::vec3, std::shared_ptr<Chunk>, Vec3Hash> m_Chunks;
    std::shared_mutex m_ChunkMutex;

    static WorldStatus m_Status;

    void RemoveBoundaryFaces(Chunk& chunk);
};

#endif  // __WORLD_H__