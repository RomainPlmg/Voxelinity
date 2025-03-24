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

World::World() : m_IsPaused(false) {}

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

std::unique_ptr<World> World::Create() { return std::make_unique<World>(); }
