#include "Viewport.h"

Viewport::Viewport(const uint32_t width, const uint32_t height, const glm::vec3& forward, const float zLevel)
    : m_Width(width)
    , m_Height(height)
    , m_BufferViewport(Framebuffer(width, height))
    , m_Forward(forward)
    , m_zLevel(zLevel)
{
}

void Viewport::onScroll(const float yOffset)
{
    m_zLevel += float(yOffset * 0.01);
    m_zLevel = glm::clamp(m_zLevel, 0.0f, 1.0f);
}