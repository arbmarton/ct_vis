#include "Slice.h"

#include "Utilities.h"

Slice::Slice(const std::string& str)
    : m_DicomImage(DicomImage(str.c_str()))
    , m_FileName(utils::splitString(str, '/').back())
{
}