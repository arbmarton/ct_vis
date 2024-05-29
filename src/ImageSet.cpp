#include "ImageSet.h"

#include "dcmtk/dcmimgle/dcmimage.h"

#include "Utilities.h"

#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex;

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

float ImageSet::sampleHounsfieldData(const glm::vec3& v) const
{
    const uint32_t width = m_DicomImages[0]->getWidth();
    const uint32_t height = m_DicomImages[0]->getHeight();
    const uint32_t depth = uint32_t(m_DicomImages.size() - 1);

    uint32_t sampleIndex = uint32_t(v.z * depth) * width * height + uint32_t(v.y * height) * width + uint32_t(v.x * width);

    //std::cout << "sampling index: " << sampleIndex << "\n";

    if (sampleIndex < 0)
    {
        std::cout << "out of range, low\n";
        sampleIndex = 0;
    }
    if (sampleIndex >= m_HounsfieldData.size())
    {
        std::cout << "out of range, high, max value: " << m_HounsfieldData.size() << "\n";
        sampleIndex = uint32_t(m_HounsfieldData.size()) - 1;
    }

    // TODO: linear interpolation between multiple samples

    return m_HounsfieldData[sampleIndex];
}

void ImageSet::applyPostprocessing(const float fftParam)
{
    const auto maxThreads = std::max(std::thread::hardware_concurrency() - 1, 1u);

    const auto width = m_DicomImages[0]->getWidth();
    const auto height = m_DicomImages[0]->getHeight();

    const auto threadLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < m_DicomImages.size(); iter += maxThreads)
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
    return getWidth() * getHeight() * m_DicomImages.size() * sizeof(float);
}

uint32_t ImageSet::getWidth() const
{
    return m_DicomImages[0]->getWidth();
}

uint32_t ImageSet::getHeight() const
{
    return m_DicomImages[0]->getHeight();
}