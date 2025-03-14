#ifndef __RENDERABLE_H__
#define __RENDERABLE_H__

#include <atomic>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>
#include <vector>

class ShaderProgram;
class VertexBuffer;
class ElementBuffer;
class VertexArray;

class Renderable {
   public:
    Renderable(const glm::vec3& position, int ID = 0) : m_Position(position), m_ID(0) { m_RenderablesToDraw.reserve(500); }
    virtual ~Renderable() = default;

    virtual void Update() = 0;
    void Register();
    void Unregister();
    bool IsRegistered();

    /* Getters */
    int GetID() const { return m_ID; }

    glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }

    std::shared_ptr<ShaderProgram> GetShader() const { return m_Shader; }
    std::vector<float> GetVertices() const { return m_Vertices; }
    std::vector<uint32_t> GetIndices() const { return m_Indices; }

    std::shared_ptr<VertexBuffer> GetVBO() const { return m_VBO; }
    std::shared_ptr<ElementBuffer> GetEBO() const { return m_EBO; }
    std::shared_ptr<VertexArray> GetVAO() const { return m_VAO; }

    static std::unordered_set<Renderable*>& GetRenderablesToDraw() { return m_RenderablesToDraw; }

   protected:
    int m_ID;
    std::atomic<bool> m_Registered = false;

    glm::vec3 m_Position;
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

    std::shared_ptr<ShaderProgram> m_Shader;
    std::vector<float> m_Vertices;
    std::vector<uint32_t> m_Indices;

    std::shared_ptr<VertexBuffer> m_VBO;
    std::shared_ptr<ElementBuffer> m_EBO;
    std::shared_ptr<VertexArray> m_VAO;

    void AddFaceToIndices();

    // Register all renderables that need to be rendered
    static std::unordered_set<Renderable*> m_RenderablesToDraw;
};

#endif  // __RENDERABLE_H__