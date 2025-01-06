#include "Image.h"
#include <algorithm>

void Image::flip_horizontally()
{
    int row_bytes = w * bpp;
    for (int y = 0; y < h; ++y)
    {
        auto row = data.data() + y * row_bytes;
        for (int x = 0; x < w / 2; ++x)
        {
            for (int b = 0; b < bpp; ++b)
            {
                std::swap(row[x * bpp + b], row[(w - 1 - x) * bpp + b]);
            }
        }
    }
}

void Image::flip_vertically()
{
    int row_bytes = w * bpp;
    for (int y = 0; y < h / 2; ++y)
    {
        auto row1 = data.data() + y * row_bytes;
        auto row2 = data.data() + (h - 1 - y) * row_bytes;
        for (int x = 0; x < row_bytes; ++x)
        {
            std::swap(row1[x], row2[x]);
        }
    }
}
