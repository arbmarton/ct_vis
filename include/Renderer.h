#pragma once

#include "glad/glad.h"

#include <cinttypes>

class Renderer
{
public:
    Renderer();

    constexpr static uint32_t RENDER_WIDTH = 512 * 2;
    constexpr static uint32_t RENDER_HEIGHT = 512 * 2;

    GLuint testTexture;


    void draw() const;

private:
    constexpr static float m_QuadVertices[] = {
        // positions + texcoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;
};