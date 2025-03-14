#include "VertexArray.h"

#include "GraphicContext.h"
#include "pch.h"
#include "utils/Logger.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_RendererID);
    glBindVertexArray(m_RendererID);
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_RendererID); }

GLenum VertexArray::GetOpenGLType(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::None:
            return GL_NONE;
        case ShaderDataType::Float:
            return GL_FLOAT;
        case ShaderDataType::Float2:
            return GL_FLOAT;
        case ShaderDataType::Float3:
            return GL_FLOAT;
        case ShaderDataType::Float4:
            return GL_FLOAT;
        case ShaderDataType::Mat3:
            return GL_FLOAT;
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
            return GL_INT;
        case ShaderDataType::Int2:
            return GL_INT;
        case ShaderDataType::Int3:
            return GL_INT;
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        default:
            break;
    }
    LOG_ERROR("Unknown shader data type.");
    return 0;
}

void VertexArray::Bind() const { glBindVertexArray(m_RendererID); }

void VertexArray::Unbind() const { glBindVertexArray(0); }

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) {
    glBindVertexArray(m_RendererID);
    buffer->Bind();

    const auto& layout = buffer->GetLayout();
    for (const auto& element : layout->GetElements()) {
        if (element.Type == ShaderDataType::Float || element.Type == ShaderDataType::Float2 || element.Type == ShaderDataType::Float3 ||
            element.Type == ShaderDataType::Float4) {
            glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(), GetOpenGLType(element.Type),
                                  element.Normalized ? GL_TRUE : GL_FALSE, layout->GetStride(),
                                  reinterpret_cast<const GLvoid*>(static_cast<uintptr_t>(element.Offset)));
            glEnableVertexAttribArray(m_VertexBufferIndex);
        } else if (element.Type == ShaderDataType::Int || element.Type == ShaderDataType::Int2 || element.Type == ShaderDataType::Int3 ||
                   element.Type == ShaderDataType::Int4 || element.Type == ShaderDataType::Bool) {
            glVertexAttribIPointer(m_VertexBufferIndex, element.GetComponentCount(), GetOpenGLType(element.Type), layout->GetStride(),
                                   reinterpret_cast<const GLvoid*>(static_cast<uintptr_t>(element.Offset)));
            glEnableVertexAttribArray(m_VertexBufferIndex);
        } else if (element.Type == ShaderDataType::Mat3 || element.Type == ShaderDataType::Mat4) {
            // glEnableVertexAttribArray(m_VertexBufferIndex);
            LOG_ERROR("Matrices in vertex arrays not yet supported");
            return;
        } else {
            LOG_ERROR("Unknown shader data type");
            return;
        }
        m_VertexBufferIndex++;
    }

    m_VertexBuffers.push_back(buffer);
}

void VertexArray::AddElementBuffer(const std::shared_ptr<ElementBuffer>& buffer) {
    glBindVertexArray(m_RendererID);
    buffer->Bind();

    m_ElementBuffer = buffer;
}

std::shared_ptr<VertexArray> VertexArray::Create() { return std::make_shared<VertexArray>(); }