#pragma once

#include "glad/glad.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <filesystem>
#include <string>
#include <vector>
#include <cinttypes>

struct Slice;

namespace utils {

template <typename T>
T findMinimum(T* data, const size_t length)
{
#undef max  // https://stackoverflow.com/questions/1394132/macro-and-member-function-conflict
    T minimum = std::numeric_limits<T>::max();
    for (size_t i = 0; i < length; ++i)
    {
        if (data[i] < minimum)
        {
            minimum = data[i];
        }
    }
    return minimum;
}

template <typename T>
T findMaximum(T* data, const size_t length)
{
    T maximum = std::numeric_limits<T>::lowest();
    for (size_t i = 0; i < length; ++i)
    {
        if (data[i] > maximum)
        {
            maximum = data[i];
        }
    }
    return maximum;
}

template <typename T>
std::pair<T, T> findMinimumAndMaximum(T* data, const size_t length)
{
    return std::make_pair<T, T>(findMinimum(data, length), findMaximum(data, length));
}

std::vector<float> normalizeVector(const std::vector<float>& vec);
GLuint textureFromDicomImage(Slice* img);
GLuint generateDataTexture(const uint32_t width, const uint32_t height);
GLuint texture3DFromData(const std::vector<uint8_t>& vec);
GLuint texture3DFromData(const std::vector<float>& vec);
std::vector<float> getTextureData(const GLuint id, const uint32_t width, const uint32_t height);
void updateTextureData(const GLuint id, const uint32_t width, const uint32_t height, const std::vector<float>& data);

std::vector<float> applyOpenCVLowPassFilter2D(float* input, const uint32_t width, const uint32_t height, const float cutoff);

std::filesystem::path getTempFolderPath();
std::filesystem::path getShaderFolderPath();
std::filesystem::path getShaderPath(const std::string& shaderName);
std::filesystem::path getDataFolderPath();
std::filesystem::path getSampleObjFolderPath();
std::filesystem::path getSkyboxesFolderPath();
std::filesystem::path getFontFolderPath();
std::filesystem::path getFontPath(const std::string& fontName);
std::filesystem::path getScenesPath();
std::filesystem::path getSoundsFolderPath();
std::filesystem::path getSoundPath(const std::string& soundName);

std::vector<std::string> splitString(const std::string& toSplit, const char ch);
std::vector<std::string> splitString(std::string toSplit, const std::string& delimiter);
glm::vec3 vec3FromStrings(const std::vector<std::string>& v);
std::string vec3ToString(const glm::vec3& v, const uint8_t decimals = std::numeric_limits<uint8_t>::max());
std::string getFileNameFromPath(const std::filesystem::path& path);
std::string getFileNameFromPath(const std::string& path);
void writeToFile(const std::string& content, const std::string& filePath, const std::string& fileName);
std::string readFile(const std::string& filePath, const std::string& fileName);

std::vector<std::string> getStringNumbers(const int max);
constexpr int stoi(const char* str);

std::vector<float> createGaussianBlurWeights(const size_t kernel_size, const float std_dev);

}  // namespace utils