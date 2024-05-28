#pragma once

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm.hpp>
#pragma warning(pop)

#include <vector>
#include <memory>

class DicomImage;

struct ImageSet
{
    std::vector<std::unique_ptr<DicomImage>> m_DicomImages;
    std::vector<uint8_t> m_PixelData;
    std::vector<float> m_HounsfieldData;
    std::vector<float> m_PostprocessedData;

    float sampleHounsfieldData(const glm::vec3& v) const;
};