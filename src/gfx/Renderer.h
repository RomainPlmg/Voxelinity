#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>

#include "GraphicStateGuard.h"

class Renderable;
class Camera;
class Event;
class ShaderProgram;
class VertexArray;

class Renderer {
   public:
    Renderer(int width, int height);
    ~Renderer() = default;

    void Init();
    void Render();
    void Shutdown();

    void OnEvent(const Event& event);

    /* Getters */
    int GetDrawcalls() const { return m_DrawCalls; }
    int GetNbTrianglesRendered() const { return m_NbTrianglesRendered; }

    /* Setters */
    void SetViewport(const int width, const int height);
    void SetCamera(const Camera& camera);

    static std::unique_ptr<Renderer> Create(int width, int height);

   private:
    const Camera* m_Camera;
    GraphicStateGuard m_StateGuard;
    glm::mat4 m_ProjMatrix;
    int m_DrawCalls;
    int m_NbTrianglesRendered;

    ShaderProgram* m_LastShader;
    VertexArray* m_LastVAO;
};

#endif  // __RENDERER_H__