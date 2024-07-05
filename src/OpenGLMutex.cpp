#include "OpenGLMutex.h"

#include "Globals.h"

#include <GLFW/glfw3.h>

OpenGLLockGuard::OpenGLLockGuard()
    : std::lock_guard<OpenGLMutex>(Globals::instance().getOpenGLMutex())
{
#ifndef QT_BUILD
    glfwMakeContextCurrent(Globals::instance().getOpenGLContext());
#endif
}

OpenGLLockGuard::~OpenGLLockGuard()
{
#ifdef __APPLE__
#else
#ifndef QT_BUILD
    glfwMakeContextCurrent(nullptr);
#endif
#endif
}
