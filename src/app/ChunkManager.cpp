#include "ChunkManager.h"

#include "Chunk.h"
#include "core/ThreadPool.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/Shader.h"

ChunkManager::ChunkManager() : m_RenderDistance(16), m_NbChunksWithData(0) {
    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(ChunkManager::OnEvent));
}

ChunkManager::~ChunkManager() {
    while (!m_ChunksToRender.empty()) {
        m_ChunksToRender.pop();  // Empty the render queue
    }
    m_Chunks.clear();  // Clear the chunk map
}

void ChunkManager::Init() {
    for (int z = -m_RenderDistance; z <= m_RenderDistance; z++) {
        for (int x = -m_RenderDistance; x <= m_RenderDistance; x++) {
            m_Chunks.emplace(glm::ivec3(x, 0, z), std::make_shared<Chunk>(glm::ivec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH)));

            auto& chunk = m_Chunks[glm::ivec3(x, 0, z)];
            m_ChunksToMesh.push(chunk.get());  // Register the chunk into the vector to iterate later
            chunk->GetShader()->GetUniform("wireframeColor")->SetValue(glm::vec3(1.0f, 1.0f, 1.0f));
            chunk->GetShader()->GetUniform("fogStart")->SetValue(static_cast<float>(m_RenderDistance * CHUNK_WIDTH - 16));
            chunk->GetShader()->GetUniform("fogEnd")->SetValue(static_cast<float>(m_RenderDistance * CHUNK_WIDTH));
            chunk->GetShader()->GetUniform("fogColor")->SetValue(glm::vec3(0.1f, 0.1f, 0.1f));

            // Send chunk generation in threads
            auto& noise = m_Noise;
            ThreadPool::Get().Enqueue(
                [chunkPtr = chunk, noise]() { chunkPtr->GenerateData(noise); });  // Create a chunkPtr copy to avoid chunk destruction
        }
    }
}

void ChunkManager::Update() {
    if (m_NbChunksWithData >= m_Chunks.size()) {
        if (!m_ChunksToMesh.empty()) {
            auto& chunk = m_ChunksToMesh.front();
            glm::ivec3 position = static_cast<glm::ivec3>(chunk->GetPosition());
            ThreadPool::Get().Enqueue([this, position, chunkPtr = chunk]() {
                chunkPtr->RemoveInternalFaces();
                chunkPtr->RemoveBoundaryFaces(GetNeighbors(position));
                chunkPtr->GenerateMesh();
            });

            m_ChunksToMesh.pop();
        }

        if (!m_ChunksToRender.empty()) {
            m_ChunksToRender.front()->Register();
            m_ChunksToRender.pop();
        }
    }
}

std::array<std::shared_ptr<Chunk>, 4> ChunkManager::GetNeighbors(glm::ivec3 pos) {
    glm::ivec3 finalPos = pos;
    finalPos.x /= CHUNK_WIDTH;
    finalPos.z /= CHUNK_WIDTH;

    std::array<std::shared_ptr<Chunk>, 4> neighbors;

    neighbors[X_POS] = (m_Chunks.find(glm::vec3(finalPos.x + 1, finalPos.y, finalPos.z)) != m_Chunks.end())
                           ? m_Chunks[glm::vec3(finalPos.x + 1, finalPos.y, finalPos.z)]
                           : nullptr;
    neighbors[X_NEG] = (m_Chunks.find(glm::vec3(finalPos.x - 1, finalPos.y, finalPos.z)) != m_Chunks.end())
                           ? m_Chunks[glm::vec3(finalPos.x - 1, finalPos.y, finalPos.z)]
                           : nullptr;
    neighbors[Z_POS] = (m_Chunks.find(glm::vec3(finalPos.x, finalPos.y, finalPos.z + 1)) != m_Chunks.end())
                           ? m_Chunks[glm::vec3(finalPos.x, finalPos.y, finalPos.z + 1)]
                           : nullptr;
    neighbors[Z_NEG] = (m_Chunks.find(glm::vec3(finalPos.x, finalPos.y, finalPos.z - 1)) != m_Chunks.end())
                           ? m_Chunks[glm::vec3(finalPos.x, finalPos.y, finalPos.z - 1)]
                           : nullptr;
    return neighbors;
}

void ChunkManager::OnEvent(const Event& event) {
    if (event.GetType() == EventType::AddNewRenderable) {
        const auto* renderableEvent = dynamic_cast<const AddNewRenderableEvent*>(&event);
        if (!renderableEvent) return;

        auto renderable = const_cast<Renderable*>(renderableEvent->GetRenderable());
        if (auto* chunk = dynamic_cast<Chunk*>(renderable)) {
            m_ChunksToRender.push(chunk);
        }
    }
    if (event.GetType() == EventType::ChunkDataGenerated) {
        m_NbChunksWithData++;
    }
}
