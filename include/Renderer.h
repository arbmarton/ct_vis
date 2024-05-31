#pragma once

#include "Viewport.h"
#include "ImageSet.h"

#include "glad/glad.h"

#include <cinttypes>
#include <limits>
#include <optional>
#include <vector>
#include <memory>

class Renderer
{
public:
    static Renderer& instance();
    ~Renderer();

#ifdef __APPLE__
    constexpr static bool vsync = false;  // this is broken for some reason
#else
    constexpr static bool vsync = false;
#endif

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
    void onMouseButton(const int button, const int action, const int mods);

    void setImageSet(std::unique_ptr<ImageSet> imgset);

private:
    Renderer();

    std::optional<float> samplePixel(const float xPos, const float yPos) const;
    Viewport* getViewportFromMousePosition();
    const Viewport* getViewportFromMousePosition() const;
    void drawImGui();
    void uploadNew3DTexture();
    void swapPostProcessFrameBuffers();

    constexpr static float m_QuadVertices[] = {
        // positions + texcoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;
    GLuint m_PBO;
#undef max  // https://stackoverflow.com/questions/1394132/macro-and-member-function-conflict
    GLuint m_3DTexture = std::numeric_limits<GLuint>::max();

    Framebuffer m_PostProcessFrameBuffer1 = Framebuffer(512, 512);
    Framebuffer m_PostProcessFrameBuffer2 = Framebuffer(512, 512);
    Framebuffer* m_NextPostProcessFrameBuffer{ nullptr };
    Framebuffer* m_LastPostProcessFrameBuffer{ nullptr };

    Viewport m_Viewport1 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(0, 0),
        glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)),
        0.0f,
        glm::vec3(1, 0, 0));
    Viewport m_Viewport2 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(RENDER_WIDTH / 2, 0),
        glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
        0.5f,
        glm::vec3(0, 1, 0));
    Viewport m_Viewport3 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(0, RENDER_HEIGHT / 2),
        glm::normalize(glm::vec3(0.0001f, 0.9999f, 0.0f)),
        0.5f,
        glm::vec3(0, 0, 1));
    Viewport* m_CurrentViewport = nullptr;

    std::unique_ptr<ImageSet> m_ImageSet;
    bool m_NeedUpload = false;

    float m_LastMouseX = RENDER_WIDTH / 2;
    float m_LastMouseY = RENDER_HEIGHT / 2;
    bool m_MousePressed = false;
    bool m_IsFFTSliderActive = false;
    bool m_IsSliderDisabled = false;

    float m_FFTThreshold = 1.0f;
    int32_t m_HounsfieldWindowLow = -1000;
    int32_t m_HounsfieldWindowHigh = 500;

    bool m_ApplyBlur = false;
    int32_t m_BlurKernelSize = 5;
    float m_BlurSigma = 1.0f;

    std::optional<float> m_LastHoveredValue{ {} };
};
