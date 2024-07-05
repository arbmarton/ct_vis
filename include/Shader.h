#pragma once

#include "Enums.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <filesystem>
#include <unordered_map>
#include <string_view>

#ifdef QT_BUILD
#include <QOpenGlFunctions>
#else
#include <glad/glad.h>
#endif

struct TextureUploadHelper
{
    TextureType m_Type;
    std::string m_TypeAsString;
    GLuint m_DefaultTextureID;
};

class Shader
#ifdef QT_BUILD
    : protected QOpenGLFunctions
#endif
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

    void use();

    void setBool(const std::string& name, const bool value);
    void setInt(const std::string& name, const int value);
    void setFloat(const std::string& name, const float value);
    void setFloatArray(const std::string& name, const size_t count, const float* value);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setMat4Array(const std::string& name, const std::vector<glm::mat4>& vec);
    void setVec4(const std::string& name, const float x, const float y, const float z, const float w);
    void setVec4(const std::string& name, const glm::vec4& vec);
    void setVec3(const std::string& name, const float x, const float y, const float z);
    void setVec3(const std::string& name, const glm::vec3& vec);
    void setVec2(const std::string& name, const float x, const float y);
    void setVec2(const std::string& name, const glm::vec2& vec);

    virtual void setValues() = 0;

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

    void setValues() override
    {
    }
};

class CtViewportShader : public Shader
{
public:
    CtViewportShader()
        : Shader(ShaderType::CtViewport)
    {
    }

    void setValues() override;
};

class CtViewportPostprocessShader : public Shader
{
public:
    CtViewportPostprocessShader()
        : Shader(ShaderType::CtViewportPostprocess)
    {
    }

    void setValues() override;
};

class MainViewportShader : public Shader
{
public:
    MainViewportShader()
        : Shader(ShaderType::MainViewport)
    {
    }

    void setValues() override;
};

class GaussianBlurShader : public Shader
{
public:
    GaussianBlurShader()
        : Shader(ShaderType::GaussianBlur)
    {
    }

    void setValues() override;
};