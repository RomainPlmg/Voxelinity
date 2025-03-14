#include "Shader.h"

#include <nlohmann/json.hpp>

#include "Buffer.h"
#include "GraphicContext.h"
#include "pch.h"
#include "utils/File.h"
#include "utils/Logger.h"

using json = nlohmann::json;

/* Shader */
ShaderProgram::ShaderProgram(const std::string& name, const std::vector<Shader>& shaders, const std::vector<BufferElement>& bufferElements)
    : m_Name(name) {
    m_RendererID = glCreateProgram();

    const auto shaderIDs = new uint32_t[shaders.size()];
    for (uint32_t i = 0; i < shaders.size(); i++) {
        shaderIDs[i] = CompileShader(shaders[i]);
        glAttachShader(m_RendererID, shaderIDs[i]);
    }

    glLinkProgram(m_RendererID);
    glValidateProgram(m_RendererID);

    for (uint32_t i = 0; i < shaders.size(); i++) {
        glDeleteShader(shaderIDs[i]);
    }

    m_BufferLayout = std::make_shared<BufferLayout>(bufferElements);

    delete[] shaderIDs;
    LOG_INFO("Shader '{0}' build succesful", name);
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(m_RendererID); }

void ShaderProgram::Bind() const { glUseProgram(m_RendererID); }

void ShaderProgram::Unbind() const { glUseProgram(0); }

void ShaderProgram::SetUniformBool(const std::string& name, bool value) const {
    const GLint uniformLoc = glGetUniformLocation(m_RendererID, name.c_str());
    if (uniformLoc == -1) {
        LOG_WARNING("Warning: Unable to find the uniform \'{0}\'", name);
    }
    glUniform1i(uniformLoc, value);
}

void ShaderProgram::SetUniformInt(const std::string& name, const int value) const {
    const GLint uniformLoc = glGetUniformLocation(m_RendererID, name.c_str());
    if (uniformLoc == -1) {
        LOG_WARNING("Warning: Unable to find the uniform \'{0}\'", name);
    }
    glUniform1i(uniformLoc, value);
}

void ShaderProgram::SetUniformIntArray(const std::string& name, int* values, uint32_t count) const {}

void ShaderProgram::SetUniformFloat(const std::string& name, const float value) const {
    const GLint uniformLoc = glGetUniformLocation(m_RendererID, name.c_str());
    if (uniformLoc == -1) {
        LOG_WARNING("Warning: Unable to find the uniform \'{0}\'", name);
    }
    glUniform1f(uniformLoc, value);
}

void ShaderProgram::SetUniformFloat2(const std::string& name, const glm::vec2& value) const {}

void ShaderProgram::SetUniformFloat3(const std::string& name, const glm::vec3& value) const {
    const GLint uniformLoc = glGetUniformLocation(m_RendererID, name.c_str());

    if (uniformLoc == -1) {
        LOG_WARNING("Warning: Unable to find the uniform \'{0}\'", name);
    }
    glUniform3f(uniformLoc, value.x, value.y, value.z);
}

void ShaderProgram::SetUniformFloat4(const std::string& name, const glm::vec4& value) const {}

