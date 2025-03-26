#ifndef __COLLISION_MANAGER_H__
#define __COLLISION_MANAGER_H__

#include "utils/Box.h"

class Player;
class World;

class CollisionManager {
   public:
    CollisionManager(World& world, Player& player);

    void Update();
    void Enable() { m_Enable = true; }
    void Disable() { m_Enable = false; }

   private:
    bool m_Enable;
    World& m_World;
    Player& m_Player;

    void ResolveCollision(const Box& entityBox, const Box& other);
};

#endif  // __COLLISION_MANAGER_H__