#include "World.h"

#include "Chunk.h"
#include "core/ThreadPool.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "events/EventKeyboard.h"
#include "gfx/GraphicContext.h"
#include "gfx/Renderable.h"
#include "gfx/Shader.h"
#include "pch.h"
// DEBUG
#include "utils/Logger.h"
#include "utils/Time.h"

WorldStatus World::m_Status;

World::World() : m_IsPaused(false), m_RenderDistance(16) {
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Chunks.reserve((m_RenderDistance * 2 + 1) * (m_RenderDistance * 2 + 1));
    m_Renderables.reserve((m_RenderDistance * 2 + 1) * (m_RenderDistance * 2 + 1));
}

World::~World() { m_Chunks.clear(); }

void World::Init() {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryAll, BIND_EVENT_FN(World::OnEvent));
    m_Player.Init();
    m_Player.Move(glm::vec3(0, CHUNK_HEIGHT, 0));

    for (int z = -m_RenderDistance; z <= m_RenderDistance; z++) {
        for (int x = -m_RenderDistance; x <= m_RenderDistance; x++) {
            m_Chunks[glm::vec3(x, 0, z)] = std::make_shared<Chunk>(glm::ivec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH));

            auto& chunk = m_Chunks[glm::vec3(x, 0, z)];
            chunk->GetShader()->GetUniform("wireframeColor")->SetValue(glm::vec3(1.0f, 1.0f, 1.0f));
            chunk->GetShader()->GetUniform("fogStart")->SetValue(static_cast<float>(m_RenderDistance * CHUNK_WIDTH - CHUNK_WIDTH));
            chunk->GetShader()->GetUniform("fogEnd")->SetValue(static_cast<float>(m_RenderDistance * CHUNK_WIDTH));
            chunk->GetShader()->GetUniform("fogColor")->SetValue(glm::vec3(0.1f, 0.1f, 0.1f));

            // Send chunk generation in threads
            auto& noise = m_Noise;
            ThreadPool::Get().Enqueue([chunk, noise]() { chunk->GenerateData(noise); });
        }
    }
}

void World::Update() {
    if (AllChunksGenerated()) {
        std::shared_lock<std::shared_mutex> lock(m_ChunkMutex);

        for (auto& [position, chunk] : m_Chunks) {
            if (!chunk->IsMeshGenerated() && !chunk->IsRegistered()) {
                auto chunkPtrCopy = chunk;
                ThreadPool::Get().Enqueue([this, chunkPtrCopy]() {
                    this->RemoveBoundaryFaces(*chunkPtrCopy);
                    chunkPtrCopy->GenerateMesh();
                });
            } else if (chunk->IsMeshGenerated() && !chunk->IsRegistered()) {
                chunk->Register();
                break;
            }
        }
    }

    if (m_IsPaused) return;

    m_Player.Update();

    // Update status
    m_Status.playerPos = m_Player.GetPosition();
}

void World::OnEvent(const Event& event) {
    if (event.GetType() == EventType::KeyPressed) {
        const auto* keyEvent = dynamic_cast<const KeyPressedEvent*>(&event);
        if (keyEvent->GetKeyCode() == GLFW_KEY_ESCAPE) {
            m_IsPaused = !m_IsPaused;
            PauseEvent event(m_IsPaused);
            EventDispatcher::Get().Dispatch(event);
        }
    }
    if (event.GetType() == EventType::AppPause) {
        const auto* pauseEvent = dynamic_cast<const PauseEvent*>(&event);
        m_IsPaused = pauseEvent->isPaused;
    }
}

const bool World::AllChunksGenerated() const {
    for (auto& [position, chunk] : m_Chunks) {
        if (!chunk->IsDataGenerated()) return false;
    }
    return true;
}

std::unique_ptr<World> World::Create() { return std::make_unique<World>(); }

void World::RemoveBoundaryFaces(Chunk& chunk) {
    auto chunkPosition = chunk.GetPosition();
    chunkPosition.x /= CHUNK_WIDTH;
    chunkPosition.z /= CHUNK_WIDTH;

    // Read lock on neighbor chunks
    std::shared_lock<std::shared_mutex> lock(m_ChunkMutex);
    std::shared_ptr<Chunk> rightNeighbor = (m_Chunks.find(glm::vec3(chunkPosition.x + 1, chunkPosition.y, chunkPosition.z)) != m_Chunks.end())
                                               ? m_Chunks[glm::vec3(chunkPosition.x + 1, chunkPosition.y, chunkPosition.z)]
                                               : nullptr;
    std::shared_ptr<Chunk> leftNeighbor = (m_Chunks.find(glm::vec3(chunkPosition.x - 1, chunkPosition.y, chunkPosition.z)) != m_Chunks.end())
                                              ? m_Chunks[glm::vec3(chunkPosition.x - 1, chunkPosition.y, chunkPosition.z)]
                                              : nullptr;
    std::shared_ptr<Chunk> frontNeighbor = (m_Chunks.find(glm::vec3(chunkPosition.x, chunkPosition.y, chunkPosition.z + 1)) != m_Chunks.end())
                                               ? m_Chunks[glm::vec3(chunkPosition.x, chunkPosition.y, chunkPosition.z + 1)]
                                               : nullptr;
    std::shared_ptr<Chunk> backNeighbor = (m_Chunks.find(glm::vec3(chunkPosition.x, chunkPosition.y, chunkPosition.z - 1)) != m_Chunks.end())
                                              ? m_Chunks[glm::vec3(chunkPosition.x, chunkPosition.y, chunkPosition.z - 1)]
                                              : nullptr;

    lock.unlock();

    for (auto& voxel : chunk.GetBoundaryVoxels()) {
        if (voxel == nullptr) {
            LOG_ERROR("Cannot remove boundary faces, boundary voxel is nullptr.");
            continue;
        }
        auto voxelPosition = voxel->GetPosition();
        if (voxelPosition.x == 0 && leftNeighbor) {
            auto neighborVoxel = leftNeighbor->GetVoxelatCoord(glm::vec3(CHUNK_WIDTH - 1, voxelPosition.y, voxelPosition.z));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Left);
            }
        } else if (voxelPosition.x == CHUNK_WIDTH - 1 && rightNeighbor) {
            auto neighborVoxel = rightNeighbor->GetVoxelatCoord(glm::vec3(0, voxelPosition.y, voxelPosition.z));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Right);
            }
        }
        if (voxelPosition.z == CHUNK_WIDTH - 1 && frontNeighbor) {
            auto neighborVoxel = frontNeighbor->GetVoxelatCoord(glm::vec3(voxelPosition.x, voxelPosition.y, 0));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Front);
            }
        } else if (voxelPosition.z == 0 && backNeighbor) {
            auto neighborVoxel = backNeighbor->GetVoxelatCoord(glm::vec3(voxelPosition.x, voxelPosition.y, CHUNK_WIDTH - 1));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Back);
            }
        }
    }
}
