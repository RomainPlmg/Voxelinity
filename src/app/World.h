#ifndef __WORLD_H__
#define __WORLD_H__

#include <FastNoiseLite.h>

#include <glm/gtx/hash.hpp>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "ChunkManager.h"
#include "Player.h"

class Renderable;
class Event;

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

    static const WorldStatus& GetStatus() { return m_Status; }
    static std::unique_ptr<World> Create();

   private:
    bool m_IsPaused;
    Player m_Player;
    ChunkManager m_ChunkManager;

    static WorldStatus m_Status;
};

#endif  // __WORLD_H__