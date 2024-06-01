#pragma once

#include "Slice.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm.hpp>
#pragma warning(pop)

#include <vector>
#include <memory>

class ImageSet
{
    friend class ImageLoader;

public:
    ~ImageSet();

    const std::vector<std::unique_ptr<Slice>>& getSlices() const
    {
        return m_Slices;
    }
    const std::vector<float>& getHounsfieldData() const
    {
        return m_HounsfieldData;
    }
    const std::vector<float>& getPostProcessedData() const;
    std::vector<float>& getPostProcessedData();

    float sampleHounsfieldData(const glm::vec3& v) const;
    void applyPostprocessing(const float fftParam);
    size_t getByteSize() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    std::vector<std::unique_ptr<Slice>> m_Slices;
    std::vector<float> m_HounsfieldData;
    std::vector<float> m_PostprocessedData;
};