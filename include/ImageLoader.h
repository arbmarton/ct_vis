#pragma once

#include "ImageSet.h"

#include <filesystem>

class ImageLoader
{
public:
    ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads = 0);

    ImageSet load() const;

private:
    const std::filesystem::path m_Folder;
    const uint32_t m_MaxThreads;
};