#pragma once

#ifdef QT_BUILD
#include <QOpenGlFunctions>
#include <QOpenGlExtraFunctions>
#else
#pragma warning(push)
#pragma warning(disable : 4005)
#include "glad/glad.h"
#pragma warning(pop)
#endif

#include <cinttypes>


struct IFramebuffer
#ifdef QT_BUILD
    : protected QOpenGLExtraFunctions
#endif
{
    uint32_t m_Width;
    uint32_t m_Height;

    GLuint m_FrameBuffer;
    GLuint m_RBO;
};

struct Framebuffer : public IFramebuffer
{
    Framebuffer(const uint32_t width, const uint32_t height);
    Framebuffer(const Framebuffer& rhs) = delete;
    Framebuffer(Framebuffer&& rhs) = delete;
    Framebuffer& operator=(const Framebuffer& rhs) = delete;
    Framebuffer& operator=(Framebuffer&& rhs) = delete;

    GLuint m_TexColorBuffer;
};

struct FloatFramebuffer : public IFramebuffer
{
    FloatFramebuffer(const uint32_t width, const uint32_t height);
    FloatFramebuffer(const FloatFramebuffer& rhs) = delete;
    FloatFramebuffer(FloatFramebuffer&& rhs) = delete;
    FloatFramebuffer& operator=(const FloatFramebuffer& rhs) = delete;
    FloatFramebuffer& operator=(FloatFramebuffer&& rhs) = delete;

    GLuint m_TexColorBuffer;
};

struct ViewportFramebuffer : public IFramebuffer
{
    ViewportFramebuffer(const uint32_t width, const uint32_t height);
    ViewportFramebuffer(const ViewportFramebuffer& rhs) = delete;
    ViewportFramebuffer(ViewportFramebuffer&& rhs) = delete;
    ViewportFramebuffer& operator=(const ViewportFramebuffer& rhs) = delete;
    ViewportFramebuffer& operator=(ViewportFramebuffer&& rhs) = delete;

    GLuint m_TexColorBuffers[2];
};