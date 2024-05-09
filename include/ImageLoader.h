#pragma once

#include "dcmtk/dcmimgle/dcmimage.h"

#include <string>
#include <filesystem>
#include <vector>
#include <memory>

class ImageLoader
{
public:
    ImageLoader(const std::filesystem::path& folderPath);

    std::vector<std::unique_ptr<DicomImage>> load() const;

private:
    const std::filesystem::path m_Folder;
};