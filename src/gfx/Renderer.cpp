#include "Renderer.h"

#include "Buffer.h"
#include "GraphicContext.h"
#include "Renderable.h"
#include "Shader.h"
#include "VertexArray.h"
#include "core/Window.h"
#include "events/EventApplication.h"
#include "events/EventDispatcher.h"
#include "gfx/Camera.h"
#include "pch.h"
#include "utils/Logger.h"

Renderer::Renderer(int width, int height)
    : m_Camera(nullptr), m_DrawCalls(0), m_NbTrianglesRendered(0), m_StateGuard(), m_LastShader(nullptr), m_LastVAO(nullptr) {
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_ProjMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 300.0f);
}

void Renderer::Init() {
    glEnable(GL_BLEND);  // Activate blending for object transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);  // Activate the depth buffer
    glEnable(GL_LINE_SMOOTH);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glEnable(GL_MULTISAMPLE);

    m_StateGuard.Save();

    EventDispatcher::Get().Subscribe(EventCategory::EventCategoryApplication, BIND_EVENT_FN(Renderer::OnEvent));
}

void Renderer::Render() {
    m_StateGuard.Restore();
    m_DrawCalls = 0;
    m_NbTrianglesRendered = 0;

    for (auto& renderable : Renderable::GetRenderablesToDraw()) {
        const auto& ebo = renderable->GetEBO();
        const auto& vao = renderable->GetVAO();
        const auto& shader = renderable->GetShader();
        const auto& modelMatrix = renderable->GetModelMatrix();
        glm::mat4 viewMatrix = (m_Camera) ? m_Camera->GetViewMatrix() : glm::mat4(1.0f);

        m_NbTrianglesRendered += ebo->GetCount() / 3;

        // Sanity checks
        if (shader == nullptr) {
            LOG_ERROR("The shader is nullptr for the renderable '{0}'", renderable->GetID());
            continue;
        }
        if (vao == nullptr) {
            LOG_ERROR("The vertex array is nullptr for the renderable '{0}'", renderable->GetID());
            continue;
        }

        // Setup the shader
        if (m_LastShader != shader.get()) {  // First pass
            if (m_LastShader) m_LastShader->Unbind();
            shader->Bind();
            shader->GetUniform("projMatrix")->SetValue(m_ProjMatrix);
            m_LastShader = shader.get();
        }

        // Setup the camera & projection matrix
        shader->GetUniform("viewMatrix")->SetValue(viewMatrix);
        shader->GetUniform("modelMatrix")->SetValue(modelMatrix);  // Update the uniform value

        // Set the uniforms
        for (auto& [name, uniform] : shader->GetUniforms()) {
            if (uniform->type == ShaderDataType::Bool) {
                shader->SetUniformBool(name, uniform->GetValue<bool>());
            } else if (uniform->type == ShaderDataType::Int) {
                shader->SetUniformInt(name, uniform->GetValue<int>());
            } else if (uniform->type == ShaderDataType::Float) {
                shader->SetUniformFloat(name, uniform->GetValue<float>());
            } else if (uniform->type == ShaderDataType::Float3) {
                shader->SetUniformFloat3(name, uniform->GetValue<glm::vec3>());
            } else if (uniform->type == ShaderDataType::Mat4) {
                shader->SetUniformMat4(name, uniform->GetValue<glm::mat4>());
            }
        }

        // Setup the VAO
        if (m_LastVAO != vao.get()) {
            if (m_LastVAO) m_LastVAO->Unbind();
            vao->Bind();
            m_LastVAO = vao.get();
        }

        // Draw element
        glDrawElements(GL_TRIANGLES, ebo->GetCount(), GL_UNSIGNED_INT, nullptr);

        // Increase number of drawcalls
        m_DrawCalls++;
    }
}

void Renderer::Shutdown() {}

void Renderer::OnEvent(const Event& event) {
    if (event.GetType() == EventType::ToggleWireframeView) {
        const auto* wireframeEvent = dynamic_cast<const ToggleWireframeViewEvent*>(&event);
        if (wireframeEvent->enable) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

void Renderer::SetViewport(const int width, const int height) {
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_ProjMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 300.0f);
    glViewport(0, 0, width, height);
}

void Renderer::SetCamera(const Camera& camera) { m_Camera = &camera; }

std::unique_ptr<Renderer> Renderer::Create(int width, int height) { return std::make_unique<Renderer>(width, height); }