void ShaderProgram::SetUniformMat4(const std::string& name, const glm::mat4& value) const {
    const GLint uniformLoc = glGetUniformLocation(m_RendererID, name.c_str());
    if (uniformLoc == -1) {
        LOG_WARNING("Warning: Unable to find the uniform \'{0}\'", name);
    }
    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

GLenum ShaderProgram::ShaderTypeFromString(const std::string& typeStr) {
    if (typeStr == "vertex") return GL_VERTEX_SHADER;
    if (typeStr == "fragment") return GL_FRAGMENT_SHADER;
    if (typeStr == "geometry") return GL_GEOMETRY_SHADER;
    LOG_ERROR("Unknown shader type '{0}'", typeStr);
    return GL_NONE;
}

std::shared_ptr<ShaderProgram> ShaderProgram::Create(const std::string& name, const std::vector<Shader>& shaders,
                                                     const std::vector<BufferElement>& bufferElements) {
    return std::make_shared<ShaderProgram>(name, shaders, bufferElements);
}

void ShaderProgram::NewUniform(const std::string& name, ShaderDataType type) {
    if (type == ShaderDataType::Bool) {
        m_Uniforms[name] = std::make_unique<Uniform>(name, false);
    } else if (type == ShaderDataType::Int) {
        m_Uniforms[name] = std::make_unique<Uniform>(name, 0);
    } else if (type == ShaderDataType::Float) {
        m_Uniforms[name] = std::make_unique<Uniform>(name, 0.0f);
    } else if (type == ShaderDataType::Float3) {
        m_Uniforms[name] = std::make_unique<Uniform>(name, glm::vec3(0.0f));
    } else if (type == ShaderDataType::Mat4) {
        m_Uniforms[name] = std::make_unique<Uniform>(name, glm::mat4(0.0f));
    } else {
        LOG_ERROR("Unknown shader data type");
        return;
    }
}

uint32_t ShaderProgram::CompileShader(const Shader& shader) {
    const uint32_t id = glCreateShader(shader.type);
    const std::string srcString = File::ReadFromFile(shader.file);
    const char* src = srcString.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Print compile errors if any
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        auto message = new char[length];
        glGetShaderInfoLog(id, length, nullptr, message);
        std::string glType;
        switch (shader.type) {
            case GL_VERTEX_SHADER:
                glType = "VERTEX";
                break;
            case GL_FRAGMENT_SHADER:
                glType = "FRAGMENT";
                break;
            case GL_GEOMETRY_SHADER:
                glType = "GEOMETRY";
                break;
            default:
                glType = "UNKNOWN";
                break;
        }
        LOG_FATAL("ERROR::Shader::{0}::COMPILATION_FAILED for '{1}'\n {2}", glType, shader.file, message);
        glDeleteShader(id);

        delete[] message;
        return 0;
    }
    return id;
}

/* ShaderLibrary */

/*static*/ ShaderProgramLibrary* s_ShaderProgramLibInst = nullptr;

void ShaderProgramLibrary::Init(const std::string& JSONPath) { s_ShaderProgramLibInst = new ShaderProgramLibrary(JSONPath); }

std::shared_ptr<ShaderProgram> ShaderProgramLibrary::GetShaderProgram(const std::string& name) {
    if (m_Shaders.contains(name)) {
        return m_Shaders[name];
    }
    LOG_ERROR("Shader program '{0}' not found", name);
    return nullptr;
}

void ShaderProgramLibrary::Shutdown() {
    delete s_ShaderProgramLibInst;
    s_ShaderProgramLibInst = nullptr;
}

ShaderProgramLibrary& ShaderProgramLibrary::Get() {
    assert(s_ShaderProgramLibInst != nullptr);
    return *s_ShaderProgramLibInst;
}

