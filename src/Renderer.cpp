#include "Renderer.h"

#include "Bank.h"
#include "OpenGL.h"
#include "Globals.h"
#include "Utilities.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/string_cast.hpp"

#include <iostream>
#include <thread>

// https://theaisummer.com/medical-image-coordinates/
const glm::vec3 Renderer::UP_DIR = glm::vec3(0, -1, 0);

Renderer& Renderer::instance()
{
    static Renderer* inst = nullptr;

    if (!inst)
    {
        inst = new Renderer();
    }

    return *inst;
}

Renderer::Renderer()
{
#ifdef QT_BUILD
    initializeOpenGLFunctions();
#endif
    OpenGLLockGuard lock;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_QuadVertices), &m_QuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glGenBuffers(1, &m_PBO);

    m_NextPostProcessFrameBuffer = &m_PostProcessFrameBuffer1;

    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(Globals::instance().getOpenGLContext(), true);
    //ImGui_ImplOpenGL3_Init(OpenGlInfo::getVersionString().data());
}

Renderer::~Renderer()
{
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
}

void Renderer::draw()
{
    if (!m_ImageSet)
    {
        return;
    }

    if (m_NeedUpload)
    {
        uploadNew3DTexture();
    }

    OpenGLLockGuard lock;

    const auto calculateKernelSize = [this]() {
        if (m_ApplyBlur)
        {
            return m_BlurKernelSize + (1 - m_BlurKernelSize % 2);
        }
        else
        {
            return 1;
        }
    };

    const std::vector<float> gaussWeights = utils::createGaussianBlurWeights(calculateKernelSize(), m_BlurSigma);

    const auto renderViewport = [this, gaussWeights, calculateKernelSize](const Viewport& viewport, const Viewport& other1, const Viewport& other2) {
        const auto& dataframebuffer = viewport.getDataFrameBuffer();
        glViewport(0, 0, dataframebuffer.m_Width, dataframebuffer.m_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, dataframebuffer.m_FrameBuffer);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& ctViewportShader = ShaderBank::instance().getValue(ShaderType::CtViewport);
        ctViewportShader->use();
        ctViewportShader->setFloat("zLevel", viewport.getZLevel());
        ctViewportShader->setVec3("forward", viewport.getForward());
        ctViewportShader->setFloat("fov", viewport.getFov());
        ctViewportShader->setVec3("centerOffset", viewport.getCenterOffset());
        ctViewportShader->setVec3("pixelSpacing", m_ImageSet->getSpacingVector());
        ctViewportShader->setVec3("right", viewport.getRight());
        ctViewportShader->setVec3("up", viewport.getUp());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, m_3DTexture);

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        const auto& blurShader = ShaderBank::instance().getValue(ShaderType::GaussianBlur);
        blurShader->use();
        blurShader->setFloatArray("weights", 32, gaussWeights.data());
        blurShader->setInt("size", calculateKernelSize());
        bool horizontal = true;
        bool firstRun = true;
        for (int i = 0; i < 2; ++i)
        {
            blurShader->setBool("horizontal", horizontal);

            glBindFramebuffer(GL_FRAMEBUFFER, m_NextPostProcessFrameBuffer->m_FrameBuffer);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            if (firstRun)
            {
                glBindTexture(GL_TEXTURE_2D, dataframebuffer.m_TexColorBuffer);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_LastPostProcessFrameBuffer->m_TexColorBuffer);
            }

            glBindVertexArray(m_QuadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            swapPostProcessFrameBuffers();
            horizontal = !horizontal;
            firstRun = false;
        }

        const auto& colorFramebuffer = viewport.getColorFrameBuffer();
        glViewport(0, 0, colorFramebuffer.m_Width, colorFramebuffer.m_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer.m_FrameBuffer);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& ctPostprocessShader = ShaderBank::instance().getValue(ShaderType::CtViewportPostprocess);
        ctPostprocessShader->use();
        ctPostprocessShader->setFloat("zLevel", viewport.getZLevel());
        ctPostprocessShader->setVec3("forward", viewport.getForward());
        ctPostprocessShader->setFloat("minWindow", float(m_HounsfieldWindowLow));
        ctPostprocessShader->setFloat("maxWindow", float(m_HounsfieldWindowHigh));
        ctPostprocessShader->setVec3("viewportColor", viewport.getColor());
        ctPostprocessShader->setVec3("otherColor1", other1.getColor());
        ctPostprocessShader->setVec3("otherColor2", other2.getColor());
        ctPostprocessShader->setVec3("otherForward1", other1.getForward());
        ctPostprocessShader->setVec3("otherForward2", other2.getForward());
        ctPostprocessShader->setFloat("otherZ1", other1.getZLevel());
        ctPostprocessShader->setFloat("otherZ2", other2.getZLevel());
        ctPostprocessShader->setFloat("fov", viewport.getFov());
        ctPostprocessShader->setVec3("centerOffset", viewport.getCenterOffset());
        ctPostprocessShader->setVec3("otherCenterOffset1", other1.getCenterOffset());
        ctPostprocessShader->setVec3("otherCenterOffset2", other2.getCenterOffset());
        ctPostprocessShader->setVec3("pixelSpacing", m_ImageSet->getSpacingVector());
        ctPostprocessShader->setVec3("up", viewport.getUp());
        ctPostprocessShader->setVec3("right", viewport.getRight());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_LastPostProcessFrameBuffer->m_TexColorBuffer);

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    renderViewport(m_Viewport1, m_Viewport2, m_Viewport3);
    renderViewport(m_Viewport2, m_Viewport1, m_Viewport3);
    renderViewport(m_Viewport3, m_Viewport1, m_Viewport2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifdef __APPLE__  // TODO: check properly for a retina display somehow
    // Retina bullshit need to be taken into account when rendering into the backbuffer...
    glViewport(0, 0, RENDER_WIDTH * 2, RENDER_HEIGHT * 2);
#elif QT_BUILD
    glViewport(0, 0, int(RENDER_WIDTH * 1.5), int(RENDER_HEIGHT * 1.5));
#else
    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);  // monitor scaling is accounted for in Qt?
