#pragma once

#include "glad/glad.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm.hpp"
#pragma warning(pop)

#include <filesystem>
#include <string>
#include <map>
#include <vector>
#include <cinttypes>

class DicomImage;

namespace utils {

GLuint textureFromDicomImage(DicomImage* img);
GLuint texture3DFromData(const std::vector<uint8_t>& vec);

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
std::string getFileNameFromPath(const std::filesystem::path& path);
std::string getFileNameFromPath(const std::string& path);
void writeToFile(const std::string& content, const std::string& filePath, const std::string& fileName);
std::string readFile(const std::string& filePath, const std::string& fileName);

std::vector<std::string> getStringNumbers(const int max);
constexpr int stoi(const char* str);

}  // namespace utils