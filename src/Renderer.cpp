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
    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

    const auto& quadShader = ShaderBank::instance().getValue(ShaderType::Quad);
    quadShader->use();
    quadShader->setFloat("zLevel", zLevel);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, testTexture2);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}