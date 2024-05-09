#include "BasicDraw.h"

#include "CImg.h"

namespace draw {
void drawCimgFromMonochromeData(const uint32_t rows, const uint32_t cols, const uint8_t* data)
{
    using namespace cimg_library;
    CImg<unsigned char> image(cols, rows, 1, 3);  // 1: one image, 3: RGB channels

    for (uint32_t i = 0; i < rows; ++i)
    {
        for (uint32_t j = 0; j < cols; ++j)
        {
            image(j, i, 0) = data[i * cols + j];
            image(j, i, 1) = data[i * cols + j];
            image(j, i, 2) = data[i * cols + j];
        }
    }

    image.display();
}
}  // namespace draw