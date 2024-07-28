#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ImageLoader.h"
#include "OpenGL.h"
#include "Renderer.h"
#include "Utilities.h"
#include "Bank.h"
#include "Globals.h"

#include <iostream>
#include <string>
#include <limits>

#ifdef __APPLE__
std::string folder = "/Users/marci/dev/ct_vis/dataset/3.000000-Recon 2 VENOGRAM AP-05356/";
#else
std::string folder = "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/2.000000-AX THN PORTAL-76848/";
//const std::string folder =
//"g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-00799/10-06-2008-NA-CT V ABDPEL-75914/3.000000Recon 2 VENOGRAM AP-05356/";
//const std::string folder = "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NACT-83628/test/";
#endif

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        folder = argv[1];
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGlInfo::m_MajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGlInfo::m_MinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if constexpr (!Renderer::vsync)
    {
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    }

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    const bool fullScreen = mode->width <= int(Renderer::RENDER_WIDTH) && mode->height <= int(Renderer::RENDER_HEIGHT);
    const auto context = glfwCreateWindow(Renderer::RENDER_WIDTH, Renderer::RENDER_HEIGHT, "CT Vis",fullScreen ? primary : nullptr, nullptr);

    if (context == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(context);
    glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    Globals::instance().setOpenGLContext(context);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }



    glfwSetFramebufferSizeCallback(context, [](GLFWwindow* /*window*/, const int width, const int height){ glViewport(0, 0, width, height); });
    glfwSetCursorPosCallback(context, [](GLFWwindow* window, double x, double y) { Renderer::instance().onMouseMove(float(x), float(y), glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS); });
    glfwSetScrollCallback(context, [](GLFWwindow* window, double /*x*/, double y) { Renderer::instance().onScroll(float(y), 0.01f, glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)); });
    glfwSetMouseButtonCallback(
        context, [](GLFWwindow* window, int button, int action, int /*mods*/) { Renderer::instance().onMouseButton(button == GLFW_MOUSE_BUTTON_LEFT, action == GLFW_PRESS, action == GLFW_RELEASE, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)); });

    Renderer::instance().setImageSet(ImageLoader(folder, 12).load());

    ShaderBank::instance().getValue(ShaderType::CtViewport);
    ShaderBank::instance().getValue(ShaderType::CtViewportPostprocess);
    ShaderBank::instance().getValue(ShaderType::MainViewport);
    ShaderBank::instance().getValue(ShaderType::GaussianBlur);

    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(context))
    {
        const auto currentTime = std::chrono::steady_clock::now();
        const float millisecs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime -lastTime).count() / 1000.0f;
        lastTime = currentTime;

        (void)millisecs;

        std::cout << "Time: " + std::to_string(millisecs) + ", FPS: " + std::to_string(1 / (millisecs / 1000))<< "\n";

        Renderer::instance().draw();

        if constexpr (!Renderer::vsync)
        {
            glfwSwapBuffers(context);
            glFlush();
        }
        else
        {
            glfwSwapBuffers(context);
        }
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
