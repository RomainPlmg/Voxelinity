#include "Renderable.h"

#include "Buffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "pch.h"
#include "utils/Logger.h"

std::unordered_set<Renderable*> Renderable::m_RenderablesToDraw;

Renderable::~Renderable() { LOG_TRACE("Destroy renderable"); }

void Renderable::Register() {
    // Create Vertex Array Object
    m_VAO = VertexArray::Create();

    // Create Vertex Buffer Object & Element Buffer Object
    m_VBO = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(float));
    m_VBO->SetLayout(m_Shader->GetBufferLayout());
    m_EBO = ElementBuffer::Create(m_Indices.data(), m_Indices.size());

    m_VAO->AddVertexBuffer(m_VBO);
    m_VAO->AddElementBuffer(m_EBO);

    m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position));

    m_RenderablesToDraw.insert(this);

    m_Registered.store(true, std::memory_order_release);
}

void Renderable::Unregister() {
    m_RenderablesToDraw.erase(this);
    m_Registered.store(false, std::memory_order_release);
}

bool Renderable::IsRegistered() { return m_Registered; }

void Renderable::AddFaceToIndices() {
    std::lock_guard<std::mutex> lock(m_Mutex);

    int indexStart = 0;

    if (m_Indices.size() != 0) {
        indexStart = m_Indices.back() + 1;
    }

    m_Indices.push_back(indexStart);
    m_Indices.push_back(indexStart + 1);
    m_Indices.push_back(indexStart + 3);

    m_Indices.push_back(indexStart + 1);
    m_Indices.push_back(indexStart + 2);
    m_Indices.push_back(indexStart + 3);
}