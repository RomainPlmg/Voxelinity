#include "CollisionManager.h"

#include "Entity.h"
#include "Voxel.h"
#include "World.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "pch.h"
#include "utils/Box.h"

CollisionManager::CollisionManager(const World& world) : m_Enable(true), m_World(world) {}

void CollisionManager::Update() {
    if (!m_Enable) return;
    for (auto& entity : Entity::GetEntities()) {
        auto entityPos = entity->GetPosition();
        auto entityBox = entity->GetBoundingBox();

        // Convert player position to voxel coordinates [3*3*3]
        int minX = std::floor(entityPos.x) - 2;
        int maxX = std::ceil(entityPos.x) + 2;
        int minY = std::floor(entityPos.y) - 2;
        int maxY = std::ceil(entityPos.y) + 2;
        int minZ = std::floor(entityPos.z) - 2;
        int maxZ = std::ceil(entityPos.z) + 2;

        entity->SetGrounded(false);

        for (int x = minX; x < maxX; x++) {
            for (int y = minY; y < maxY; y++) {
                for (int z = minZ; z < maxZ; z++) {
                    Voxel* voxel = m_World.GetVoxel(glm::vec3(x, y, z));
                    if (voxel && !voxel->IsTransparent()) {
                        Box other = voxel->GetBoundingBox();
                        if (entityBox.Intersects(other)) {
                            ResolveCollision(*entity, other);
                        }
                    }
                }
            }
        }
    }
}

void CollisionManager::ResolveCollision(Entity& entity, const Box& other) {
    const auto& entityBox = entity.GetBoundingBox();
    glm::vec3 velocity = entity.GetVelocityVector();

    float RightPenetration = other.max.x - entityBox.min.x;
    float LeftPenetration = entityBox.max.x - other.min.x;
    float TopPenetration = other.max.y - entityBox.min.y;
    float BottomPenetration = entityBox.max.y - other.min.y;
    float FrontPenetration = other.max.z - entityBox.min.z;
    float BackPenetration = entityBox.max.z - other.min.z;

    float minPene = std::min({RightPenetration, LeftPenetration, TopPenetration, BottomPenetration, FrontPenetration, BackPenetration});

    glm::vec3 correction(0.0f);

    if (minPene == RightPenetration) {
        correction.x = RightPenetration;
        velocity.x = 0.0f;
    } else if (minPene == LeftPenetration) {
        correction.x = -LeftPenetration;
        velocity.x = 0.0f;
    } else if (minPene == TopPenetration) {
        correction.y = TopPenetration;
        velocity.y = 0.0f;
        entity.SetGrounded(true);
    } else if (minPene == BottomPenetration) {
        correction.y = -BottomPenetration;
        velocity.y = 0.0f;
    } else if (minPene == FrontPenetration) {
        correction.z = FrontPenetration;
        velocity.z = 0.0f;
    } else if (minPene == BackPenetration) {
        correction.z = -BackPenetration;
        velocity.z = 0.0f;
    }

    entity.Move(correction);
    entity.SetVelocity(velocity);
}
