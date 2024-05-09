#pragma once

#include "dcmtk/dcmimgle/dcmimage.h"

#include <string>
#include <filesystem>
#include <vector>
#include <memory>

class ImageLoader
{
public:
    ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads = 0);

    std::vector<std::unique_ptr<DicomImage>> load() const;

private:
    const std::filesystem::path m_Folder;
    uint32_t m_MaxThreads;
};