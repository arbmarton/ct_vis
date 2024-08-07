#pragma once

#include <cinttypes>
#include <string>

struct OpenGlInfo
{
#ifdef __APPLE__
    constexpr static uint8_t m_MajorVersion = 4;
    constexpr static uint8_t m_MinorVersion = 1;
#else
    constexpr static uint8_t m_MajorVersion = 4;
    constexpr static uint8_t m_MinorVersion = 6;
#endif
    static std::string getVersionString()
    {
        return std::string("#version ") + std::to_string(m_MajorVersion) + std::to_string(m_MinorVersion) + std::string("0");
    }

private:
    OpenGlInfo();
};
