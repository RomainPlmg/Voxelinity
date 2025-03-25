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

WorldStatus World::m_Status;

World::World() : m_IsPaused(false), m_CollisionManager(m_Player, *this) {}

World::~World() {}

void World::Init() {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryAll, BIND_EVENT_FN(World::OnEvent));
    m_Player.Init();
    m_Player.Move(glm::vec3(0, CHUNK_HEIGHT, 0));
    m_ChunkManager.Init();
}

void World::Update() {
    m_ChunkManager.Update();
    m_CollisionManager.Update();

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

Voxel* World::GetVoxel(glm::vec3 pos) const {
    if (pos.y > CHUNK_HEIGHT - 1 || pos.y < 0) return nullptr;

    glm::ivec3 chunkPos;
    chunkPos.x = static_cast<int>(pos.x) / CHUNK_WIDTH;
    chunkPos.y = 0;
    chunkPos.z = static_cast<int>(pos.z) / CHUNK_WIDTH;

    glm::vec3 localPos;
    localPos.x = (static_cast<int>(pos.x) % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
    localPos.y = pos.y;
    localPos.z = (static_cast<int>(pos.z) % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;

    // LOG_TRACE("CHECK CUBE {0} | {1} | {2}", localPos.x, localPos.y, localPos.z);

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
