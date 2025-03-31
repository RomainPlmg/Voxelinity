#include "World.h"

#include "Chunk.h"
#include "CollisionManager.h"
#include "core/ThreadPool.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "events/EventKeyboard.h"
#include "gfx/GraphicContext.h"
#include "gfx/Renderable.h"
#include "gfx/Shader.h"
#include "pch.h"

WorldStatus World::m_Status;

World::World() : m_IsPaused(false), m_CollisionManager(*this) {}

World::~World() {}

void World::Init() {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryAll, BIND_EVENT_FN(World::OnEvent));
    m_Player.Init();
    m_Player.Move(glm::vec3(0, CHUNK_HEIGHT, 0));
    m_ChunkManager.Init();
}

void World::Update() {
    m_ChunkManager.Update();

    if (m_IsPaused) return;

    m_Player.Update();
    m_CollisionManager.Update();

    m_Player.GetCamera().Update();

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

Voxel* World::GetVoxel(const glm::vec3& pos) const {
    if (pos.y > CHUNK_HEIGHT - 1 || pos.y < 0) return nullptr;

    // LOG_TRACE("World pos = {0} | {1} | {2}", pos.x, pos.y, pos.z);

    glm::ivec3 chunkPos;
    chunkPos.x = (pos.x < 0) ? ((static_cast<int>(pos.x) - CHUNK_WIDTH + 1) / CHUNK_WIDTH) : (static_cast<int>(pos.x) / CHUNK_WIDTH);
    chunkPos.y = 0;
    chunkPos.z = (pos.z < 0) ? ((static_cast<int>(pos.z) - CHUNK_WIDTH + 1) / CHUNK_WIDTH) : (static_cast<int>(pos.z) / CHUNK_WIDTH);

    glm::uvec3 localPos;
    localPos.x = (static_cast<int>(pos.x) % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
    localPos.y = pos.y;
    localPos.z = (static_cast<int>(pos.z) % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;

    // LOG_TRACE("Local pos = {0} | {1} | {2}", localPos.x, localPos.y, localPos.z);

    Chunk* chunk = m_ChunkManager.GetChunk(chunkPos);
    if (chunk) {
        Voxel* voxel = chunk->GetVoxelatCoord(localPos);
        if (voxel) {
            return voxel;
        }
    }
    return nullptr;
}

std::unique_ptr<World> World::Create() { return std::make_unique<World>(); }
