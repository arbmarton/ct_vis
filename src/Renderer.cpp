#include "Renderer.h"

#include "Bank.h"
#include "OpenGL.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
        const auto& framebuffer = viewport.getFrameBuffer();
        glViewport(0, 0, framebuffer.m_Width, framebuffer.m_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.m_FrameBuffer);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& ctViewportShader = ShaderBank::instance().getValue(ShaderType::CtViewport);
        ctViewportShader->use();
        ctViewportShader->setFloat("zLevel", viewport.getZLevel());
        ctViewportShader->setVec3("forward", viewport.getForward());
        ctViewportShader->setFloat("minWindow", float(m_HounsfieldWindowLow));
        ctViewportShader->setFloat("maxWindow", float(m_HounsfieldWindowHigh));
        ctViewportShader->setVec3("viewportColor", viewport.getColor());
        ctViewportShader->setVec3("otherColor1", other1.getColor());
        ctViewportShader->setVec3("otherColor2", other2.getColor());
        ctViewportShader->setVec3("otherForward1", other1.getForward());
        ctViewportShader->setVec3("otherForward2", other2.getForward());
        ctViewportShader->setFloat("otherZ1", other1.getZLevel());
        ctViewportShader->setFloat("otherZ2", other2.getZLevel());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, m_3DTexture);

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
    glBindTexture(GL_TEXTURE_2D, m_Viewport1.getFrameBuffer().m_TexColorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_Viewport2.getFrameBuffer().m_TexColorBuffers[0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_Viewport3.getFrameBuffer().m_TexColorBuffers[0]);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    drawImGui();
}

void Renderer::onScroll(const float yOffset)
{
    if (auto viewport = getViewportFromMousePosition()) {
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
    if (!viewport) {
        return {};
    }
    const auto& framebuffer = viewport->getFrameBuffer();

    const float x = ((xPos - viewport->getWindowOffset().x) / viewport->getPixelWidth()) * viewport->getRenderWidth();
    float y = ((yPos - viewport->getWindowOffset().y) / viewport->getPixelHeight()) * viewport->getRenderHeight();

    y = viewport->getRenderHeight() - y;

    //std::cout << "input xpos: " << xPos << ", transformed: " << x << "\n";
    //std::cout << "input ypos: " << yPos << ", transformed: " << y << "\n";

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.m_FrameBuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    float value;
    glReadPixels(uint32_t(x), uint32_t(y), 1, 1, GL_RED, GL_FLOAT, &value);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return { value };
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
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos({ RENDER_WIDTH / 2 + 50, RENDER_HEIGHT / 2 + 50 }, ImGuiCond_Always);
        ImGui::SetNextWindowSize({ 600, 140 }, ImGuiCond_Always);

        ImGui::Begin("Settings", nullptr, flags);
        ImGui::SetWindowFontScale(1.5f);
        ImGui::DragIntRange2("Hounsfield window", &m_HounsfieldWindowLow, &m_HounsfieldWindowHigh, 5, -3000, 2000, "Min: %d units", "Max: %d units");

        // TODO: use viewport coordinates instead of window coordinates
        ImGui::Text(("X: " + std::to_string(int(m_LastMouseX)) + ",Y: " + std::to_string(int(m_LastMouseY)) + ", " + (m_LastHoveredValue ? std::to_string(int(round(*m_LastHoveredValue))) : "---")).c_str());

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}