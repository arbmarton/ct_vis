#include "ImageLoader.h"

#include "dcmtk/dcmimgle/dcmimage.h"
#include <dcmtk/dcmdata/dctk.h>

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
    std::mutex hounsfieldMutex;

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
    // Force alphebetical order
    std::sort(dicomDirectoryEntries.begin(), dicomDirectoryEntries.end());

    ret.m_PixelData.resize(512 * 512 * dicomDirectoryEntries.size());
    ret.m_HounsfieldData.resize(512 * 512 * dicomDirectoryEntries.size());
    ret.m_DicomImages.resize(dicomDirectoryEntries.size());

    const auto threadLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < dicomDirectoryEntries.size(); iter += m_MaxThreads)
        {
            const auto& currentPath = dicomDirectoryEntries[iter];
            auto img = std::make_unique<DicomImage>(currentPath.string().c_str());
            if (!img->isMonochrome())
            {
                throw 0;
            }
            if (img->getPhotometricInterpretation() != EPI_Monochrome2)
            {
                throw 0;
            }
            DcmFileFormat fileformat;
            fileformat.loadFile(currentPath.string().c_str());
            auto dataset = fileformat.getDataset();

            Float64 pixelSpacing;
            Float64 imagePosition;
            Float64 sliceThickness;
            Float64 spacingBetweenSlices;
            dataset->findAndGetFloat64(DCM_PixelSpacing, pixelSpacing, 1);
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition);
            dataset->findAndGetFloat64(DCM_SliceThickness, sliceThickness);
            dataset->findAndGetFloat64(DCM_SpacingBetweenSlices, spacingBetweenSlices);

            Float64 rescaleIntercept;
            Float64 rescaleSlope;
            dataset->findAndGetFloat64(DCM_RescaleSlope, rescaleSlope);
            dataset->findAndGetFloat64(DCM_RescaleIntercept, rescaleIntercept);
            const auto width = img->getWidth();
            const auto height = img->getHeight();

            const auto internalPixelDataPtr = img->getInterData()->getData();
            const auto internalRepresentation = img->getInterData()->getRepresentation();

            if (internalRepresentation != EPR_Sint16)
            {
                throw 0;
            }

            const auto castedData = static_cast<const Sint16*>(internalPixelDataPtr);
            std::vector<float> floatData;
            floatData.resize(width * height);

            double min;
            double max;
            img->getMinMaxValues(min, max);
            std::cout << "min HU value in image: " << min << "\n";
            std::cout << "max HU value in image: " << max << "\n";
            std::cout << "rescale slope: " << rescaleSlope << "\n";
            std::cout << "rescale intercept: " << rescaleIntercept << "\n";

            for (size_t i = 0; i < width * height; ++i)
            {
                if (castedData[i] < min || castedData[i] > max)
                {
                    throw 0;
                }
                //std::cout << castedData[i] << " ";
                //floatData[i] = (castedData[i] * float(rescaleSlope)) + float(rescaleIntercept);
                floatData[i] = float(castedData[i]);
            }

            {
                // TODO: it should be possible to eliminate floatData
                std::lock_guard<std::mutex> guard(hounsfieldMutex);
                memcpy(&ret.m_HounsfieldData[width * height * iter], floatData.data(), width * height * sizeof(float));
            }

            img->setMinMaxWindow();
            const uint8_t* pixelData = static_cast<const uint8_t*>(img->getOutputData(8));

            {
                std::lock_guard<std::mutex> guard(pixelDataMutex);
                memcpy(&ret.m_PixelData[width * height * iter], pixelData, width * height * sizeof(uint8_t));
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
