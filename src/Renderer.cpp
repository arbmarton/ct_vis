#include "Renderer.h"

#include "Bank.h"

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
}

void Renderer::draw() const
{
    if (m_3DTexture == std::numeric_limits<GLuint>::max()) {
        throw 0;
    }

    const auto renderViewport = [this](const Viewport& viewport) {
        const auto& framebuffer = viewport.getFrameBuffer();
        glViewport(0, 0, framebuffer.m_Width, framebuffer.m_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.m_FrameBuffer);
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
    };

    renderViewport(m_Viewport1);
    renderViewport(m_Viewport2);
    renderViewport(m_Viewport3);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifdef __APPLE__ // TODO: check properly for a retina display somehow
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
    glBindTexture(GL_TEXTURE_2D, m_Viewport1.getFrameBuffer().m_TexColorBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_Viewport2.getFrameBuffer().m_TexColorBuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_Viewport3.getFrameBuffer().m_TexColorBuffer);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::onScroll(const float yOffset)
{
    //m_zLevel += float(yOffset * 0.01);
    //m_zLevel = glm::clamp(m_zLevel, 0.0f, 1.0f);
    if (m_LastMouseX < RENDER_WIDTH / 2 && m_LastMouseY < RENDER_HEIGHT / 2) {
        m_Viewport1.onScroll(yOffset);
    }
    else if (m_LastMouseX >= RENDER_WIDTH / 2 && m_LastMouseY < RENDER_HEIGHT / 2) {
        m_Viewport2.onScroll(yOffset);
    }
    else if (m_LastMouseX < RENDER_WIDTH / 2 && m_LastMouseY >= RENDER_HEIGHT / 2) {
        m_Viewport3.onScroll(yOffset);
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
}
