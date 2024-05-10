#pragma once

#include <cinttypes>

struct OpenGlInfo
{
    constexpr static uint8_t m_MajorVersion = 4;
    constexpr static uint8_t m_MinorVersion = 6;
    static std::string getVersionString()
    {
        return std::string("#version ") + std::to_string(m_MajorVersion) + std::to_string(m_MinorVersion) + std::string("0");
    }

private:
    OpenGlInfo();
};