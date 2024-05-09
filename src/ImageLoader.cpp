#include "ImageLoader.h"

ImageLoader::ImageLoader(const std::filesystem::path& folderPath)
    : m_Folder(folderPath)
{
}

std::vector<std::unique_ptr<DicomImage>> ImageLoader::load() const
{
    std::vector<std::unique_ptr<DicomImage>> ret;

    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_Folder))
    {
        const auto currentPath = dirEntry.path();
        if (currentPath.extension() != ".dcm")
        {
            continue;
        }

        ret.emplace_back(std::make_unique<DicomImage>(currentPath.string().c_str()));
    }

    return ret;
}