#endif
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto& mainViewportShader = ShaderBank::instance().getValue(ShaderType::MainViewport);
    mainViewportShader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Viewport1.getColorFrameBuffer().m_TexColorBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_Viewport2.getColorFrameBuffer().m_TexColorBuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_Viewport3.getColorFrameBuffer().m_TexColorBuffer);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

#ifndef QT_BUILD
    drawImGui();
#endif
}

void Renderer::onScroll(const float yOffset, const float speed, const bool controlPressed)
{
    if (!m_ImageSet)
    {
        return;
    }

    if (auto viewport = getViewportFromMousePosition())
    {
        viewport->onScroll(yOffset, speed, controlPressed, m_ImageSet->getSpacingVector());
    }
}

void Renderer::onMouseMove(const float xPos, const float yPos, const bool leftMousePressed, const bool middleMousePressed)
{
    if (!m_ImageSet)
    {
        return;
    }

    static bool firstMouse = true;

    if (firstMouse)
    {
        m_LastMouseX = float(xPos);
        m_LastMouseY = float(yPos);

        firstMouse = false;
    }

    const float xOffset = float(xPos) - m_LastMouseX;
    const float yOffset = m_LastMouseY - float(yPos);
    m_LastMouseX = float(xPos);
    m_LastMouseY = float(yPos);

    m_CurrentViewport = getViewportFromMousePosition();
    if (m_CurrentViewport)
    {
        m_CurrentViewport->onMouseMove(xOffset, yOffset, leftMousePressed, middleMousePressed);

        const auto samplingPos = calculateSamplingPositionFromMousePosition(m_CurrentViewport, xPos, yPos);
        m_LastHoveredValue = getHounsfieldFromSamplingPosition(samplingPos);
        m_CurrentSlice = getSliceFromSamplingPosition(samplingPos);
    }
    else
    {
        m_LastHoveredValue = {};
        m_CurrentSlice = nullptr;
    }
}

