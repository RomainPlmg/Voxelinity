#ifndef __WORLD_H__
#define __WORLD_H__

#include <FastNoiseLite.h>

#include <memory>
#include <unordered_set>
#include <vector>

#include "Player.h"

class Renderable;
class Chunk;
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
    const std::unordered_set<Renderable*>& GetRenderables() const { return m_Renderables; }
    const Player& GetPlayer() const { return m_Player; }

    static const WorldStatus& GetStatus() { return m_Status; }
    static std::unique_ptr<World> Create();

   private:
    bool m_IsPaused;
    int m_RenderDistance;
    std::unordered_set<Renderable*> m_Renderables;
    Player m_Player;
    FastNoiseLite m_Noise;

    std::vector<Chunk*> m_Chunks;

    static WorldStatus m_Status;
};

#endif  // __WORLD_H__