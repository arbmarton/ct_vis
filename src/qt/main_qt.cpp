#include "OpenGL.h"
#include "OpenGLWidget.h"
#include "Renderer.h"
#include "MainWindow.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#include <QApplication>
#pragma warning(pop)

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    app.exec();
}