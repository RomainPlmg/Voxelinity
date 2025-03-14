#include "GraphicContext.h"

#include "Shader.h"
#include "pch.h"
#include "utils/Logger.h"

void APIENTRY glDebugOutputCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                                    const void* userParam) {
    // Ignore bullshit messages
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::stringstream logMsg;

    logMsg << "---------------" << "\n";
    logMsg << "Debug message (" << id << "): " << message << "\n";

    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB:
            logMsg << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
            logMsg << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
            logMsg << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
            logMsg << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
            logMsg << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER_ARB:
            logMsg << "Source: Other";
            break;
    }
    logMsg << "\n";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB:
            logMsg << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            logMsg << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            logMsg << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
            logMsg << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            logMsg << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_OTHER_ARB:
            logMsg << "Type: Other";
            break;
    }
    logMsg << "\n\n";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
            LOG_FATAL(logMsg.str());
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
            LOG_ERROR(logMsg.str());
            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:
            LOG_WARNING(logMsg.str());
            break;
    }
}

GraphicContext::GraphicContext(GLFWwindow* windowHandler) : m_Handler(windowHandler) {}

GraphicContext::~GraphicContext() {}

void GraphicContext::Init() {
    // Make the window's context current
    glfwMakeContextCurrent(m_Handler);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_FATAL("Failed to initialize GLAD.");
    }  // Initialized glad

#ifndef _NDEBUG
    if (GLAD_GL_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB(glDebugOutputCallBack, nullptr);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    } else {
        LOG_WARNING("GL_ARB_debug_output is NOT supported");
    }
#endif

    ShaderProgramLibrary::Init(ASSET_DIRECTORY "shaders/shaders.json");
}

std::unique_ptr<GraphicContext> GraphicContext::Create(GLFWwindow* windowHandler) { return std::make_unique<GraphicContext>(windowHandler); }
