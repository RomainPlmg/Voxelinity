#include "CollisionManager.h"

#include "Player.h"
#include "Voxel.h"
#include "World.h"
#include "pch.h"
#include "utils/Box.h"

CollisionManager::CollisionManager(World& world, Player& player) : m_Enable(true), m_World(world), m_Player(player) {}

void CollisionManager::Update() {
    if (!m_Enable) return;

    glm::vec3 playerPos = m_Player.GetPosition();
    auto entityBox = m_Player.GetBoundingBox();

    // Convert player position to voxel coordinates [3*3*3]
    int minX = std::floor(playerPos.x) - 1;
    int maxX = std::ceil(playerPos.x) + 1;
    int minY = std::floor(playerPos.y) - 1;
    int maxY = std::ceil(playerPos.y) + 1;
    int minZ = std::floor(playerPos.z) - 1;
    int maxZ = std::ceil(playerPos.z) + 1;

    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            for (int z = minZ; z < maxZ; z++) {
                Voxel* voxel = m_World.GetVoxel(glm::vec3(x, y, z));
                if (voxel && !voxel->IsTransparent()) {
                    Box other = voxel->GetBoundingBox();
                    if (entityBox.Intersects(other)) {
                        ResolveCollision(entityBox, other);
                    }
                }
            }
        }
    }
}

void CollisionManager::ResolveCollision(const Box& entityBox, const Box& other) {
    float RightPenetration = other.max.x - entityBox.min.x;
    float LeftPenetration = entityBox.max.x - other.min.x;
    float TopPenetration = other.max.y - entityBox.min.y;
    float BottomPenetration = entityBox.max.y - other.min.y;
    float FrontPenetration = other.max.z - entityBox.min.z;
    float BackPenetration = entityBox.max.z - other.min.z;

    float minPene = std::min({RightPenetration, LeftPenetration, TopPenetration, BottomPenetration, FrontPenetration, BackPenetration});

    if (minPene == RightPenetration)
        m_Player.Move(glm::vec3(RightPenetration, 0.0f, 0.0f));
    else if (minPene == LeftPenetration)
        m_Player.Move(glm::vec3(-LeftPenetration, 0.0f, 0.0f));
    else if (minPene == TopPenetration)
        m_Player.Move(glm::vec3(0.0f, TopPenetration, 0.0f));
    else if (minPene == BottomPenetration)
        m_Player.Move(glm::vec3(0.0f, -BottomPenetration, 0.0f));
    else if (minPene == FrontPenetration)
        m_Player.Move(glm::vec3(0.0f, 0.0f, FrontPenetration));
    else if (minPene == BackPenetration)
        m_Player.Move(glm::vec3(0.0f, 0.0f, -BackPenetration));
}
