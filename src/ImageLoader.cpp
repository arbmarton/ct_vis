#include "ImageLoader.h"

#include "Utilities.h"
#include "ImageSet.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#pragma warning(disable : 4251)
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#pragma warning(pop)

#include <thread>

ImageLoader::ImageLoader(const std::filesystem::path& folderPath, const uint32_t maxThreads)
    : m_Folder(folderPath)
    , m_MaxThreads(maxThreads == 0 ? std::thread::hardware_concurrency() : maxThreads)
{
}

std::unique_ptr<ImageSet> ImageLoader::load() const
{
    auto ret = std::make_unique<ImageSet>();

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

    ret->m_HounsfieldData.resize(512 * 512 * dicomDirectoryEntries.size());
    ret->getPostProcessedData().resize(512 * 512 * dicomDirectoryEntries.size());
    ret->m_Slices.resize(dicomDirectoryEntries.size());

    std::vector<std::unique_ptr<Slice>> tempDicomImages;
    tempDicomImages.resize(dicomDirectoryEntries.size());
    const auto dicomCreationLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < dicomDirectoryEntries.size(); iter += m_MaxThreads)
        {
            const auto& currentPath = dicomDirectoryEntries[iter];
            auto slice = std::make_unique<Slice>(currentPath.string().c_str());
            if (!slice->m_DicomImage.isMonochrome())
            {
                throw 0;
            }
            if (slice->m_DicomImage.getPhotometricInterpretation() != EPI_Monochrome2)
            {
                throw 0;
            }
            DcmFileFormat fileformat;
            fileformat.loadFile(currentPath.string().c_str());
            auto dataset = fileformat.getDataset();

            Float64 pixelSpacing;
            OFString imagePositionPatient;
            Float64 sliceThickness;
            Float64 spacingBetweenSlices;
            dataset->findAndGetFloat64(DCM_PixelSpacing, pixelSpacing, 1);
            dataset->findAndGetOFStringArray(DCM_ImagePositionPatient, imagePositionPatient);
            dataset->findAndGetFloat64(DCM_SliceThickness, sliceThickness);
            dataset->findAndGetFloat64(DCM_SpacingBetweenSlices, spacingBetweenSlices);

            slice->m_PixelSpacig = float(pixelSpacing);
            slice->m_SliceThickness = float(sliceThickness);
            slice->m_SliceSpacing = float(spacingBetweenSlices);

            const glm::vec3 imagePos = utils::vec3FromStrings(utils::splitString(imagePositionPatient.c_str(), "\\"));
            //std::cout << "ZPOS: " << std::to_string(imagePos[2]) << "\n";

            slice->m_SlicePosition = imagePos;

            tempDicomImages[iter] = std::move(slice);
        }
    };

    {
        std::vector<std::thread> threads;
        threads.reserve(m_MaxThreads);
        for (uint32_t thread = 0; thread < m_MaxThreads; ++thread)
        {
            threads.emplace_back(dicomCreationLambda, thread);
        }

        for (auto& th : threads)
        {
            th.join();
        }
    }

    std::sort(
        tempDicomImages.begin(), tempDicomImages.end(), [](const auto& left, const auto& right) { return left->m_SlicePosition.z < right->m_SlicePosition.z; });

    const auto dataLambda = [&](const uint32_t threadID) {
        for (uint32_t iter = threadID; iter < tempDicomImages.size(); iter += m_MaxThreads)
        {
            auto& currentSlice = tempDicomImages[iter];

            const auto width = currentSlice->m_DicomImage.getWidth();
            const auto height = currentSlice->m_DicomImage.getHeight();

            const auto internalPixelDataPtr = currentSlice->m_DicomImage.getInterData()->getData();
            const auto internalRepresentation = currentSlice->m_DicomImage.getInterData()->getRepresentation();

            if (internalRepresentation != EPR_Sint16)
            {
                throw 0;
            }

            const auto castedData = static_cast<const Sint16*>(internalPixelDataPtr);
            std::vector<float> floatData;
            floatData.resize(width * height);

            double min;
            double max;
            currentSlice->m_DicomImage.getMinMaxValues(min, max);
            //std::cout << "min HU value in image: " << min << "\n";
            //std::cout << "max HU value in image: " << max << "\n";

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

            memcpy(&ret->m_HounsfieldData[width * height * iter], floatData.data(), width * height * sizeof(float));

            const auto postProcessed = utils::applyOpenCVLowPassFilter2D(floatData.data(), width, height, 1.0f);
            memcpy(&ret->getPostProcessedData()[width * height * iter], postProcessed.data(), width * height * sizeof(float));

            ret->m_Slices[iter] = std::move(currentSlice);
        }
    };

    {
        std::vector<std::thread> threads;
        threads.reserve(m_MaxThreads);
        for (uint32_t thread = 0; thread < m_MaxThreads; ++thread)
        {
            threads.emplace_back(dataLambda, thread);
        }

        for (auto& th : threads)
        {
            th.join();
        }
    }

    return ret;
}
