#pragma once

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

private:
    GLFWwindow* m_Window{ nullptr };
};