#pragma once

#include <vector>
#include <memory>

class DicomImage;

struct ImageSet
{
    std::vector<std::unique_ptr<DicomImage>> m_DicomImages;
    std::vector<uint8_t> m_PixelData;
    std::vector<float> m_HounsfieldData;
    std::vector<float> m_PostprocessedData;
};