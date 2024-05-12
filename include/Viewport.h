#pragma once

#include "Framebuffer.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm.hpp>
#pragma warning(pop)

#include <cinttypes>

class Viewport
{
public:
    Viewport(const uint32_t width, const uint32_t height, const glm::vec3& forward, const float zLevel);

    void onScroll(const float yOffset);
    const Framebuffer& getFrameBuffer() const
    {
        return m_BufferViewport;
    }
    glm::vec3 getForward() const
    {
        return m_Forward;
    }
    float getZLevel() const
    {
        return m_zLevel;
    }

private:
    const uint32_t m_Width;
    const uint32_t m_Height;
    const Framebuffer m_BufferViewport;

    glm::vec3 m_Forward;
    float m_zLevel;
};