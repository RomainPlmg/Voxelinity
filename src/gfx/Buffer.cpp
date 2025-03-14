#include "Buffer.h"

#include "GraphicContext.h"
#include "pch.h"

/* Vertex buffer */
VertexBuffer::VertexBuffer(const uint32_t size) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const float* vertices, const uint32_t size) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &m_RendererID); }

void VertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); }

void VertexBuffer::Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) { return std::make_shared<VertexBuffer>(size); }

std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, const uint32_t size) { return std::make_shared<VertexBuffer>(vertices, size); }

/* Element buffer */
ElementBuffer::ElementBuffer(const uint32_t* indices, const uint32_t count) {
    m_Count = count;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
}

ElementBuffer::~ElementBuffer() { glDeleteBuffers(1, &m_RendererID); }

void ElementBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }

void ElementBuffer::Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

std::shared_ptr<ElementBuffer> ElementBuffer::Create(uint32_t* indices, uint32_t count) { return std::make_shared<ElementBuffer>(indices, count); }

/* Buffer Layout */
BufferLayout::BufferLayout(const std::vector<BufferElement>& elements) : m_Elements{elements} {
    uint32_t offset = 0;
    for (auto& element : m_Elements) {
        element.Offset = offset;
        offset += static_cast<uint32_t>(element.Size);
        m_Stride += static_cast<uint32_t>(element.Size);
        m_Size += element.GetComponentCount();
    }
}

std::shared_ptr<BufferLayout> BufferLayout::Create(std::initializer_list<BufferElement> elements) { return std::make_shared<BufferLayout>(elements); }

/* Frame buffer */
FrameBuffer::FrameBuffer(int width, int height) {
    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Framebuffer texture
    glGenTextures(1, &m_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);

    // Depth buffer
    glGenRenderbuffers(1, &m_DepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check framebuffer integrity
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Incomplete framebuffer");
    }
}

FrameBuffer::~FrameBuffer() { glDeleteFramebuffers(1, &m_RendererID); }

void FrameBuffer::Resize(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);  // Bind the framebuffer

    // Update color texture
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Update deth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the framebuffer
}

void FrameBuffer::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID); }

void FrameBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

std::shared_ptr<FrameBuffer> FrameBuffer::Create(int width, int height) { return std::make_shared<FrameBuffer>(width, height); }