void Renderer::onMouseButton(const int button, const int action, const int /*mods*/)
{
    if (!m_ImageSet)
    {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            m_MousePressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_MousePressed = false;
        }
    }
}

void Renderer::setImageSet(std::unique_ptr<ImageSet> imgset)
{
    m_ImageSet = std::move(imgset);
    m_3DTexture = utils::texture3DFromData(m_ImageSet->getHounsfieldData());
}

glm::vec3 Renderer::calculateSamplingPositionFromMousePosition(const Viewport* viewport, const float xPos, const float yPos) const
{
    float x = ((xPos - viewport->getWindowOffset().x) / viewport->getPixelWidth()) * viewport->getRenderWidth();
    float y = ((yPos - viewport->getWindowOffset().y) / viewport->getPixelHeight()) * viewport->getRenderHeight();

    x /= viewport->getRenderWidth();
    y /= viewport->getRenderHeight();

    // I think this is needed because the Texcoord y axis and the window y axis are opposites
    y = 1 - y;

    //std::cout << "input xpos: " << xPos << ", transformed: " << x << "\n";
    //std::cout << "input ypos: " << yPos << ", transformed: " << y << "\n";

    const auto forw = viewport->getForward();
    const auto fov = viewport->getFov();
    const auto spacings = m_ImageSet->getSpacingVector();

    const glm::vec3 center = glm::vec3(0.5, 0.5, 0.5) + viewport->getCenterOffset() / spacings;
    const glm::vec3 right = viewport->getRight();
    const glm::vec3 up = viewport->getUp();
    glm::vec3 temp = right * (x * 2.0f - 1.0f) * 0.5f * fov + up * (y * 2.0f - 1.0f) * 0.5f * fov + forw * viewport->getZLevel();
    temp /= spacings;
    temp += center;

    return temp;
}

std::optional<float> Renderer::getHounsfieldFromSamplingPosition(const glm::vec3& v) const
{
    return m_ImageSet->sampleHounsfieldData(v);
}

Slice* Renderer::getSliceFromSamplingPosition(const glm::vec3& v) const
{
    return m_ImageSet->sliceFromSamplingPosition(v);
}

Viewport* Renderer::getViewportFromMousePosition()
{
    if (m_LastMouseX < RENDER_WIDTH / 2 && m_LastMouseY < RENDER_HEIGHT / 2)
    {
        return &m_Viewport1;
    }
    else if (m_LastMouseX >= RENDER_WIDTH / 2 && m_LastMouseY < RENDER_HEIGHT / 2)
    {
        return &m_Viewport2;
    }
    else if (m_LastMouseX < RENDER_WIDTH / 2 && m_LastMouseY >= RENDER_HEIGHT / 2)
    {
        return &m_Viewport3;
    }

    return nullptr;
}

const Viewport* Renderer::getViewportFromMousePosition() const
{
    if (m_LastMouseX < RENDER_WIDTH / 2 && m_LastMouseY < RENDER_HEIGHT / 2)
    {
        return &m_Viewport1;
    }
    else if (m_LastMouseX >= RENDER_WIDTH / 2 && m_LastMouseY < RENDER_HEIGHT / 2)
    {
        return &m_Viewport2;
    }
    else if (m_LastMouseX < RENDER_WIDTH / 2 && m_LastMouseY >= RENDER_HEIGHT / 2)
    {
        return &m_Viewport3;
    }

    return nullptr;
}

