#include "Utilities.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#include "dcmtk/dcmimgle/dcmimage.h"
#pragma warning(pop)

#define GLM_ENABLE_EXPERIMENTAL
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4201)
#include <gtx/string_cast.hpp>
#pragma warning(pop)

#include "BasicDraw.h"

#pragma warning(push)
#pragma warning(disable : 4127)
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#pragma warning(pop)

#include <iostream>
#include <sstream>
#include <fstream>

constexpr bool is_digit(const char c)
{
    return c <= '9' && c >= '0';
}

constexpr int stoi_impl(const char* str, int value = 0)
{
    return *str ? is_digit(*str) ? stoi_impl(str + 1, (*str - '0') + value * 10) : throw "compile-time-error: not a digit" : value;
}

namespace utils {

std::vector<float> normalizeVector(const std::vector<float>& vec)
{
    if (vec.empty())
    {
        return vec;
    }

    // Find the minimum and maximum values in the vector
    float minVal = *std::min_element(vec.begin(), vec.end());
    float maxVal = *std::max_element(vec.begin(), vec.end());

    // Normalize each element in the vector
    std::vector<float> normalizedVec;
    normalizedVec.reserve(vec.size());
    for (float value : vec)
    {
        float normalizedValue = (value - minVal) / (maxVal - minVal);
        normalizedVec.push_back(normalizedValue);
    }

    return normalizedVec;
}

GLuint textureFromDicomImage(DicomImage* img)
{
    const uint8_t* data = static_cast<const uint8_t*>(img->getOutputData(8));

    GLuint textureID;
    glGenTextures(1, &textureID);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: "
                  << "\n";
    }
    return textureID;
}

GLuint texture3DFromData(const std::vector<uint8_t>& vec)
{
    const uint8_t* data = vec.data();

    GLuint textureID;
    glGenTextures(1, &textureID);

    if (data)
    {
        glBindTexture(GL_TEXTURE_3D, textureID);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 512, 512, GLsizei(vec.size() / (512 * 512)), 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_3D);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: "
                  << "\n";
    }
    return textureID;
}

GLuint texture3DFromData(const std::vector<float>& vec)
{
    const float* data = vec.data();

    GLuint textureID;
    glGenTextures(1, &textureID);

    if (data)
    {
        glBindTexture(GL_TEXTURE_3D, textureID);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 512, 512, GLsizei(vec.size() / (512 * 512)), 0, GL_RED, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_3D);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: "
                  << "\n";
    }
    return textureID;
}


