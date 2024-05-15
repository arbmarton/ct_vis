#include "Viewport.h"

Viewport::Viewport(const glm::ivec2& renderSize, const glm::vec2& pixelSize, const glm::ivec2& windowOffset, const glm::vec3& forward, const float zLevel)
    : m_RenderWidth(renderSize[0])
    , m_RenderHeight(renderSize[1])
    , m_PixelWidth(pixelSize[0])
    , m_PixelHeight(pixelSize[1])
    , m_WindowOffset(windowOffset)
    , m_BufferViewport(ViewportFramebuffer(renderSize[0], renderSize[1]))
    , m_Forward(forward)
    , m_zLevel(zLevel)
{
}

void Viewport::onScroll(const float yOffset)
{
    m_zLevel += float(yOffset * 0.01);
    m_zLevel = glm::clamp(m_zLevel, 0.0f, 1.0f);
}