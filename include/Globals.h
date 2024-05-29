#pragma once

#include "OpenGLMutex.h"

struct GLFWwindow;

class Globals
{
public:
    static Globals& instance()
    {
        static Globals* inst = nullptr;
        if (!inst)
        {
            inst = new Globals();
        }

        return *inst;
    }

    void setOpenGLContext(GLFWwindow* context)
    {
        m_Window = context;
    }
    GLFWwindow* getOpenGLContext() const
    {
        return m_Window;
    }

    OpenGLMutex& getOpenGLMutex()
    {
        return m_OpenGLmutex;
    }

private:
    GLFWwindow* m_Window{ nullptr };
    OpenGLMutex m_OpenGLmutex;
};