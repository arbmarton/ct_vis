#include "Framebuffer.h"

#include "OpenGLMutex.h"

#include <iostream>

Framebuffer::Framebuffer(const uint32_t width, const uint32_t height)
    : IFramebuffer()
{
#ifdef QT_BUILD
    initializeOpenGLFunctions();
#endif
    OpenGLLockGuard lock;

    m_Width = width;
    m_Height = height;
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    glGenTextures(1, &m_TexColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TexColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexColorBuffer, 0);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw std::exception();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FloatFramebuffer::FloatFramebuffer(const uint32_t width, const uint32_t height)
    : IFramebuffer()
{
#ifdef QT_BUILD
    initializeOpenGLFunctions();
#endif
    OpenGLLockGuard lock;

    m_Width = width;
    m_Height = height;
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    glGenTextures(1, &m_TexColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TexColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexColorBuffer, 0);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw std::exception();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ViewportFramebuffer::ViewportFramebuffer(const uint32_t width, const uint32_t height)
    : IFramebuffer()
{
#ifdef QT_BUILD
    initializeOpenGLFunctions();
#endif
    OpenGLLockGuard lock;

    m_Width = width;
    m_Height = height;
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    glGenTextures(2, m_TexColorBuffers);
    glBindTexture(GL_TEXTURE_2D, m_TexColorBuffers[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexColorBuffers[0], 0);

    glBindTexture(GL_TEXTURE_2D, m_TexColorBuffers[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_TexColorBuffers[1], 0);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw std::exception();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
