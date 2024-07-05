#include "OpenGLWidget.h"

#include "Renderer.h"
#include "ImageLoader.h"
#include "Bank.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#include <QMouseEvent>
#pragma warning(pop)

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    ShaderBank::instance().getValue(ShaderType::CtViewport);
    ShaderBank::instance().getValue(ShaderType::CtViewportPostprocess);
    ShaderBank::instance().getValue(ShaderType::MainViewport);
    ShaderBank::instance().getValue(ShaderType::GaussianBlur);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    (void)w;
    (void)h;
}

void OpenGLWidget::paintGL()
{
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

    const float millisecs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_LastTime).count() / 1000.0f;
    m_LastTime = currentTime;

    (void)millisecs;

    std::cout << "Time: " + std::to_string(millisecs) + ", FPS: " + std::to_string(1 / (millisecs / 1000)) << "\n";

    Renderer::instance().draw();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    Renderer::instance().onMouseMove(float(event->pos().x()), float(event->pos().y()), event->buttons() & Qt::LeftButton, event->buttons() & Qt::MiddleButton);
    const auto lastHovered = Renderer::instance().getLastHoveredValue();
    if (lastHovered) {
        emit mouseMove("X: " + QString::number(event->pos().x()) + ", Y: " + QString::number(event->pos().y()) + ", HU value: " + QString::number(*lastHovered));
    }

    update();
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    Renderer::instance().onScroll(float(event->angleDelta().y()), 0.0001f, event->modifiers() & Qt::ControlModifier);
    update();
}