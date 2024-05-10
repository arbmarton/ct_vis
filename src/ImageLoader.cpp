#include "ImageLoader.h"

#include "dcmtk/dcmimgle/dcmimage.h"

#include <thread>
#include <mutex>

ImageLoader::ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads)
    : m_Folder(folderPath)
    , m_MaxThreads(maxThreads == 0 ? std::thread::hardware_concurrency() : maxThreads)
{
}

ImageSet ImageLoader::load() const
{
    ImageSet ret;
    std::mutex dicomImageMutex;
    std::mutex pixelDataMutex;

    std::vector<std::filesystem::path> dicomDirectoryEntries;
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_Folder))
    {
        const auto currentPath = dirEntry.path();
        if (currentPath.extension() != ".dcm")
        {
            continue;
        }

        dicomDirectoryEntries.push_back(currentPath);
    }
    ret.m_PixelData.resize(512 * 512 * dicomDirectoryEntries.size());
    ret.m_DicomImages.resize(dicomDirectoryEntries.size());

    const auto threadLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < dicomDirectoryEntries.size(); iter += m_MaxThreads)
        {
            const auto currentPath = dicomDirectoryEntries[iter];
            auto img = std::make_unique<DicomImage>(currentPath.string().c_str());
            if (!img->isMonochrome())
            {
                throw 0;
            }
            img->setMinMaxWindow();
            const uint8_t* pixelData = static_cast<const uint8_t*>(img->getOutputData(8));

            {
                std::lock_guard<std::mutex> guard(pixelDataMutex);
                memcpy(&ret.m_PixelData[512 * 512 * iter], pixelData, 512 * 512);
            }

            {
                std::lock_guard<std::mutex> guard(dicomImageMutex);
                ret.m_DicomImages[iter] = std::move(img);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(m_MaxThreads);
    for (uint32_t thread = 0; thread < m_MaxThreads; ++thread)
    {
        threads.emplace_back(threadLambda, thread);
    }

    for (auto& th : threads)
    {
        th.join();
    }

    return ret;
}