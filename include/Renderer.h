#pragma once

#include "Viewport.h"

#include "glad/glad.h"

#include <cinttypes>
#include <limits>
#include <optional>
#include <vector>

class Renderer
{
public:
    static Renderer& instance();
    ~Renderer();

#ifdef __APPLE__
    constexpr static uint32_t RENDER_WIDTH = uint32_t(512 * 2);
    constexpr static uint32_t RENDER_HEIGHT = uint32_t(512 * 2);
#else
    constexpr static uint32_t RENDER_WIDTH = uint32_t(512 * 3.6);
    constexpr static uint32_t RENDER_HEIGHT = uint32_t(512 * 3.6);
#endif

    void draw();
    void onScroll(const float yOffset);
    void onMouseMove(const float xPos, const float yPos);

    void set3DTexture(const GLuint texture)
    {
        m_3DTexture = texture;
    }

private:
    Renderer();

    std::optional<float> samplePixel(const float xPos, const float yPos) const;
    Viewport* getViewportFromMousePosition();
    const Viewport* getViewportFromMousePosition() const;
    void drawImGui();

    constexpr static float m_QuadVertices[] = {
        // positions + texcoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;

    Viewport m_Viewport1 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(0, 0),
        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
        0.0f,
        glm::vec3(1, 0, 0));
    Viewport m_Viewport2 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(RENDER_WIDTH / 2, 0),
        glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)),
        0.5f,
        glm::vec3(0, 1, 0));
    Viewport m_Viewport3 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(0, RENDER_HEIGHT / 2),
        glm::normalize(glm::vec3(0.0001f, 0.9999f, 0.0f)),
        0.5f,
        glm::vec3(0, 0, 1));

#undef max  // https://stackoverflow.com/questions/1394132/macro-and-member-function-conflict
    GLuint m_3DTexture = std::numeric_limits<GLuint>::max();
    GLuint m_PostprocessOutput = std::numeric_limits<GLuint>::max();

    float m_LastMouseX = RENDER_WIDTH / 2;
    float m_LastMouseY = RENDER_HEIGHT / 2;

    float m_FFTThreshold = 1.0f;
    int32_t m_HounsfieldWindowLow = -1000;
    int32_t m_HounsfieldWindowHigh = 500;

    std::optional<float> m_LastHoveredValue{ {} };
};
