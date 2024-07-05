#pragma warning(push)
#pragma warning(disable : 4005)
#include <QOpenGLWidget>
#pragma warning(pop)

#include <QOpenGlFunctions>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget* parent = nullptr);

signals:
    void mouseMove(const QString& str);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    std::chrono::steady_clock::time_point m_LastTime = std::chrono::steady_clock::now();
};