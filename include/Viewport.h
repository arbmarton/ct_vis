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
    Viewport(
        const glm::ivec2& renderSize,
        const glm::vec2& pixelSize,
        const glm::ivec2& windowOffset,
        const glm::vec3& forward,
        const float zLevel,
        const glm::vec3& color);

    void onScroll(const float yOffset, const float speed, const float controlPressed, const glm::vec3& sliceSpacings);
    void onMouseMove(const float xOffset, const float yOffset, const bool leftMousePressed, const bool middleMousePressed);
    const FloatFramebuffer& getDataFrameBuffer() const
    {
        return m_Databuffer;
    }
    const Framebuffer& getColorFrameBuffer() const
    {
        return m_Colorbuffer;
    }
    glm::vec3 getForward() const;
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
    glm::vec3 getColor() const
    {
        return m_Color;
    }
    float getFov() const
    {
        return m_fov;
    }
    void setFov(const float fov)
    {
        m_fov = fov;
    }
    glm::vec3 getCenterOffset() const
    {
        return m_CenterOffset;
    }
    void setRotationVertical(const float val)
    {
        m_RotationVertical = val;
    }
    float getRotationVertical() const
    {
        return m_RotationVertical;
    }
    void setRotationHorizontal(const float val)
    {
        m_RotationHorizontal = val;
    }
    float getRotationHorizontal() const
    {
        return m_RotationHorizontal;
    }
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;
    glm::mat3 getLocalTransform() const;

    void centerOnPosition(const glm::vec3& pos, const glm::vec3& spacings);

private:
    const uint32_t m_RenderWidth;
    const uint32_t m_RenderHeight;
    const float m_PixelWidth;
    const float m_PixelHeight;
    const glm::ivec2 m_WindowOffset;

    const FloatFramebuffer m_Databuffer;
    const Framebuffer m_Colorbuffer;

    glm::vec3 m_CenterOffset = glm::vec3(0.0, 0, 0);
    float m_zLevel;
    float m_fov = 1.0f;
    float m_RotationHorizontal;
    float m_RotationVertical;

    const glm::vec3 m_Color;
};