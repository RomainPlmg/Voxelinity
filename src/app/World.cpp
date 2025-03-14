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

World::~World() {
    for (auto& chunk : m_Chunks) {
        delete chunk;
        chunk = nullptr;
    }

    m_Chunks.clear();
}

void World::Init() {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryAll, BIND_EVENT_FN(World::OnEvent));
    m_Player.Init();
    m_Player.Move(glm::vec3(0, CHUNK_HEIGHT, 0));

    for (int z = -m_RenderDistance; z <= m_RenderDistance; z++) {
        for (int x = -m_RenderDistance; x <= m_RenderDistance; x++) {
            m_Chunks.emplace_back(new Chunk(glm::ivec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH)));

            auto& chunk = m_Chunks.back();
            chunk->GetShader()->GetUniform("wireframeColor")->SetValue(glm::vec3(1.0f, 1.0f, 1.0f));
            chunk->GetShader()->GetUniform("fogStart")->SetValue(static_cast<float>(m_RenderDistance * CHUNK_WIDTH - CHUNK_WIDTH));
            chunk->GetShader()->GetUniform("fogEnd")->SetValue(static_cast<float>(m_RenderDistance * CHUNK_WIDTH));
            chunk->GetShader()->GetUniform("fogColor")->SetValue(glm::vec3(0.1f, 0.1f, 0.1f));

            // Send chunk generation in threads
            auto& noise = m_Noise;
            ThreadPool::Get().Enqueue([chunk, noise]() { chunk->Load(noise); });
        }
    }
}

void World::Update() {
    for (auto& chunk : m_Chunks) {
        if (chunk->IsGenerated() && !chunk->IsRegistered()) {
            chunk->Register();
            break;
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

std::unique_ptr<World> World::Create() { return std::make_unique<World>(); }
