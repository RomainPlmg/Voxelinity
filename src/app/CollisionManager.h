#ifndef __COLLISION_MANAGER_H__
#define __COLLISION_MANAGER_H__

class Player;
class World;

class CollisionManager {
   public:
    CollisionManager(Player& player, World& world);

    void Update();
    void Enable() { m_Enable = true; }
    void Disable() { m_Enable = false; }

   private:
    bool m_Enable;

    Player& m_Player;
    World& m_World;
};

#endif  // __COLLISION_MANAGER_H__