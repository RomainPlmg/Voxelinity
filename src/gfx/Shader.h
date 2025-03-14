#ifndef __SHADER_H__
#define __SHADER_H__

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Buffer.h"

typedef unsigned int GLenum;

struct Shader {
    GLenum type;
    std::string file;
};

using UniformType = std::variant<bool, int, float, glm::vec3, glm::mat4>;

struct Uniform {
    const ShaderDataType type;
    UniformType value;

    Uniform(const std::string name, bool v) : type(ShaderDataType::Bool), value(v) {}
    Uniform(const std::string name, int v) : type(ShaderDataType::Int), value(v) {}
    Uniform(const std::string name, float v) : type(ShaderDataType::Float), value(v) {}
    Uniform(const std::string name, const glm::vec3& v) : type(ShaderDataType::Float3), value(v) {}
    Uniform(const std::string name, const glm::mat4& v) : type(ShaderDataType::Mat4), value(v) {}

    template <typename T>
    T GetValue() const {
        return std::get<T>(value);
    }

    template <typename T>
    void SetValue(const T& newValue) {
        value = newValue;
    }
};

class ShaderProgram {
   public:
    ShaderProgram(const std::string& name, const std::vector<Shader>& shaders, const std::vector<BufferElement>& bufferElements);
    ~ShaderProgram();

    void Bind() const;
    void Unbind() const;

    /* Utility uniform functions */
    void SetUniformBool(const std::string& name, bool value) const;
    void SetUniformInt(const std::string& name, int value) const;
    void SetUniformIntArray(const std::string& name, int* values, uint32_t count) const;
    void SetUniformFloat(const std::string& name, float value) const;
    void SetUniformFloat2(const std::string& name, const glm::vec2& value) const;
    void SetUniformFloat3(const std::string& name, const glm::vec3& value) const;
    void SetUniformFloat4(const std::string& name, const glm::vec4& value) const;
    void SetUniformMat4(const std::string& name, const glm::mat4& value) const;

    /* Getters */
    inline const std::string& GetName() const { return m_Name; }
    inline std::shared_ptr<BufferLayout> GetBufferLayout() const { return m_BufferLayout; }
    Uniform* GetUniform(const std::string& name) { return m_Uniforms[name].get(); }
    const std::unordered_map<std::string, std::unique_ptr<Uniform>>& GetUniforms() const { return m_Uniforms; }

    static GLenum ShaderTypeFromString(const std::string& typeStr);
    static std::shared_ptr<ShaderProgram> Create(const std::string& name, const std::vector<Shader>& shaders,
                                                 const std::vector<BufferElement>& bufferElements);

    /* Setters */
    void NewUniform(const std::string& name, ShaderDataType type);

   private:
    uint32_t m_RendererID;
    std::string m_Name;
    std::shared_ptr<BufferLayout> m_BufferLayout;
    std::unordered_map<std::string, std::unique_ptr<Uniform>> m_Uniforms;

    static uint32_t CompileShader(const Shader& shader);
};

class ShaderProgramLibrary {
   public:
    ShaderProgramLibrary(const ShaderProgramLibrary&) = delete;
    ShaderProgramLibrary& operator=(const ShaderProgramLibrary&) = delete;

    static void Init(const std::string& JSONPath);
    std::shared_ptr<ShaderProgram> GetShaderProgram(const std::string& name);
    static void Shutdown();

    static ShaderProgramLibrary& Get();

   private:
    ShaderProgramLibrary(const std::string& JSONPath);
    ~ShaderProgramLibrary();
    std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_Shaders;
};

#endif  // __SHADER_H__