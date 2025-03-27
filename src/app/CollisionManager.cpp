#include "CollisionManager.h"

#include "Player.h"
#include "Voxel.h"
#include "World.h"
#include "pch.h"
#include "utils/Box.h"
#include "utils/Logger.h"

CollisionManager::CollisionManager(World& world, Player& player) : m_Enable(true), m_World(world), m_Player(player) {}

void CollisionManager::Update() {
    if (!m_Enable) return;

    glm::vec3 playerPos = m_Player.GetPosition();
    auto& entityBox = m_Player.GetBoundingBox();

    // Convert player position to voxel coordinates [3*3*3]
    int minX = std::floor(playerPos.x) - 1;
    int maxX = std::ceil(playerPos.x) + 1;
    int minY = std::floor(playerPos.y) - 1;
    int maxY = std::ceil(playerPos.y) + 1;
    int minZ = std::floor(playerPos.z) - 1;
    int maxZ = std::ceil(playerPos.z) + 1;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                Voxel* voxel = m_World.GetVoxel(glm::vec3(x, y, z));
                if (voxel && !voxel->IsTransparent()) {
                    Box other = voxel->GetBoundingBox();
                    if (entityBox.Intersects(other)) {
                        ResolveCollision(entityBox, other);
                        LOG_TRACE("COLLISION");
                    }
                }
            }
        }
    }
}

void CollisionManager::ResolveCollision(const Box& entityBox, const Box& other) {}
