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
    Viewport(const glm::ivec2& renderSize, const glm::vec2& pixelSize, const glm::ivec2& windowOffset, const glm::vec3& forward, const float zLevel);

    void onScroll(const float yOffset);
    const ViewportFramebuffer& getFrameBuffer() const
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
    glm::ivec2 getWindowOffset() const
    {
        return m_WindowOffset;
    }
    float getPixelWidth() const
    {
        return m_PixelWidth;
    }
    float getPixelHeight() const
    {
        return m_PixelHeight;
    }
    uint32_t getRenderWidth() const
    {
        return m_RenderWidth;
    }
    uint32_t getRenderHeight() const
    {
        return m_RenderHeight;
    }

private:
    const uint32_t m_RenderWidth;
    const uint32_t m_RenderHeight;
    const float m_PixelWidth;
    const float m_PixelHeight;
    const glm::ivec2 m_WindowOffset;

    const ViewportFramebuffer m_BufferViewport;

    glm::vec3 m_Forward;
    float m_zLevel;
};