void fftShift(cv::Mat magI)
{
    // crop if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    int cx = magI.cols / 2;
    int cy = magI.rows / 2;

    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    cv::Mat tmp;                            // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);                     // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

std::vector<float> applyOpenCVLowPassFilter2D(const std::vector<float>& input, const uint32_t width, const uint32_t height, const float cutoff)
{
    (void)input;
    //cv::Mat image = cv::imread("d:/Pictures/geoguesser.png", cv::IMREAD_GRAYSCALE);
    //cv::Mat image(input, true);
    //image.reshape(0, { int(height), int(width) });
    cv::Mat floatMat(512, 512, CV_32F);
    memcpy(floatMat.data, input.data(), input.size() * sizeof(float));

    const int m = cv::getOptimalDFTSize(height);
    const int n = cv::getOptimalDFTSize(width); // on the border add zero values
    cv::Mat padded(m, n, CV_32F); //expand input image to optimal size
    cv::copyMakeBorder(floatMat, padded, 0, m - height, 0, n - width, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = { padded, cv::Mat::zeros(padded.size(), CV_32F) };
    cv::Mat complexI;
    cv::merge(planes, 2, complexI); // Add to the expanded another plane with zeros

    cv::dft(complexI, complexI);

    cv::Mat mask = cv::Mat::zeros(planes[0].size(), planes[0].type());
    int cx = complexI.cols / 2;
    int cy = complexI.rows / 2;
    // Create a circular mask
    //float radius = 500.0; // Change this value to set the radius of the low-pass filter
    cv::circle(mask, cv::Point(cx, cy), static_cast<int>(cx * sqrt(2) * cutoff), cv::Scalar(1, 1), -1);

    cv::split(complexI, planes); // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))


    fftShift(planes[0]);
    fftShift(planes[1]);
    planes[0] = planes[0].mul(mask);
    planes[1] = planes[1].mul(mask);
    fftShift(planes[0]);
    fftShift(planes[1]);
    cv::merge(planes, 2, complexI);


    //cv::magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    //cv::Mat magI = planes[0];
    //
    //magI += cv::Scalar::all(1); // switch to logarithmic scale
    //cv::log(magI, magI);
    //
    //fftShift(magI);
    //
    //cv::normalize(magI, magI, 0, 1, cv::NORM_MINMAX); // Transform the matrix with float values into a
    // viewable image form (float between values 0 and 1).

    //cv::dft(complexI, complexI, cv::DFT_INVERSE);
    //cv::Mat inv;
    //cv::Mat planes2[] = {inv, cv::Mat::zeros(inv.size(), CV_32F)};
    //cv::split(complexI, planes2);

    cv::Mat inverseTransform;
    cv::dft(complexI, inverseTransform, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
    cv::normalize(inverseTransform, inverseTransform, 0, 255, cv::NORM_MINMAX);
    inverseTransform.convertTo(inverseTransform, CV_8U);


    cv::imshow("Input Image", floatMat); // Show the result
    //cv::imshow("spectrum magnitude", magI);
    cv::imshow("inverted", inverseTransform);
    cv::waitKey();

    // Shift back the zero frequency component to the original place
    //q0 = cv::Mat(magI, cv::Rect(0, 0, cx, cy));   // Top-Left
    //q1 = cv::Mat(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
    //q2 = cv::Mat(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    //q3 = cv::Mat(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right
    //
    //q0.copyTo(tmp);
    //q3.copyTo(q0);
    //tmp.copyTo(q3);
    //
    //q1.copyTo(tmp);
    //q2.copyTo(q1);
    //tmp.copyTo(q2);
    //
    //cv::idft()

    return {};
}

std::filesystem::path getTempFolderPath()
{
    std::string root = std::filesystem::current_path().parent_path().string();
    root.append("/temp/");
    return std::filesystem::path(root);
}

std::filesystem::path getShaderFolderPath()
{
#ifdef __APPLE__
    std::string root = std::filesystem::current_path().parent_path().parent_path().string();
#else
    std::string root = std::filesystem::current_path().parent_path().string();
#endif
    root.append("/shaders/");
    return std::filesystem::path(root);
}

std::filesystem::path getShaderPath(const std::string& shaderName)
{
    return std::filesystem::path(getShaderFolderPath().string() + shaderName);
}

std::filesystem::path getDataFolderPath()
{
    std::string root = std::filesystem::current_path().parent_path().string();
    root.append("/data/");
    return std::filesystem::path(root);
}

std::filesystem::path getSampleObjFolderPath()
{
    std::string dataFolder = getDataFolderPath().string();
    dataFolder.append("/sample_obj/");
    return std::filesystem::path(dataFolder);
}

std::filesystem::path getSkyboxesFolderPath()
{
    std::string dataFolder = getDataFolderPath().string();
    dataFolder.append("/skyboxes/");
    return std::filesystem::path(dataFolder);
}

std::filesystem::path getFontFolderPath()
{
    std::string root = std::filesystem::current_path().parent_path().string();
    root.append("/fonts/");
    return std::filesystem::path(root);
}

std::filesystem::path getFontPath(const std::string& fontName)
{
    return std::filesystem::path(getFontFolderPath().string() + fontName + ".ttf");
}

std::filesystem::path getScenesPath()
{
    std::string dataFolder = getDataFolderPath().string();
    dataFolder.append("/scenes/");
    return std::filesystem::path(dataFolder);
}

std::filesystem::path getSoundsFolderPath()
{
    std::string dataFolder = getDataFolderPath().string();
    dataFolder.append("/sounds/");
    return std::filesystem::path(dataFolder);
}

std::filesystem::path getSoundPath(const std::string& soundName)
{
    return std::filesystem::path(getSoundsFolderPath().string() + soundName);
}

std::vector<std::string> splitString(const std::string& toSplit, const char ch)
{
    std::stringstream test(toSplit);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(test, segment, ch))
    {
        seglist.push_back(segment);
    }

    return seglist;
}

std::string getFileNameFromPath(const std::filesystem::path& path)
{
    return getFileNameFromPath(path.string());
}

std::string getFileNameFromPath(const std::string& path)
{
    const auto split = splitString(path, '/');
    return utils::splitString(split[split.size() - 1], '.')[0];
}

void writeToFile(const std::string& content, const std::string& filePath, const std::string& fileName)
{
    std::ofstream file(filePath + "/" + fileName);
    file << content;
}

std::string readFile(const std::string& filePath, const std::string& fileName)
{
    std::ifstream istream;

    istream.open(filePath + "/" + fileName);
    std::stringstream ss;
    ss << istream.rdbuf();
    istream.close();

    return ss.str();
}

std::vector<std::string> getStringNumbers(const int max)
{
    std::vector<std::string> ret;
    for (int i = 0; i < max; ++i)
    {
        ret.push_back(std::to_string(i));
    }
    return ret;
}

constexpr int stoi(const char* str)
{
    return stoi_impl(str);
}
}  // namespace utils
