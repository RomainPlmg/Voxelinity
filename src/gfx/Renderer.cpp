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

Renderer::Renderer(int width, int height) : m_Camera(nullptr), m_DrawCalls(0), m_NbTrianglesRendered(0), m_StateGuard() {
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

        m_NbTrianglesRendered += ebo->GetCount() / 3;

        if (shader == nullptr) {
            LOG_ERROR("The shader is nullptr for the renderable '{0}'", renderable->GetID());
            continue;
        }
        shader->GetUniform("modelMatrix")->SetValue(modelMatrix);
        if (m_Camera == nullptr) {
            LOG_WARNING("No camera provided to the renderer");
            shader->GetUniform("viewMatrix")->SetValue(glm::mat4(1.0f));
        } else {
            shader->GetUniform("viewMatrix")->SetValue(m_Camera->GetViewMatrix());
        }
        shader->GetUniform("projMatrix")->SetValue(m_ProjMatrix);

        if (vao == nullptr) {
            LOG_ERROR("The vertex array is nullptr for the renderable '{0}'", renderable->GetID());
            continue;
        }

        // Setup the shader
        shader->Bind();

        // Set the uniforms
        for (auto& [name, uniform] : shader->GetUniforms()) {
            if (uniform->type == ShaderDataType::Bool) {
                shader->SetUniformBool(name, uniform->GetValue<bool>());
            } else if (uniform->type == ShaderDataType::Int) {
                shader->SetUniformFloat(name, uniform->GetValue<int>());
            } else if (uniform->type == ShaderDataType::Float) {
                shader->SetUniformFloat(name, uniform->GetValue<float>());
            } else if (uniform->type == ShaderDataType::Float3) {
                shader->SetUniformFloat3(name, uniform->GetValue<glm::vec3>());
            } else if (uniform->type == ShaderDataType::Mat4) {
                shader->SetUniformMat4(name, uniform->GetValue<glm::mat4>());
            }
        }

        shader->SetUniformMat4("modelMatrix", modelMatrix);
        if (m_Camera == nullptr) {
            LOG_WARNING("No camera provided to the renderer");
            shader->SetUniformMat4("viewMatrix", glm::mat4(1.0f));
        } else {
            shader->SetUniformMat4("viewMatrix", m_Camera->GetViewMatrix());
        }
        shader->SetUniformMat4("projMatrix", m_ProjMatrix);

        // Bind the vertex array
        vao->Bind();

        // Draw element
        glDrawElements(GL_TRIANGLES, ebo->GetCount(), GL_UNSIGNED_INT, nullptr);

        // Undind resources
        vao->Unbind();
        shader->Unbind();

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