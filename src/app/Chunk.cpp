#include "Chunk.h"

#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/Buffer.h"
#include "gfx/Shader.h"
#include "gfx/VertexArray.h"
#include "pch.h"

// clang-format off
const std::unordered_map<Voxel::Face, std::array<float, 16>> Chunk::m_VoxelVertices = {
    //--- Position ---// // -Static illumination- //
{Voxel::Face::Front, {
    0.0f, 0.0f,  1.0f,              0.8f,
    1.0f, 0.0f,  1.0f,              0.8f,
    1.0f, 1.0f,  1.0f,              0.8f,
    0.0f, 1.0f,  1.0f,              0.8f,
}},           
{Voxel::Face::Back, {           
    1.0f, 0.0f, 0.0f,               0.8f,
    0.0f, 0.0f, 0.0f,               0.8f,
    0.0f, 1.0f, 0.0f,               0.8f,
    1.0f, 1.0f, 0.0f,               0.8f,
}},           
{Voxel::Face::Left, {           
    0.0f, 0.0f, 0.0f,               0.8f,
    0.0f, 0.0f, 1.0f,               0.8f,
    0.0f, 1.0f, 1.0f,               0.8f,
    0.0f, 1.0f, 0.0f,               0.8f,
}},           
{Voxel::Face::Right, {           
    1.0f, 0.0f, 1.0f,               0.8f,
    1.0f, 0.0f, 0.0f,               0.8f,
    1.0f, 1.0f, 0.0f,               0.8f,
    1.0f, 1.0f, 1.0f,               0.8f,
}},           
{Voxel::Face::Top, {           
    0.0f, 1.0f, 1.0f,               1.0f,
    1.0f, 1.0f, 1.0f,               1.0f,
    1.0f, 1.0f, 0.0f,               1.0f,
    0.0f, 1.0f, 0.0f,               1.0f,
}},           
{Voxel::Face::Bottom, {           
    0.0f, 0.0f, 0.0f,               0.6f,
    1.0f, 0.0f, 0.0f,               0.6f,
    1.0f, 0.0f, 1.0f,               0.6f,
    0.0f, 0.0f, 1.0f,               0.6f,
}}
};
// clang-format on

Chunk::Chunk(glm::ivec3 position) : m_DataGenerated(false), m_MeshGenerated(false), Renderable(position, 1) {
    // Recover the shader
    m_Shader = ShaderProgramLibrary::Get().GetShaderProgram("gbuffer_terrain");

    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryAll, BIND_EVENT_FN(Chunk::OnEvent));
}

Chunk::~Chunk() { Unregister(); }

void Chunk::GenerateData(const FastNoiseLite& noise) {
    int boundaryIndex = 0;
    for (int z = 0; z < CHUNK_WIDTH; z++) {
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            // Calculate the heightmap at position x/z
            glm::vec3 noiseCoord(m_Position.x + x, 0.0, m_Position.z + z);
            float noiseValue = noise.GetNoise(noiseCoord.x, noiseCoord.z);
            // Noise bounded between -1...1, convert between 1...CHUNK_HEIGHT
            int height = static_cast<int>(((noiseValue + 1.0f) / 2.0f) * (CHUNK_HEIGHT - 1)) + 1;

            // Build blocks on vertical axis
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int index = GetVoxelIndex(glm::vec3(x, y, z));

                glm::vec3 cubePos = glm::vec3(x, y, z);
                m_Voxels[index] = new Voxel(cubePos);

                // Setup the voxel bounding box
                Box voxelBox(m_Position + cubePos, glm::vec3(1.0f));
                m_Voxels[index]->SetBoundingBox(voxelBox);

                // Check if voxel is at boundary
                if (x == 0 || x == CHUNK_WIDTH - 1 || z == 0 || z == CHUNK_WIDTH - 1 || y == 0 || y == CHUNK_HEIGHT - 1) {
                    m_BoundaryVoxels[boundaryIndex] = m_Voxels[index];
                    boundaryIndex++;
                }

                if (y == 0) {
                    m_Voxels[index]->SetFaceInvisible(Voxel::Face::Bottom);
                }

                if (y > height) {
                    m_Voxels[index]->SetTransparent(true);
                }
            }
        }
    }
    m_DataGenerated.store(true, std::memory_order_release);
    ChunkDataGeneratedEvent event = ChunkDataGeneratedEvent();
    EventDispatcher::Get().Dispatch(event);
}

