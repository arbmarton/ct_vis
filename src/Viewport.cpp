#include "Viewport.h"

#include "Globals.h"
#include <GLFW/glfw3.h>

Viewport::Viewport(
    const glm::ivec2& renderSize,
    const glm::vec2& pixelSize,
    const glm::ivec2& windowOffset,
    const glm::vec3& forward,
    const float zLevel,
    const glm::vec3& color)
    : m_RenderWidth(renderSize[0])
    , m_RenderHeight(renderSize[1])
    , m_PixelWidth(pixelSize[0])
    , m_PixelHeight(pixelSize[1])
    , m_WindowOffset(windowOffset)
    , m_Databuffer(FloatFramebuffer(renderSize[0], renderSize[1]))
    , m_Colorbuffer(Framebuffer(renderSize[0], renderSize[1]))
    , m_Forward(forward)
    , m_zLevel(zLevel)
    , m_Color(color)
{
}

void Viewport::onScroll(const float yOffset)
{
    if (glfwGetKey(Globals::instance().getOpenGLContext(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        m_fov += yOffset * 0.01f * -1.0f;
        m_fov = glm::clamp(m_fov, 0.5f, 1.5f);
    }
    else
    {
        m_zLevel += yOffset * 0.01f;
        m_zLevel = glm::clamp(m_zLevel, 0.0f, 1.0f);
    }
}