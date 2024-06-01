#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#pragma warning(disable : 4251)
#include <dcmtk/dcmimgle/dcmimage.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <string>

struct Slice
{
    Slice(const std::string& str);
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