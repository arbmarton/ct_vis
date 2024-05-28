#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BasicDraw.h"
#include "ImageLoader.h"
#include "OpenGL.h"
#include "Renderer.h"
#include "Utilities.h"
#include "Bank.h"
#include "DataTransform.h"
#include "Globals.h"

#include <iostream>
#include <string>
#include <limits>

constexpr bool vsync = false;

#ifdef __APPLE__
std::string folder = "/Users/marci/dev/ct_vis/dataset/3.000000-Recon 2 VENOGRAM AP-05356/";
#else
std::string folder = "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/2.000000-AX THN PORTAL-76848/";
//const std::string folder =
//"g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-00799/10-06-2008-NA-CT V ABDPEL-75914/3.000000-Recon 2 VENOGRAM AP-05356/";
//const std::string folder = "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/test/";
#endif

int main(int argc, char** argv)
{
    if (argc > 1) {
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

    if constexpr (!vsync)
    {
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    }

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    const bool fullScreen = mode->width <= int(Renderer::RENDER_WIDTH) && mode->height <= int(Renderer::RENDER_HEIGHT);
    const auto context = glfwCreateWindow(Renderer::RENDER_WIDTH, Renderer::RENDER_HEIGHT, "CT Vis", fullScreen ? primary : nullptr, nullptr);

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

    glfwSetFramebufferSizeCallback(context, [](GLFWwindow* /*window*/, const int width, const int height) { glViewport(0, 0, width, height); });
    glfwSetCursorPosCallback(context, [](GLFWwindow* /*window*/, double x, double y) { Renderer::instance().onMouseMove(float(x), float(y)); });
    glfwSetScrollCallback(context, [](GLFWwindow* /*window*/, double /*x*/, double y) { Renderer::instance().onScroll(float(y)); });
    glfwSetMouseButtonCallback(context, [](GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/) {});

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    auto imageSet = ImageLoader(folder, 12).load();
    //const auto test = data::transformHUtoPixels(imageSet.m_HounsfieldData, -2000, 500);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    //Renderer::instance().set3DTexture(utils::texture3DFromData(test));
    Renderer::instance().set3DTexture(utils::texture3DFromData(imageSet.m_HounsfieldData));
    Renderer::instance().setImageSet(&imageSet);
    std::chrono::steady_clock::time_point asd = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(asd - end).count() << "[ms]" << std::endl;

    ShaderBank::instance().getValue(ShaderType::CtViewport);
    ShaderBank::instance().getValue(ShaderType::CtViewportPostprocess);
    ShaderBank::instance().getValue(ShaderType::MainViewport);

    std::chrono::steady_clock::time_point qqqq = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(qqqq - asd).count() << "[ms]" << std::endl;


    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(context))
    {
        const auto currentTime = std::chrono::steady_clock::now();
        const float millisecs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime).count() / 1000.0f;
        lastTime = currentTime;

        (void)millisecs;

        //std::cout << "Time: " + std::to_string(millisecs) + ", FPS: " + std::to_string(1 / (millisecs / 1000)) << "\n";

        Renderer::instance().draw();

        if constexpr (!vsync)
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
