#pragma once

#include <mutex>

class OpenGLMutex : public std::mutex
{
};

class OpenGLLockGuard : public std::lock_guard<OpenGLMutex>
{
public:
    OpenGLLockGuard();
    ~OpenGLLockGuard();
};
