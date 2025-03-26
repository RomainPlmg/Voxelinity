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
                    if (entityBox.IsColliding(other)) {
                        ResolveCollision(entityBox, other);
                        LOG_TRACE("COLLISION");
                    }
                }
            }
        }
    }
}

void CollisionManager::ResolveCollision(const Box& entityBox, const Box& other) {
    glm::vec3 delta(0.0f);

    // X axis
    if (entityBox.Xmax >= other.Xmin) {  // Too far to the right
        delta.x = -(entityBox.Xmax - other.Xmin);
    } else if (entityBox.Xmin <= other.Xmax) {  // Too far to the left
        delta.x = other.Xmax - entityBox.Xmin;
    }

    // Y axis
    if (entityBox.Ymax >= other.Ymin) {  // Too far to the top
        delta.y = -(entityBox.Ymax - other.Ymin);
    } else if (entityBox.Ymin <= other.Ymax) {  // Too far to the bottom
        delta.y = other.Ymax - entityBox.Ymin;
    }

    // Z axis
    if (entityBox.Zmax >= other.Zmin) {  // Too far to the front
        delta.z = -(entityBox.Zmax - other.Zmin);
    } else if (entityBox.Zmin <= other.Zmax) {  // Too far to the back
        delta.z = other.Zmax - entityBox.Zmin;
    }

    m_Player.Move(delta);
}
