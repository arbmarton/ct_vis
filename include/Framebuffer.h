#pragma once

#include "glad/glad.h"

#include <cinttypes>

struct Framebuffer
{
    Framebuffer(const uint32_t width, const uint32_t height);
    Framebuffer(const Framebuffer& rhs) = delete;
    Framebuffer(Framebuffer&& rhs) = delete;
    Framebuffer& operator=(const Framebuffer& rhs) = delete;
    Framebuffer& operator=(Framebuffer&& rhs) = delete;

    uint32_t m_Width;
    uint32_t m_Height;

    GLuint m_FrameBuffer;
    GLuint m_RBO;
    GLuint m_TexColorBuffer;
};