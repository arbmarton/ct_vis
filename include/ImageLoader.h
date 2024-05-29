#pragma once

#include "ImageSet.h"

#include <filesystem>
#include <memory>

class ImageLoader
{
public:
    ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads = 0);

    std::unique_ptr<ImageSet> load() const;

private:
    const std::filesystem::path m_Folder;
    const uint32_t m_MaxThreads;
};