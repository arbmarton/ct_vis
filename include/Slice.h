#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include "dcmtk/dcmimgle/dcmimage.h"
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <string>

struct Slice {
	Slice(const std::string& str)
		: m_DicomImage(DicomImage(str.c_str()))
	{
	}
	Slice() = delete;

	DicomImage m_DicomImage;
	glm::vec3 m_SlicePosition;
	float m_SliceThickness;
	float m_SliceSpacing;
	float m_PixelSpacig;
};