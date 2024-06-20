#pragma once

#include "Viewport.h"
#include "ImageSet.h"


#include <cinttypes>
#include <limits>
#include <optional>
#include <vector>
#include <memory>

#ifdef QT_BUILD
#include <QOpenGlFunctions>
#include <QOpenGlExtraFunctions>
#else
#pragma warning(push)
#pragma warning(disable : 4005)
#include "glad/glad.h"
#pragma warning(pop)
#endif

class Renderer
#ifdef QT_BUILD
    : protected QOpenGLExtraFunctions
#endif
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
#elif QT_BUILD
    constexpr static uint32_t RENDER_WIDTH = uint32_t(1024);
    constexpr static uint32_t RENDER_HEIGHT = uint32_t(1024);
#else
    constexpr static uint32_t RENDER_WIDTH = uint32_t(512 * 3.6);
    constexpr static uint32_t RENDER_HEIGHT = uint32_t(512 * 3.6);
#endif
    const static glm::vec3 UP_DIR;

    void draw();
    void onScroll(const float yOffset, const float speed, const bool controlPressed);
    void onMouseMove(const float xPos, const float yPos, const bool leftMousePressed, const bool middleMousePressed);
    void onMouseButton(const int button, const int action, const int mods);

    void setImageSet(std::unique_ptr<ImageSet> imgset);

    std::optional<float> getLastHoveredValue() const
    {
        return m_LastHoveredValue;
    }

private:
    Renderer();

    Viewport* getViewportFromMousePosition();
    const Viewport* getViewportFromMousePosition() const;
    void drawImGui();
    void uploadNew3DTexture();
    void swapPostProcessFrameBuffers();
    glm::vec3 calculateSamplingPositionFromMousePosition(const Viewport* viewport, const float xPos, const float yPos) const;
    std::optional<float> getHounsfieldFromSamplingPosition(const glm::vec3& v) const;
    Slice* getSliceFromSamplingPosition(const glm::vec3& v) const;

    constexpr static float m_QuadVertices[] = { // positions + texcoords
                                                -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                                                -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f
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
        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
        0.0f,
        glm::vec3(1, 0, 0));
    Viewport m_Viewport2 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(RENDER_WIDTH / 2, 0),
        glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
        0.0f,
        glm::vec3(0, 1, 0));
    Viewport m_Viewport3 = Viewport(
        glm::ivec2(512, 512),
        glm::vec2(RENDER_WIDTH / 2, RENDER_HEIGHT / 2),
        glm::ivec2(0, RENDER_HEIGHT / 2),
        glm::normalize(glm::vec3(0.0001f, 0.9999f, 0.0f)),
        0.0f,
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
    Slice* m_CurrentSlice{ nullptr };
};
