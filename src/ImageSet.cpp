#include "ImageSet.h"

#include "Utilities.h"

#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex;

ImageSet::~ImageSet()
{
}

const std::vector<float>& ImageSet::getPostProcessedData() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return m_PostprocessedData;
}

std::vector<float>& ImageSet::getPostProcessedData()
{
    std::lock_guard<std::mutex> lock(mutex);
    return m_PostprocessedData;
}

std::optional<float> ImageSet::sampleHounsfieldData(const glm::vec3& v) const
{
    if (glm::any(glm::lessThan(v, glm::vec3(0.0f))) || glm::any(glm::greaterThan(v, glm::vec3(1.0f))))
    {
        return {};
    }

    const uint32_t width = m_Slices[0]->m_DicomImage.getWidth();
    const uint32_t height = m_Slices[0]->m_DicomImage.getHeight();
    const uint32_t depth = uint32_t(m_Slices.size() - 1);

    const uint32_t sampleIndex = uint32_t(v.z * depth) * width * height + uint32_t(v.y * height) * width + uint32_t(v.x * width);

    //std::cout << "sampling index: " << sampleIndex << "\n";

    if (sampleIndex < 0)
    {
        std::cout << "out of range, low\n";
        //sampleIndex = 0;
        return {};
    }
    if (sampleIndex >= m_HounsfieldData.size())
    {
        std::cout << "out of range, high, max value: " << m_HounsfieldData.size() << "\n";
        //sampleIndex = uint32_t(m_HounsfieldData.size()) - 1;
        return {};
    }

    // TODO: linear interpolation between multiple samples

    return m_HounsfieldData[sampleIndex];
}

Slice* ImageSet::sliceFromSamplingPosition(const glm::vec3& v) const
{
    if (glm::any(glm::lessThan(v, glm::vec3(0.0f))) || glm::any(glm::greaterThan(v, glm::vec3(1.0f))))
    {
        return nullptr;
    }

    const uint32_t depth = uint32_t(m_Slices.size() - 1);

    return m_Slices[uint32_t(v.z * depth)].get();
}

void ImageSet::applyPostprocessing(const float fftParam)
{
    const auto maxThreads = std::max(std::thread::hardware_concurrency() - 1, 1u);

    const auto width = m_Slices[0]->m_DicomImage.getWidth();
    const auto height = m_Slices[0]->m_DicomImage.getHeight();

    const auto threadLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < m_Slices.size(); iter += maxThreads)
        {
            const auto floatData = &m_HounsfieldData[width * height * iter];
            const auto postProcessed = utils::applyOpenCVLowPassFilter2D(floatData, width, height, fftParam);
            {
                std::lock_guard<std::mutex> lock(mutex);
                memcpy(m_PostprocessedData.data() + (width * height * iter), postProcessed.data(), width * height * sizeof(float));
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(maxThreads);
    for (uint32_t thread = 0; thread < maxThreads; ++thread)
    {
        threads.emplace_back(threadLambda, thread);
    }

    for (auto& th : threads)
    {
        th.join();
    }
}

size_t ImageSet::getByteSize() const
{
    return getWidth() * getHeight() * m_Slices.size() * sizeof(float);
}

uint32_t ImageSet::getWidth() const
{
    return m_Slices[0]->m_DicomImage.getWidth();
}

uint32_t ImageSet::getHeight() const
{
    return m_Slices[0]->m_DicomImage.getHeight();
}

float ImageSet::getXSpacing() const
{
    return m_Slices[0]->m_xPixelSpacing;
}

float ImageSet::getYSpacing() const
{
    return m_Slices[0]->m_yPixelSpacing;
}

float ImageSet::getZSpacing() const
{
    return m_Slices[0]->m_SliceThickness;
}

glm::vec3 ImageSet::getSpacingVector() const
{
#ifdef __APPLE__
    return { 1.0f, getYSpacing() / getXSpacing(), getZSpacing() / (getXSpacing() * 2.0) }; // WHY?????
#else
    return { 1.0f, getYSpacing() / getXSpacing(), getZSpacing() / getXSpacing() };
#endif
}
