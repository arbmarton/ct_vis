#pragma once

#include "Utilities.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#include "dcmtk/dcmimgle/dcmimage.h"
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <string>
#include <vector>

struct Slice
{
    Slice(const std::string& str)
        : m_DicomImage(DicomImage(str.c_str()))
        , m_FileName(utils::splitString(str, '/').back())
    {
    }
    Slice(const Slice& rhs) = delete;
    Slice(Slice&& rhs) = delete;
    Slice() = delete;

    Slice& operator=(const Slice& rhs) = delete;
    Slice& operator=(Slice&& rhs) = delete;

    DicomImage m_DicomImage;
    std::string m_FileName;
    glm::vec3 m_SlicePosition;
    float m_SliceThickness;
    float m_SliceSpacing;
    float m_PixelSpacig;
};