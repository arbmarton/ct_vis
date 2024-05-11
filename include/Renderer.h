#pragma once

#include "glad/glad.h"

#include <cinttypes>

class Renderer
{
public:
    static Renderer& instance();

    constexpr static uint32_t RENDER_WIDTH = 512 * 2;
    constexpr static uint32_t RENDER_HEIGHT = 512 * 2;

    void draw() const;
    void onScroll(const float yOffset);

    void setTexture(const GLuint texture)
    {
        m_Texture = texture;
    }

private:
    Renderer();
    constexpr static float m_QuadVertices[] = {
        // positions + texcoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;

    GLuint m_Texture = 0;

    float m_zLevel = 0.0f;
};