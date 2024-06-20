#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>

class ImageSet;

class ImageLoader
{
public:
    ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads = 0);

    std::unique_ptr<ImageSet> load(std::function<void()> callback = nullptr);
    size_t entries() const
    {
        return m_DicomDirectoryEntries.size();
    }

private:
    const std::filesystem::path m_Folder;
    const uint32_t m_MaxThreads;
    std::mutex m_Mutex;

    std::vector<std::filesystem::path> m_DicomDirectoryEntries;
};
