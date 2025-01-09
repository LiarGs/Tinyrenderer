#include "rasterizer.h"
#include "model.h"

const auto Red = Color(255.f, 0.f, 0.f);

int main(int argc, char **argv)
{
    Model *model = nullptr;
    const int width = 800;
    const int height = 800;

    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../../../obj/african_head/african_head.obj");
    }
    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height);

    // auto image = std::make_unique<PNGImage>(width, height, Image::RGBA); // 这里实现多态

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i); // 获取第 i 个面的顶点索引
        // 遍历三角形的三个顶点，绘制线段
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            // 坐标变换，将模型坐标系中的坐标转换到屏幕坐标系中
            auto x0 = (v0.x + 1.f) * width / 2.f;
            auto y0 = ((v0.y + 1.f) * height / 2.f);
            auto x1 = ((v1.x + 1.f) * width / 2.f);
            auto y1 = ((v1.y + 1.f) * height / 2.f);
            ras.draw_line({x0, y0, 1.f}, {x1, y1, 1.f}, Red);
            // line(x0, y0, x1, y1, image.get(), RED);
            // LOGI("line: (%d, %d) -> (%d, %d)", x0, y0, x1, y1);
        }
    }

    // image->flip_vertically();
    std::string filename = "output";
    ras.image->write_file(filename);
    delete model;
    return 0;
}