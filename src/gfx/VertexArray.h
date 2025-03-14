#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include <memory>
#include <vector>

#include "Buffer.h"

typedef unsigned int GLenum;

class VertexArray {
   public:
    VertexArray();
    ~VertexArray();

    static GLenum GetOpenGLType(ShaderDataType type);

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &buffer);
    void AddElementBuffer(const std::shared_ptr<ElementBuffer> &buffer);

    static std::shared_ptr<VertexArray> Create();

   private:
    uint32_t m_RendererID;
    uint32_t m_VertexBufferIndex = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<ElementBuffer> m_ElementBuffer;
};

#endif  // __VERTEX_ARRAY_H__