void Chunk::GenerateMesh() {
    for (auto& voxel : m_Voxels) {
        if (voxel->IsTransparent()) continue;

        auto cubePos = voxel->GetPosition();
        // Calculate the vertices position relative to the cube position
        if (m_Shader == nullptr) {
            LOG_ERROR("No shader binded for the actual chunk");
            return;
        }

        int m_ShaderBufferSize = m_Shader->GetBufferLayout()->GetSize();
        for (auto& [face, vertices] : m_VoxelVertices) {
            if (!voxel->NeedToRenderFace(face)) continue;
            // Copy the non modifiable common vertices into a buffer to edit them
            std::array<float, 16> movedVertices = vertices;

            // Edit the new vertices with the cube position in the chunk
            for (int i = 0; i < vertices.size(); i = i + m_ShaderBufferSize) {
                movedVertices[i] += cubePos.x;
                movedVertices[i + 1] += cubePos.y;
                movedVertices[i + 2] += cubePos.z;
            }

            // Write the new vertices into the final vertex buffer
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Vertices.insert(m_Vertices.end(), movedVertices.begin(), movedVertices.end());
            }

            // Update the element buffer
            AddFaceToIndices();
        }
    }

    m_MeshGenerated.store(true, std::memory_order_release);
    AddNewRenderableEvent event(this);
    EventDispatcher::Get().Dispatch(event);
}

void Chunk::Unload() {
    for (auto& voxel : m_Voxels) {
        delete voxel;
    }
}

void Chunk::Update() {}

void Chunk::OnEvent(const Event& event) {
    if (event.GetType() == EventType::ToggleWireframeView) {
        const auto* wireframeEvent = dynamic_cast<const ToggleWireframeViewEvent*>(&event);
        m_Shader->GetUniform("wireframeMode")->SetValue(wireframeEvent->enable);
    }
}

void Chunk::RemoveInternalFaces() {
    for (auto& voxel : m_Voxels) {
        for (int i = 0; i < 6; i++) {
            auto face = static_cast<Voxel::Face>(i);
            if (face == Voxel::Face::Back) {
                if (voxel->GetPosition().z != 0) {
                    auto neighbor = GetVoxelatCoord(glm::vec3(voxel->GetPosition().x, voxel->GetPosition().y, voxel->GetPosition().z - 1));
                    if (!neighbor->IsTransparent()) {
                        voxel->SetFaceInvisible(face);
                    }
                }
            } else if (face == Voxel::Face::Front) {
                if (voxel->GetPosition().z != CHUNK_WIDTH - 1) {
                    auto neighbor = GetVoxelatCoord(glm::vec3(voxel->GetPosition().x, voxel->GetPosition().y, voxel->GetPosition().z + 1));
                    if (!neighbor->IsTransparent()) {
                        voxel->SetFaceInvisible(face);
                    }
                }
            } else if (face == Voxel::Face::Left) {
                if (voxel->GetPosition().x != 0) {
                    auto neighbor = GetVoxelatCoord(glm::vec3(voxel->GetPosition().x - 1, voxel->GetPosition().y, voxel->GetPosition().z));
                    if (!neighbor->IsTransparent()) {
                        voxel->SetFaceInvisible(face);
                    }
                }
            } else if (face == Voxel::Face::Right) {
                if (voxel->GetPosition().x != CHUNK_WIDTH - 1) {
                    auto neighbor = GetVoxelatCoord(glm::vec3(voxel->GetPosition().x + 1, voxel->GetPosition().y, voxel->GetPosition().z));
                    if (!neighbor->IsTransparent()) {
                        voxel->SetFaceInvisible(face);
                    }
                }

            } else if (face == Voxel::Face::Top) {
                if (voxel->GetPosition().y != CHUNK_HEIGHT - 1) {
                    auto neighbor = GetVoxelatCoord(glm::vec3(voxel->GetPosition().x, voxel->GetPosition().y + 1, voxel->GetPosition().z));
                    if (!neighbor->IsTransparent()) {
                        voxel->SetFaceInvisible(face);
                    }
                }
            } else if (face == Voxel::Face::Bottom) {
                if (voxel->GetPosition().y != 0) {
                    auto neighbor = GetVoxelatCoord(glm::vec3(voxel->GetPosition().x, voxel->GetPosition().y - 1, voxel->GetPosition().z));
                    if (!neighbor->IsTransparent()) {
                        voxel->SetFaceInvisible(face);
                    }
                }
            }
        }
    }
}