#ifndef QT_BUILD
void Renderer::drawImGui()
{
#ifdef __APPLE__
    constexpr float scaling = 1.0f;
#else
    constexpr float scaling = 1.5f;
#endif
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos({ RENDER_WIDTH / 2 + 50, RENDER_HEIGHT / 2 + 50 }, ImGuiCond_Always);
        ImGui::SetNextWindowSize({ 400 * scaling, 200 * scaling }, ImGuiCond_Always);

        ImGui::Begin("Settings", nullptr, flags);
        ImGui::SetWindowFontScale(scaling);

        // TODO: use viewport coordinates instead of window coordinates
        ImGui::Text(("X: " + std::to_string(int(m_LastMouseX)) + ",Y: " + std::to_string(int(m_LastMouseY))
                     + ", Hounsfield value: " + (m_LastHoveredValue ? std::to_string(int(round(*m_LastHoveredValue))) : "---"))
                        .c_str());
        ImGui::Text(("Slice filename: " + (m_CurrentSlice ? m_CurrentSlice->m_FileName : "----")).c_str());

        if (m_CurrentViewport)
        {
            const auto str = std::string("Dir: ") + utils::vec3ToString(m_CurrentViewport->getForward(), 2)
                             + ", Depth: " + std::to_string(m_CurrentViewport->getZLevel()).substr(0, 4)
                             + ", Fov: " + std::to_string(m_CurrentViewport->getFov()).substr(0, 4);
            ImGui::Text(str.c_str());
        }

        ImGui::DragIntRange2("Hounsfield window", &m_HounsfieldWindowLow, &m_HounsfieldWindowHigh, 5, -3000, 2000, "Min: %d units", "Max: %d units");

        if (m_IsSliderDisabled)
        {
            ImGui::BeginDisabled();
        }

        ImGui::DragFloat("FFT", &m_FFTThreshold, 0.001f, 0.1f, 1.0f);
        if (m_IsSliderDisabled)
        {
            ImGui::EndDisabled();
        }
        if (ImGui::IsItemActive())
        {
            m_IsFFTSliderActive = true;
        }
        else
        {
            if (m_IsFFTSliderActive)
            {
                m_IsFFTSliderActive = false;
                m_IsSliderDisabled = true;

                if (!m_MousePressed)
                {
                    std::cout << "Slider adjustment ended with value: " << m_FFTThreshold << std::endl;
                }

                std::thread thread([this] {
                    m_ImageSet->applyPostprocessing(m_FFTThreshold);

                    OpenGLLockGuard lock;

                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
                    glBufferData(GL_PIXEL_UNPACK_BUFFER, m_ImageSet->getByteSize(), nullptr, GL_STREAM_DRAW);

                    void* ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                    if (ptr)
                    {
                        // Copy the texture data to the PBO
                        memcpy(ptr, m_ImageSet->getPostProcessedData().data(), m_ImageSet->getByteSize());
                        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
                    }

                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                    m_NeedUpload = true;

                    m_IsSliderDisabled = false;
                });
                thread.detach();
            }
        }
        ImGui::Checkbox("Apply blur", &m_ApplyBlur);
        if (m_ApplyBlur)
        {
            ImGui::DragInt("Blur Kernel Size", &m_BlurKernelSize, 0.01f, 1, 15);
            ImGui::DragFloat("Blur Sigma", &m_BlurSigma, 0.01f, 0.01f, 15.0f);
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif

void Renderer::uploadNew3DTexture()
{
    OpenGLLockGuard lock;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
    glBindTexture(GL_TEXTURE_3D, m_3DTexture);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_ImageSet->getWidth(), m_ImageSet->getHeight(), GLsizei(m_ImageSet->getSlices().size()), GL_RED, GL_FLOAT, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    m_NeedUpload = false;
}

void Renderer::swapPostProcessFrameBuffers()
{
    if (m_NextPostProcessFrameBuffer == &m_PostProcessFrameBuffer1)
    {
        m_NextPostProcessFrameBuffer = &m_PostProcessFrameBuffer2;
        m_LastPostProcessFrameBuffer = &m_PostProcessFrameBuffer1;
    }
    else
    {
        m_NextPostProcessFrameBuffer = &m_PostProcessFrameBuffer1;
        m_LastPostProcessFrameBuffer = &m_PostProcessFrameBuffer2;
    }
}
