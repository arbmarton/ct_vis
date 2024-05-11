#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "BasicDraw.h"
#include "ImageLoader.h"
#include "OpenGL.h"
#include "Renderer.h"
#include "Utilities.h"

#include <iostream>
#include <string>
#include <limits>

constexpr bool vsync = false;

const std::string folder = "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/2.000000-AX THN PORTAL-76848/";
//const std::string folder = "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/test/";

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGlInfo::m_MajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGlInfo::m_MinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if constexpr (vsync)
    {
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    }

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    const bool fullScreen = mode->width <= int(Renderer::RENDER_WIDTH) && mode->height <= int(Renderer::RENDER_HEIGHT);
    const auto context = glfwCreateWindow(Renderer::RENDER_WIDTH, Renderer::RENDER_HEIGHT, "CT Vis", fullScreen ? primary : nullptr, nullptr);

    if (context == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(context);
    glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSetFramebufferSizeCallback(context, [](GLFWwindow* /*window*/, const int width, const int height) { glViewport(0, 0, width, height); });
    glfwSetCursorPosCallback(context, [](GLFWwindow* /*window*/, double /*x*/, double /*y*/) {});
    glfwSetScrollCallback(context, [](GLFWwindow* /*window*/, double /*x*/, double y) { Renderer::instance().onScroll(float(y)); });
    glfwSetMouseButtonCallback(context, [](GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/) {});

    const auto imageSet = ImageLoader(folder).load();
    Renderer::instance().set3DTexture(utils::texture3DFromData(imageSet.m_PixelData));

    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(context))
    {
        const auto currentTime = std::chrono::steady_clock::now();
        const float millisecs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime).count() / 1000.0f;
        lastTime = currentTime;

        (void)millisecs;

        Renderer::instance().draw();

        if constexpr (!vsync)
        {
            glfwSwapBuffers(context);
            glFlush();
        }
        else
        {
            glfwSwapBuffers(context);
        }
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


#pragma warning(push)
#pragma warning(disable : 4701)