void Chunk::RemoveBoundaryFaces(std::array<std::shared_ptr<Chunk>, 4> neighbors) {
    glm::vec3 chunkPosition = m_Position;
    chunkPosition.x /= CHUNK_WIDTH;
    chunkPosition.z /= CHUNK_WIDTH;

    for (auto& voxel : m_BoundaryVoxels) {
        if (voxel == nullptr) {
            LOG_ERROR("Cannot remove boundary faces, boundary voxel is nullptr.");
            continue;
        }
        auto voxelPosition = voxel->GetPosition();
        if (voxelPosition.x == 0 && neighbors[X_NEG]) {
            auto neighborVoxel = neighbors[X_NEG]->GetVoxelatCoord(glm::vec3(CHUNK_WIDTH - 1, voxelPosition.y, voxelPosition.z));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Left);
            }
        } else if (voxelPosition.x == CHUNK_WIDTH - 1 && neighbors[X_POS]) {
            auto neighborVoxel = neighbors[X_POS]->GetVoxelatCoord(glm::vec3(0, voxelPosition.y, voxelPosition.z));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Right);
            }
        }
        if (voxelPosition.z == CHUNK_WIDTH - 1 && neighbors[Z_POS]) {
            auto neighborVoxel = neighbors[Z_POS]->GetVoxelatCoord(glm::vec3(voxelPosition.x, voxelPosition.y, 0));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Front);
            }
        } else if (voxelPosition.z == 0 && neighbors[Z_NEG]) {
            auto neighborVoxel = neighbors[Z_NEG]->GetVoxelatCoord(glm::vec3(voxelPosition.x, voxelPosition.y, CHUNK_WIDTH - 1));
            if (neighborVoxel != nullptr && !neighborVoxel->IsTransparent()) {
                voxel->SetFaceInvisible(Voxel::Face::Back);
            }
        }
    }
}

glm::vec3 Chunk::GetVoxelCoord(int index) {
    glm::vec3 coord;
    coord.y = index % CHUNK_HEIGHT;   // Recover y by peeking the carry
    int temp = index / CHUNK_HEIGHT;  // Remove y from index

    coord.x = temp % CHUNK_WIDTH;  // Recover x
    coord.z = temp / CHUNK_WIDTH;  // The carry is z

    return coord;
}

int Chunk::GetVoxelIndex(const glm::vec3& coord) { return coord.y + (coord.x * CHUNK_HEIGHT) + (coord.z * CHUNK_WIDTH * CHUNK_HEIGHT); }

Voxel* Chunk::GetVoxelatCoord(const glm::vec3& coord) {
    int index = GetVoxelIndex(coord);
    if (index >= CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT || index < 0) {
        LOG_WARNING("Cube at position xyz: {0} | {1} | {2} is out of bounds of the chunk", coord.x, coord.y, coord.z);
        return nullptr;
    }
    return m_Voxels[index];
}
