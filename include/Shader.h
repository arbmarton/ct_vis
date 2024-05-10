#pragma once

#include "Enums.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <glad/glad.h>

#include <filesystem>
#include <unordered_map>
#include <string_view>

struct TextureUploadHelper
{
    TextureType m_Type;
    std::string m_TypeAsString;
    GLuint m_DefaultTextureID;
};

class Shader
{
public:
    Shader(const ShaderType type);
    virtual ~Shader();

    ShaderType getType() const
    {
        return m_Type;
    }

    GLuint getID() const
    {
        return m_ID;
    }

    const std::vector<TextureUploadHelper>& getTexturesToUpload() const
    {
        return m_TexturesToUpload;
    }

    void use() const
    {
        glUseProgram(m_ID);
        setValues();
    }

    void setBool(const std::string& name, const bool value) const;
    void setInt(const std::string& name, const int value) const;
    void setFloat(const std::string& name, const float value) const;
    void setFloatArray(const std::string& name, const size_t count, float* value) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setMat4Array(const std::string& name, const std::vector<glm::mat4>& vec) const;
    void setVec4(const std::string& name, const float x, const float y, const float z, const float w) const;
    void setVec4(const std::string& name, const glm::vec4& vec) const;
    void setVec3(const std::string& name, const float x, const float y, const float z) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setVec2(const std::string& name, const float x, const float y) const;
    void setVec2(const std::string& name, const glm::vec2& vec) const;

    virtual void setValues() const = 0;

protected:
    void loadInternal(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::filesystem::path& geometryPath = "");

    ShaderType m_Type;
    GLuint m_ID;

    Shader() = delete;
    Shader(const Shader& rhs) = delete;
    Shader(Shader&& rhs) = delete;

    Shader& operator=(const Shader& rhs) = delete;
    Shader& operator=(const Shader&& rhs) = delete;

    std::string getShaderVariableFileContent() const;
    std::unordered_map<std::string, std::string> getShaderVariables() const;

    std::vector<TextureUploadHelper> m_TexturesToUpload;
};

class BasicShader : public Shader
{
public:
    BasicShader()
        : Shader(ShaderType::Basic)
    {
    }

    void setValues() const override
    {
    }
};

class QuadShader : public Shader
{
public:
    QuadShader()
        : Shader(ShaderType::Quad)
    {
    }

    void setValues() const override;
};
