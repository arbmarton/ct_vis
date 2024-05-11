#pragma once

#include "Viewport.h"

#include "glad/glad.h"

#include <cinttypes>
#include <limits>

class Renderer
{
public:
    static Renderer& instance();

    constexpr static uint32_t RENDER_WIDTH = 512 * 2;
    constexpr static uint32_t RENDER_HEIGHT = 512 * 2;

    void draw() const;
    void onScroll(const float yOffset);
    void onMouseMove(const float xPos, const float yPos);

    void set3DTexture(const GLuint texture)
    {
        m_3DTexture = texture;
    }

private:
    Renderer();
    constexpr static float m_QuadVertices[] = {
        // positions + texcoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;

    Viewport m_Viewport1 = Viewport(512, 512, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)), 0.0f);
    Viewport m_Viewport2 = Viewport(512, 512, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), 0.5f);
    Viewport m_Viewport3 = Viewport(512, 512, glm::normalize(glm::vec3(0.01f, 0.99f, 0.0f)), 0.5f);

#undef max  // https://stackoverflow.com/questions/1394132/macro-and-member-function-conflict
    GLuint m_3DTexture = std::numeric_limits<GLuint>::max();

    float m_LastMouseX = RENDER_WIDTH / 2;
    float m_LastMouseY = RENDER_HEIGHT / 2;
};
