#include "OpenGLMutex.h"

#include "Globals.h"

#include <GLFW/glfw3.h>

OpenGLLockGuard::OpenGLLockGuard()
    : std::lock_guard<OpenGLMutex>(Globals::instance().getOpenGLMutex())
{
    glfwMakeContextCurrent(Globals::instance().getOpenGLContext());
}

OpenGLLockGuard::~OpenGLLockGuard()
{
#ifdef __APPLE__
#else
    glfwMakeContextCurrent(nullptr);
#endif
}
