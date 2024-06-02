#include "Viewport.h"

#include "Globals.h"
#include "Renderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/transform.hpp"

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
    , m_zLevel(zLevel)
    , m_Color(color)
{
    m_RotationVertical = asin(forward.y * -1.0f);
    m_RotationVertical = glm::clamp(m_RotationVertical, glm::pi<float>() / 2 * -1.0f + 0.001f, glm::pi<float>() / 2 - 0.001f);
    m_RotationHorizontal = acos(glm::dot(glm::vec3(0, 0, 1), glm::vec3(forward.x, 0, forward.z)));
}

glm::vec3 Viewport::getForward() const
{
    const auto horizontalRotation = glm::rotate(glm::mat4(1.0f), m_RotationHorizontal, Renderer::UP_DIR);
    const auto temp = horizontalRotation * glm::vec4(0, 0, 1, 1);
    const auto verticalRotation = glm::rotate(glm::mat4(1.0f), m_RotationVertical, glm::cross(glm::vec3(temp), Renderer::UP_DIR));

    return verticalRotation * horizontalRotation * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
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
    if (glfwGetMouseButton(Globals::instance().getOpenGLContext(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        constexpr float speed = 0.001f;
        const auto right = glm::normalize(glm::cross(getForward(), Renderer::UP_DIR));
        const auto up = glm::normalize(glm::cross(right, getForward()));

        const auto offset = xOffset * right * speed * -1.0f + yOffset * up * speed * -1.0f;

        m_CenterOffset += offset;

        m_CenterOffset = glm::clamp(m_CenterOffset, -0.5f, 0.5f);
    }
    else if (glfwGetMouseButton(Globals::instance().getOpenGLContext(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        constexpr float speed = 0.01f / 2;
        m_RotationHorizontal += xOffset * speed;
        m_RotationVertical += yOffset * speed;
        m_RotationVertical = glm::clamp(m_RotationVertical, glm::pi<float>() / 2 * -1.0f + 0.001f, glm::pi<float>() / 2 - 0.001f);
    }
}