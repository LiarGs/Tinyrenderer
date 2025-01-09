#include "model.h"
#include "rasterizer.h"

int main(int argc, char **argv)
{
    Model *model = nullptr;
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../../../obj/african_head/african_head.obj");
    }
    auto width = 800;
    auto height = 800;
    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height, ".png");

    Vec3f light_dir{0, 0, -1}; // define light_dir

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3]; // 三角形三个顶点的屏幕坐标
        Vec3f world_coords[3]; // 新加入一个数组用于存放三个顶点的世界坐标
        Triangle screen_triangle;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            world_coords[j] = v;
            screen_coords[j] = Vec2i{(v.x + 1.) * width / 2., (v.y + 1.) * height / 2.};
            screen_triangle.setVertex(j, Vec3f{static_cast<float>(screen_coords[j].x), static_cast<float>(screen_coords[j].y), v.z});
        }
        // 用世界坐标计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir; // 光照强度=法向量*光照方向
        if (intensity > 0)
        {
            auto colorColor = Color(rand() % 255, rand() % 255, rand() % 255);
            auto lightColor = Color(intensity * 255, intensity * 255, intensity * 255, 255);
            ras.rasterize_triangle(screen_triangle, lightColor);
        }
    }

    std::string filename = "output";
    ras.image->write_file(filename);
    return 0;
}