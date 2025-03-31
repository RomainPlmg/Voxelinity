#ifndef __COLLISION_MANAGER_H__
#define __COLLISION_MANAGER_H__

#include "utils/Box.h"

class Entity;
class Box;
class World;

class CollisionManager {
   public:
    CollisionManager(const World& world);

    void Update();
    void Enable() { m_Enable = true; }
    void Disable() { m_Enable = false; }

   private:
    bool m_Enable;
    const World& m_World;

    void ResolveCollision(Entity& entity, const Box& other);
};

#endif  // __COLLISION_MANAGER_H__