ShaderProgramLibrary::ShaderProgramLibrary(const std::string& JSONPath) {
    const std::filesystem::path jsonSysPath = JSONPath;
    const std::string shadersDir = jsonSysPath.parent_path().string() + '/';

    auto jsonFile = File::LoadFile(JSONPath);
    json data = json::parse(*jsonFile);

    // Useful variables
    std::string name;
    std::vector<Shader> shaders;
    std::vector<BufferElement> bufferElements;

    if (!data.contains("shader_programs")) {
        LOG_FATAL("Failed to load shader programs from JSON file '{0}'", JSONPath);
    }
    for (const auto& shaderProgram : data["shader_programs"]) {
        // Recover shader name
        if (!shaderProgram.contains("name")) {
            LOG_ERROR("Missing shader name in '{0}'", JSONPath);
            goto continue_main_loop;
        }
        name = shaderProgram["name"];

        // Recover shader files
        if (!shaderProgram.contains("shaders")) {
            LOG_ERROR("Shader files not specified in '{0}' for shader {1}", JSONPath, name);
            goto continue_main_loop;
        }
        for (const auto& shaderInfo : shaderProgram["shaders"]) {
            Shader sh;
            if (!shaderInfo.contains("type") || !shaderInfo.contains("file")) {
                LOG_ERROR("Cannot load shader '{0}', missing 'shaders' info in '{1}'", name, JSONPath);
                goto continue_main_loop;
            }
            sh.type = ShaderProgram::ShaderTypeFromString(shaderInfo["type"]);
            sh.file = shadersDir + shaderInfo["file"].get<std::string>();
            if (sh.type == GL_NONE || sh.file.empty()) {
                LOG_ERROR("Invalid type for the shader file '{0}', in '{1}'", sh.file, JSONPath);
            } else {
                shaders.push_back(sh);
            }
        }

        // Recover shader layout
        if (!shaderProgram.contains("layout")) {
            LOG_ERROR("Missing 'layout' attribute for the shader '{0}' in '{1}'", name, JSONPath);
            shaders.pop_back();  // Remove the shader from the shader vector
            goto continue_main_loop;
        }

        for (const auto& bufferElement : shaderProgram["layout"]) {
            if (!bufferElement.contains("type") || !bufferElement.contains("name") || !bufferElement.contains("normalized")) {
                LOG_ERROR("Cannot load shader '{0}', missing layout info the file '{1}'", name, JSONPath);
                shaders.pop_back();  // Remove the shader from the shader vector
                goto continue_main_loop;
            }
            const std::string typeStr = bufferElement["type"].get<std::string>();
            const std::string elementName = bufferElement["name"].get<std::string>();
            const bool normalized = bufferElement["normalized"].get<bool>();

            ShaderDataType dataType;
            if (typeStr == "bool") {
                dataType = ShaderDataType::Bool;
            } else if (typeStr == "int") {
                dataType = ShaderDataType::Int;
            } else if (typeStr == "float") {
                dataType = ShaderDataType::Float;
            } else if (typeStr == "float3") {
                dataType = ShaderDataType::Float3;
            } else if (typeStr == "mat4") {
                dataType = ShaderDataType::Mat4;
            } else {
                LOG_FATAL("Unknown shader data type '{0}'", typeStr);
            }

            bufferElements.emplace_back(BufferElement(dataType, elementName, normalized));
        }

        m_Shaders[name] = ShaderProgram::Create(name, shaders, bufferElements);

        // Recover shader uniforms
        if (!shaderProgram.contains("uniforms")) {
            LOG_ERROR("Missing 'uniforms' attribute for the shader '{0}' in '{1}'", name, JSONPath);
            shaders.pop_back();  // Remove the shader from the shader vector
            goto continue_main_loop;
        }

        for (const auto& uniform : shaderProgram["uniforms"]) {
            if (!uniform.contains("type") || !uniform.contains("name")) {
                LOG_ERROR("Cannot load shader '{0}', missing uniforms info the file '{1}'", name, JSONPath);
                shaders.pop_back();  // Remove the shader from the shader vector
                goto continue_main_loop;
            }

            const std::string typeStr = uniform["type"].get<std::string>();
            const std::string uniformName = uniform["name"].get<std::string>();

            ShaderDataType dataType;
            if (typeStr == "bool") {
                m_Shaders[name]->NewUniform(uniformName, ShaderDataType::Bool);
            } else if (typeStr == "int") {
                m_Shaders[name]->NewUniform(uniformName, ShaderDataType::Int);
            } else if (typeStr == "float") {
                m_Shaders[name]->NewUniform(uniformName, ShaderDataType::Float);
            } else if (typeStr == "float3") {
                m_Shaders[name]->NewUniform(uniformName, ShaderDataType::Float3);
            } else if (typeStr == "mat4") {
                m_Shaders[name]->NewUniform(uniformName, ShaderDataType::Mat4);
            } else {
                LOG_FATAL("Unknown shader data type '{0}'", typeStr);
            }
        }

    continue_main_loop:
        continue;
    }
}

ShaderProgramLibrary::~ShaderProgramLibrary() {
    for (auto& shader : m_Shaders) {
        shader.second.reset();
    }
    m_Shaders.clear();
}
