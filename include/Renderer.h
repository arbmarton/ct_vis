#pragma once

#include "Framebuffer.h"

#include "glad/glad.h"

#include <cinttypes>
#include <limits>

class Renderer
{
public:
    static Renderer& instance();

    constexpr static uint32_t RENDER_WIDTH = 512 * 3;
    constexpr static uint32_t RENDER_HEIGHT = 512 * 3;

    void draw() const;
    void onScroll(const float yOffset);

    void set3DTexture(const GLuint texture)
    {
        m_3DTexture = texture;
    }

private:
    Renderer();
    constexpr static float m_QuadVertices[] = {
        // positions + texcoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;

    Framebuffer m_BufferViewport1 = Framebuffer(512, 512);
    Framebuffer m_BufferViewport2 = Framebuffer(512, 512);
    Framebuffer m_BufferViewport3 = Framebuffer(512, 512);

#undef max  // https://stackoverflow.com/questions/1394132/macro-and-member-function-conflict
    GLuint m_3DTexture = std::numeric_limits<GLuint>::max();

    float m_zLevel = 0.0f;
};