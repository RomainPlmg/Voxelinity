#ifndef __WORLD_H__
#define __WORLD_H__

#include <FastNoiseLite.h>

#include <glm/gtx/hash.hpp>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "ChunkManager.h"
#include "CollisionManager.h"
#include "Player.h"

class Renderable;
class Event;
class Voxel;

constexpr float GRAVITY = 40.0f;

struct WorldStatus {
    glm::vec3 playerPos;

    WorldStatus() : playerPos(glm::vec3(0)) {}
};

class World {
   public:
    World();
    ~World();

    void Init();
    void Update();

    void OnEvent(const Event& event);

    /* Getters */
    const Player& GetPlayer() const { return m_Player; }
    Voxel* GetVoxel(const glm::vec3& pos) const;

    static const WorldStatus& GetStatus() { return m_Status; }
    static std::unique_ptr<World> Create();

   private:
    bool m_IsPaused;
    Player m_Player;
    ChunkManager m_ChunkManager;
    CollisionManager m_CollisionManager;
    glm::vec3 m_LastPlayerPos;

    static WorldStatus m_Status;
};

#endif  // __WORLD_H__