#include "Renderer.h"

#include "Bank.h"
#include "OpenGL.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/string_cast.hpp"

#include <iostream>

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

    m_PostprocessOutput = utils::generateDataTexture(512, 512);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Globals::instance().getOpenGLContext(), true);
    ImGui_ImplOpenGL3_Init(OpenGlInfo::getVersionString().data());
}

Renderer::~Renderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::draw()
{
    if (m_3DTexture == std::numeric_limits<GLuint>::max())
    {
        throw 0;
    }

    const auto renderViewport = [this](const Viewport& viewport, const Viewport& other1, const Viewport& other2) {
        const auto& dataframebuffer = viewport.getDataFrameBuffer();
        glViewport(0, 0, dataframebuffer.m_Width, dataframebuffer.m_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, dataframebuffer.m_FrameBuffer);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& ctViewportShader = ShaderBank::instance().getValue(ShaderType::CtViewport);
        ctViewportShader->use();
        ctViewportShader->setFloat("zLevel", viewport.getZLevel());
        ctViewportShader->setVec3("forward", viewport.getForward());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, m_3DTexture);

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        //const auto v = utils::getTextureData(dataframebuffer.m_TexColorBuffer, dataframebuffer.m_Width, //dataframebuffer.m_Height);
        //
        //const auto transformed = utils::applyOpenCVLowPassFilter2D(v, dataframebuffer.m_Width, //dataframebuffer.m_Height, m_FFTThreshold);
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - //begin).count() << "[ms]" << std::endl;
        //
        //
        //utils::updateTextureData(m_PostprocessOutput, dataframebuffer.m_Width, dataframebuffer.m_Height, transformed);


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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dataframebuffer.m_TexColorBuffer);

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
#else
    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
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

    drawImGui();
}

void Renderer::onScroll(const float yOffset)
{
    if (auto viewport = getViewportFromMousePosition())
    {
        viewport->onScroll(yOffset);
    }
}

void Renderer::onMouseMove(const float xPos, const float yPos)
{
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

    (void)xOffset;
    (void)yOffset;

    m_LastHoveredValue = samplePixel(xPos, yPos);
}

std::optional<float> Renderer::samplePixel(const float xPos, const float yPos) const
{
    const auto viewport = getViewportFromMousePosition();
    if (!viewport)
    {
        return {};
    }
    //const auto& framebuffer = viewport->getDataFrameBuffer();

    float x = ((xPos - viewport->getWindowOffset().x) / viewport->getPixelWidth()) * viewport->getRenderWidth();
    float y = ((yPos - viewport->getWindowOffset().y) / viewport->getPixelHeight()) * viewport->getRenderHeight();

    //y = viewport->getRenderHeight() - y;

    x /= viewport->getRenderWidth();
    y /= viewport->getRenderHeight();

    std::cout << "input xpos: " << xPos << ", transformed: " << x << "\n";
    std::cout << "input ypos: " << yPos << ", transformed: " << y << "\n";

    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.m_FrameBuffer);
    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    //float value;
    //glReadPixels(uint32_t(x), uint32_t(y), 1, 1, GL_RED, GL_FLOAT, &value);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //return { value };
    const auto forw = viewport->getForward();
    glm::vec3 center = glm::vec3(0.5, 0.5, 0.5);
    glm::vec3 right = glm::normalize(glm::cross(viewport->getForward(), glm::vec3(0.0, 1.0, 0.0)));
    glm::vec3 up = glm::normalize(glm::cross(right, viewport->getForward()));
    glm::vec3 samplingPosition = center +
        right * (x * 2.0f - 1.0f) * 0.5f +
        up * ((y) * 2.0f - 1.0f) * 0.5f +
        viewport->getForward() * (viewport->getZLevel() * 2.0f - 1.0f) * 0.5f;

    std::cout << glm::to_string(samplingPosition) << "\n";
    return m_ImageSet->sampleHounsfieldData(samplingPosition);
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
        ImGui::SetNextWindowSize({ 400 * scaling, 100 * scaling }, ImGuiCond_Always);

        ImGui::Begin("Settings", nullptr, flags);
        ImGui::SetWindowFontScale(scaling);
        ImGui::DragIntRange2("Hounsfield window", &m_HounsfieldWindowLow, &m_HounsfieldWindowHigh, 5, -3000, 2000, "Min: %d units", "Max: %d units");

        ImGui::DragFloat("FFT", &m_FFTThreshold, 0.001f, 0.1f, 1.0f);

        // TODO: use viewport coordinates instead of window coordinates
        ImGui::Text(("X: " + std::to_string(int(m_LastMouseX)) + ",Y: " + std::to_string(int(m_LastMouseY)) + ", "
                     + (m_LastHoveredValue ? std::to_string(int(round(*m_LastHoveredValue))) : "---"))
                        .c_str());

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
