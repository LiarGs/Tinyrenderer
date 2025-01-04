#include "../Base/tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255, TGAImage::RGB);
const TGAColor red = TGAColor(255, 0, 0, 255, TGAImage::RGB);

void line(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor& color)
{
    for (float t = 0.; t < 1.; t += 0.1f)
    {
        int x = static_cast<int>(x0 * (1.f - t) + x1 * t);
        int y = static_cast<int>(y0 * (1.f - t) + y1 * t);
        image.set(x, y, color);
    }
}

int main(int argc, char **argv)
{
    TGAImage image(100, 100, TGAImage::RGB);
    line(13, 20, 80, 40, image, white);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}