//int main(int argc, char** argv)
//{
//    (void)argc;
//    (void)argv;
//
//    const auto loader = ImageLoader("g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/2.000000-AX THN PORTAL-76848/");
//
//    const auto imageSet = loader.load();
//    for (size_t i = 0; i < imageSet.m_DicomImages.size(); ++i)
//    {
//        draw::drawCimgFromMonochromeData(512, 512, &imageSet.m_PixelData[i * 512 * 512]);
//    }
//
//    const std::string filename =
//        "g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3L-01459/02-12-2009-NA-CT-83628/2.000000-AX THN PORTAL-76848/1-001.dcm";
//    //"g:/medical_data/test/manifest-1692379830142/CPTAC-CCRCC/C3N-02015/07-26-2010-NA-CT ABDOMEN W AND WO IV CONTRAST-02857/4.000000-LUNG  B70f-64700/1-12.dcm";
//
//    DcmFileFormat dcmFileFormat;
//    OFCondition status = dcmFileFormat.loadFile(filename.c_str());
//    if (!status.good())
//    {
//        std::cerr << "Error: Unable to load DICOM file: " << status.text() << std::endl;
//        return 1;
//    }
//
//    DcmDataset* dataset = dcmFileFormat.getDataset();
//
//    OFString photometricInterpretation;
//    if (dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation).good())
//    {
//        std::cout << "Photometric Interpretation: " << photometricInterpretation << std::endl;
//    }
//    else
//    {
//        std::cerr << "Error: Photometric Interpretation not found." << std::endl;
//    }
//
//    // Get the number of frames (if it's a multi-frame image)
//    Uint16 numberOfFrames;
//    if (dataset->findAndGetUint16(DCM_NumberOfFrames, numberOfFrames).good())
//    {
//        for (Uint16 frame = 0; frame < numberOfFrames; ++frame)
//        {
//            // Load the current frame
//            DicomImage* dcmImage = new DicomImage(filename.c_str(), dataset->getOriginalXfer(), CIF_MayDetachPixelData);
//            if (dcmImage != nullptr && dcmImage->getStatus() == EIS_Normal)
//            {
//                // Process the current frame
//                // Example: Access pixel data of the current frame
//                const Uint16* pixelData = (Uint16*)(dcmImage->getOutputData(16 /* bits per sample */, frame /* frame number */));
//                if (pixelData)
//                {
//                    // Process pixel data here
//                    std::cout << "Frame " << frame << ": Pixel data" << std::endl;
//                }
//                else
//                {
//                    std::cerr << "Error: Unable to get pixel data for frame " << frame << std::endl;
//                }
//
//                // Clean up
//                delete dcmImage;
//            }
//            else
//            {
//                std::cerr << "Error: Unable to load DICOM image for frame " << frame << std::endl;
//            }
//        }
//    }
//    else
//    {
//        // Single-frame DICOM image
//        // Load the DICOM image
//        DicomImage* dcmImage = new DicomImage(filename.c_str(), dataset->getOriginalXfer(), CIF_MayDetachPixelData);
//        if (dcmImage != nullptr && dcmImage->getStatus() == EIS_Normal)
//        {
//            std::uint16_t bitsAllocated;
//            std::uint16_t bitsStored;
//            std::uint16_t pixelRepresentation;
//            if (dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated).good())
//            {
//                // Get the Bits Stored attribute
//                dataset->findAndGetUint16(DCM_BitsStored, bitsStored);
//
//                // Get the Pixel Representation attribute
//                dataset->findAndGetUint16(DCM_PixelRepresentation, pixelRepresentation);
//
//                // Calculate the number of bits per sample
//                unsigned int numberOfBits = bitsStored;
//                if (pixelRepresentation == 1)
//                {
//                    // If Pixel Representation is signed, add an extra bit for sign
//                    numberOfBits++;
//                }
//
//                // Use 'numberOfBits' as the parameter for DicomImage.getOutputData()
//            }
//            std::cout << "bitsAllocated: " << bitsAllocated << "\n";
//            std::cout << "bitsStored: " << bitsStored << "\n";
//            std::cout << "pixelRepresentation: " << pixelRepresentation << "\n";
//
//
//            double minValue;
//            double maxValue;
//            dcmImage->getMinMaxValues(minValue, maxValue, 1);
//            std::cout << "min value in image: " << minValue << "\n";
//            std::cout << "max value in image: " << maxValue << "\n";
//
//            //auto dfgdf = dcmImage->isMonochrome();
//            //std::cout << "depth: " << dfgdf << "\n";
//            //unsigned long dataSize = dcmImage->getWidth() * dcmImage->getHeight() * dcmImage->getDepth();
//            //
//            //// Allocate memory for pixel data
//            //Uint16* buffer = new Uint16[dataSize];
//            //
//            //dcmImage->setMinMaxWindow();
//            //// Get the pixel data
//            //if (dcmImage->getOutputData(buffer, dataSize, 16)) {
//            //    for (uint32_t i = 0; i < dcmImage->getWidth() * dcmImage->getHeight(); ++i) {
//            //        //if (buffer[i] < 0) {
//            //        //    throw 0;
//            //        //}
//            //        //std::cout << (int)buffer[i] << " ";
//            //    }
//            //    // Pixel data is now in 'buffer', you can process it accordingly
//            //}
//
//            const auto dgfgdfg = dcmImage->getInterData()->getRepresentation();
//            (void)dgfgdfg;
//
//            if (dcmImage->isMonochrome())
//            {
//                dcmImage->setMinMaxWindow();
//                //Sint16* pixelData = (Sint16*)(dcmImage->getOutputData(16 /* bits */));
//                Uint8* pixelData = (Uint8*)(dcmImage->getOutputData(8 /* bits */));
//                draw::drawCimgFromMonochromeData(512, 512, pixelData);
//                if (pixelData != nullptr)
//                {
//                    for (uint32_t i = 0; i < 512 * 512; ++i)
//                    {
//                        std::cout << (int)pixelData[i] << " ";
//                        //pixelData[i] = 12;
//                    }
//
//                    // Process 16-bit pixel data as needed
//                }
//            }
//
//            // Process the single frame
//            // Example: Access pixel data of the single frame
//            Uint16 rows;
//            Uint16 columns;
//            const Uint16* pixelData = (Uint16*)(dcmImage->getOutputData(16 /* bits per sample */, 0 /* frame number */));
//            std::uint16_t smallest = (std::numeric_limits<std::uint16_t>::max)();
//            std::uint16_t highest = std::numeric_limits<std::uint16_t>::lowest();
//            Float64 rescaleIntercept;
//            Float64 rescaleSlope;
//            if (dataset->findAndGetFloat64(DCM_RescaleIntercept, rescaleIntercept).good() && dataset->findAndGetFloat64(DCM_RescaleSlope, rescaleSlope).good())
//            {
//                std::cout << "Rescale Intercept: " << rescaleIntercept << std::endl;
//                std::cout << "Rescale Slope: " << rescaleSlope << std::endl;
//            }
//            else
//            {
//                std::cerr << "Error: Rescale Slope and/or Intercept not found." << std::endl;
//            }
//            OFString rescaleType;
//            if (dataset->findAndGetOFString(DCM_RescaleType, rescaleType).good())
//            {
//                std::cout << "Rescale Type: " << rescaleType << std::endl;
//            }
//            else
//            {
//                std::cerr << "Error: Rescale Type not found." << std::endl;
//            }
//
//            // Read the Window Center attribute
//            Float64 windowCenter;
//            if (dataset->findAndGetFloat64(DCM_WindowCenter, windowCenter).good())
//            {
//                std::cout << "Window Center: " << windowCenter << std::endl;
//            }
//            else
//            {
//                std::cerr << "Error: Window Center not found." << std::endl;
//            }
//
//            // Read the Window Width attribute
//            Float64 windowWidth;
//            if (dataset->findAndGetFloat64(DCM_WindowWidth, windowWidth).good())
//            {
//                std::cout << "Window Width: " << windowWidth << std::endl;
//            }
//            else
//            {
//                std::cerr << "Error: Window Width not found." << std::endl;
//            }
//
//            if (dataset->findAndGetUint16(DCM_Rows, rows).good() && dataset->findAndGetUint16(DCM_Columns, columns).good() && pixelData)
//            {
//                std::cout << "rows: " << rows << "\n";
//                std::cout << "columns: " << columns << "\n";
//                for (int i = 0; i < rows; ++i)
//                {
//                    for (int j = 0; j < columns; ++j)
//                    {
//                        if (pixelData[i * columns + j] < smallest)
//                        {
//                            smallest = pixelData[i * columns + j];
//                        }
//                        if (pixelData[i * columns + j] > highest)
//                        {
//                            highest = pixelData[i * columns + j];
//                        }
//                        //std::cout << pixelData[i * columns + j] << " ";
//                    }
//                    //std::cout << "\n";
//                }
//
//                std::cout << smallest << " " << highest << "\n";
//            }
//            else
//            {
//                std::cerr << "Error: Unable to get pixel data for the single frame" << std::endl;
//            }
//
//            // Clean up
//            delete dcmImage;
//        }
//        else
//        {
//            std::cerr << "Error: Unable to load DICOM image" << std::endl;
//        }
//    }
//    return 0;
//}

#pragma warning(pop)