#include "Shader.h"

#include "Utilities.h"
#include "OpenGL.h"
#include "OpenGLMutex.h"
#include "Renderer.h"

#define GLM_ENABLE_EXPERIMENTAL

#pragma warning(push)
#pragma warning(disable : 4201)
#pragma warning(disable : 4217)
#include "gtc/type_ptr.hpp"
#include "gtx/string_cast.hpp"
#pragma warning(pop)

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

Shader::Shader(const ShaderType type)
    : m_Type(type)
{
    const std::string shaderName = shaderTypeToString(type);

    const auto vertex = utils::getShaderPath(shaderName).string().append(".vs");
    const auto fragment = utils::getShaderPath(shaderName).string().append(".fs");

    if (std::filesystem::exists(utils::getShaderPath(shaderName).string().append(".gs")))
    {
        loadInternal(vertex, fragment, utils::getShaderPath(shaderName).string().append(".gs"));
    }
    else
    {
        loadInternal(vertex, fragment);
    }
}

Shader::~Shader() = default;

void Shader::loadInternal(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::filesystem::path& geometryPath)
{
    OpenGLLockGuard lock;

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    const auto lambda = [](const std::filesystem::path& path) -> std::string {
        try
        {
            std::ifstream istream;

            istream.open(path.string());
            std::stringstream ss;
            ss << istream.rdbuf();
            istream.close();

            // TODO: temporary fix for the mysterious garbage values in the fragment shader code of fxaa...
            auto temp = ss.str();
            while (temp[0] < 0)
            {
                temp.erase(temp.begin());
            }
            return temp;
        } catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            return "";
        }
    };

    vertexCode = lambda(vertexPath);
    fragmentCode = lambda(fragmentPath);
    if (geometryPath != "")
    {
        geometryCode = lambda(geometryPath);
    }

    GLuint vertex, fragment;
    GLuint geometry = std::numeric_limits<GLuint>::max();
    int success;
    char infoLog[512];

    const char* vertexCodeChar = vertexCode.c_str();
    const char* fragmentCodeChar = fragmentCode.c_str();
    const char* geometryCodeChar = geometryCode.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCodeChar, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::exception();
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCodeChar, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::exception();
    };

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    if (geometryCode != "")
    {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometryCodeChar, nullptr);
        glCompileShader(geometry);

        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometry, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            throw std::exception();
        };

        glAttachShader(m_ID, geometry);
    }
    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        throw std::exception();
    };

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (geometryCode != "")
    {
        glDeleteShader(geometry);
    }
}

std::string Shader::getShaderVariableFileContent() const
{
    std::ifstream istream;

    istream.open(utils::getShaderFolderPath().string().append("/shader_vars.txt"));
    std::stringstream ss;
    ss << istream.rdbuf();
    istream.close();

    return ss.str();
}

std::unordered_map<std::string, std::string> Shader::getShaderVariables() const
{
    static const std::string content = getShaderVariableFileContent();

    std::unordered_map<std::string, std::string> ret;
    const auto lines = utils::splitString(content, '\n');
    const std::string name = "-" + shaderTypeToString(m_Type);
    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (lines[i] != name)
        {
            continue;
        }
        else
        {
            ++i;
            while (i < lines.size() && lines[i] != "")
            {
                const auto splitLine = utils::splitString(lines[i], '=');
                ret[splitLine[0]] = splitLine[1];
                ++i;
            }
            break;
        }
    }

    return ret;
}

void Shader::setBool(const std::string& name, const bool value) const
{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setFloatArray(const std::string& name, const size_t count, const float* value) const
{
    glUniform1fv(glGetUniformLocation(m_ID, name.c_str()), int(count), value);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4Array(const std::string& name, const std::vector<glm::mat4>& vec) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), int(vec.size()), GL_FALSE, glm::value_ptr(vec[0]));
}

void Shader::setVec4(const std::string& name, const float x, const float y, const float z, const float w) const
{
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);
}

void Shader::setVec4(const std::string& name, const glm::vec4& vec) const
{
    glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vec[0]);
}

void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const
{
    glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const
{
    glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vec[0]);
}

void Shader::setVec2(const std::string& name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
}

void Shader::setVec2(const std::string& name, const glm::vec2& vec) const
{
    glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vec[0]);
}

void CtViewportShader::setValues() const
{
    setInt("texture3D", 0);
    setVec3("upVector", Renderer::UP_DIR);
}

void CtViewportPostprocessShader::setValues() const
{
    setInt("textureInput", 0);
    setVec3("upVector", Renderer::UP_DIR);
}

void MainViewportShader::setValues() const
{
    setInt("viewport1", 0);
    setInt("viewport2", 1);
    setInt("viewport3", 2);
}

void GaussianBlurShader::setValues() const
{
    setInt("screenTexture", 0);
}