#ifndef __GRAPIC_CONTEXT_H__
#define __GRAPIC_CONTEXT_H__

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <memory>

class GraphicContext {
   public:
    GraphicContext(GLFWwindow* windowHandler);
    ~GraphicContext();

    void Init();

    static std::unique_ptr<GraphicContext> Create(GLFWwindow* windowHandler);

   private:
    GLFWwindow* m_Handler;
};

#endif  // __GRAPIC_CONTEXT_H__