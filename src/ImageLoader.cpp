#include "ImageLoader.h"

#include <thread>
#include <mutex>

ImageLoader::ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads)
    : m_Folder(folderPath)
{
    if (maxThreads == 0)
    {
        m_MaxThreads = std::thread::hardware_concurrency();
    }
    else
    {
        m_MaxThreads = maxThreads;
    }
}

std::vector<std::unique_ptr<DicomImage>> ImageLoader::load() const
{
    std::vector<std::unique_ptr<DicomImage>> ret;
    std::mutex mutex;

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

    const auto threadLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < dicomDirectoryEntries.size(); iter += 12)
        {
            const auto currentPath = dicomDirectoryEntries[iter];
            auto img = std::make_unique<DicomImage>(currentPath.string().c_str());

            std::lock_guard<std::mutex> guard(mutex);
            ret.push_back(std::move(img));
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