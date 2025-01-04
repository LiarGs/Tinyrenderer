#include "../Base/tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255, TGAImage::RGB);
const TGAColor red = TGAColor(255, 0, 0, 255, TGAImage::RGB);

void line(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor& color)
{
    bool steep = false; // 标记当前斜率的绝对值是否大于1
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        // 斜率绝对值>1了，此时将线段端点各自的x,y坐标对调。
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    { // x0>x1时，对线段端点坐标进行对调
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            // 如果线段是斜率大于1的，那么线段上的点原本坐标应该是(y,x)
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

int main(int argc, char **argv)
{
    TGAImage image(100, 100, TGAImage::RGB);
    line(13, 20, 80, 40, image, white);
    line(20, 13, 40, 80, image, red);
    line(80, 40, 13, 20, image, red);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}