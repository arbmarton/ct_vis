#include "Viewport.h"

#include "Globals.h"
#include "Renderer.h"
#include "Utilities.h"
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
        m_fov += yOffset * 0.02f * -1.0f;
        m_fov = glm::clamp(m_fov, 0.5f, 1.5f);
    }
    else
    {
        m_zLevel += yOffset * 0.01f;
        m_zLevel = glm::clamp(m_zLevel, 0.0f, 1.0f);
    }
}

void Viewport::onMouseMove(const float xOffset, const float yOffset)
{
    if (glfwGetMouseButton(Globals::instance().getOpenGLContext(), GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
    {
        return;
    }

    constexpr float speed = 0.001f;
    const auto right = glm::normalize(glm::cross(m_Forward, Renderer::UP_DIR));
    const auto up = glm::normalize(glm::cross(right, m_Forward));

    const auto offset = xOffset * right * speed * -1.0f + yOffset * up * speed * -1.0f;

    m_CenterOffset += offset;

    m_CenterOffset = glm::clamp(m_CenterOffset, -0.5f, 0.5f);
}