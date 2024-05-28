#include "ImageSet.h"

#include "dcmtk/dcmimgle/dcmimage.h"

#include <iostream>

float ImageSet::sampleHounsfieldData(const glm::vec3& v) const
{
    const uint32_t width = m_DicomImages[0]->getWidth();
    const uint32_t height = m_DicomImages[0]->getHeight();
    const uint32_t depth = uint32_t(m_DicomImages.size() - 1);

    const uint32_t sampleIndex = uint32_t(v.z * depth) * width * height + uint32_t(v.y * height) * width + uint32_t(v.x * width);

    uint32_t truncated = uint32_t(sampleIndex);

    std::cout << "sampling index: " << truncated << "\n";

    if (truncated < 0) {
        std::cout << "out of range, low\n";
        truncated = 0;
    }
    if (truncated >= m_HounsfieldData.size()) {
        std::cout << "out of range, high, max value: " << m_HounsfieldData.size() << "\n";
        truncated = uint32_t(m_HounsfieldData.size()) - 1;
    }

    return m_HounsfieldData[truncated];
}