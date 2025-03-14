#include "GraphicStateGuard.h"

void GraphicStateGuard::Save() {
    // Save currents OpenGL states
    glGetBooleanv(GL_DEPTH_TEST, &m_DepthTestEnabled);
    glGetBooleanv(GL_BLEND, &m_BlendEnabled);
    glGetBooleanv(GL_LINE_SMOOTH, &m_LineSmoothEnabled);
    glGetBooleanv(GL_CULL_FACE, &m_FaceCullingEnabled);
    glGetBooleanv(GL_MULTISAMPLE, &m_MultiSample);
}

void GraphicStateGuard::Restore() {
    // Restore OpenGL states
    SetGLState(GL_DEPTH_TEST, m_DepthTestEnabled);
    SetGLState(GL_BLEND, m_BlendEnabled);
    SetGLState(GL_LINE_SMOOTH, m_LineSmoothEnabled);
    SetGLState(GL_CULL_FACE, m_FaceCullingEnabled);
    SetGLState(GL_MULTISAMPLE, m_MultiSample);
}