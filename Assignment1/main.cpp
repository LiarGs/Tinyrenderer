#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255, TGAImage::RGB);
const TGAColor red = TGAColor(255, 0, 0, 255, TGAImage::RGB);
Model *model = nullptr;
const int width = 800;
const int height = 800;

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
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../../../obj/african_head/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i); // 获取第 i 个面的顶点索引
        // 遍历面的三个顶点，绘制线段
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            // 坐标变换，将模型坐标系中的坐标转换到屏幕坐标系中
            int x0 = static_cast<int>((v0.x + 1.) * width / 2.f);
            int y0 = static_cast<int>((v0.y + 1.) * height / 2.f);
            int x1 = static_cast<int>((v1.x + 1.) * width / 2.f);
            int y1 = static_cast<int>((v1.y + 1.) * height / 2.f);
            line(x0, y0, x1, y1, image, white);
            // LOGI("line: (%d, %d) -> (%d, %d)", x0, y0, x1, y1);
        }
    }

    // image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}