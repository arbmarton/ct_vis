#pragma once

#include <vector>
#include <cinttypes>

namespace data
{
std::vector<uint8_t> transformHUtoPixels(const std::vector<float>& input, const float lowerWindow = std::numeric_limits<float>::lowest(), const float upperWindow = std::numeric_limits<float>::max());
}