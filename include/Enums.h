#pragma once

#include <string>

enum class TextureType
{
    none,
    texture_specular,
    texture_diffuse,
    texture_normal,
    texture_displacement,
    texture_roughness,
    texture_ambient_occlusion,
    texture_metalness
};

inline std::string textureTypeToString(const TextureType type)
{
    switch (type)
    {
        case TextureType::texture_diffuse:
            return "texture_diffuse";
        case TextureType::texture_specular:
            return "texture_specular";
        case TextureType::texture_normal:
            return "texture_normal";
        case TextureType::texture_displacement:
            return "texture_displacement";
        case TextureType::texture_roughness:
            return "texture_roughness";
        case TextureType::texture_ambient_occlusion:
            return "texture_ambient_occlusion";
        case TextureType::texture_metalness:
            return "texture_metalness";
    }

    throw std::exception();
}

enum class ShaderType
{
    Basic,
    Quad,
    None
};

inline std::string shaderTypeToString(const ShaderType type)
{
    switch (type)
    {
        case ShaderType::Basic:
            return "basic";
        case ShaderType::Quad:
            return "quad";
    }

    throw std::exception();
}

enum class Direction
{
    X,
    Y,
    Z,
    None,
};

enum class RotationPlane
{
    XY,
    YZ,
    XZ,
    None
};