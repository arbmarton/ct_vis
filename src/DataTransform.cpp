#include "DataTransform.h"

#include <cmath>

//#include <iostream>

namespace data {
std::vector<uint8_t> transformHUtoPixels(const std::vector<float>& input, const float lowerWindow, const float upperWindow)
{
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i] < min)
        {
            min = input[i];
        }
        if (input[i] > max)
        {
            //std::cout << input[i] << "\n";
            max = input[i];
        }
    }

    min = std::max(min, lowerWindow);
    max = std::min(max, upperWindow);

    std::vector<uint8_t> ret;
    ret.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        const float temp = std::max(input[i], min);
        const float modifiedValue = std::min(temp, max);
        const float interpolation = abs((modifiedValue - min)) / abs((max - min));

        ret[i] = uint8_t(std::lerp(0.0f, 255.0f, interpolation));
    }
    return ret;
}
}  // namespace data
