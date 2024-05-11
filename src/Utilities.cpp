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

std::filesystem::path getTempFolderPath()
{
    std::string root = std::filesystem::current_path().parent_path().string();
    root.append("/temp/");
    return std::filesystem::path(root);
}

std::filesystem::path getShaderFolderPath()
{
    std::string root = std::filesystem::current_path().parent_path().string();
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
