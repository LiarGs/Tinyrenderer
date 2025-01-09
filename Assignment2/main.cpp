#include "rasterizer.h"
#include "model.h"

// 按线绘制三角形(坐标1，坐标2，坐标3，tga指针，颜色)
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, Image* image, const Color& color)
{
    // 三角形面积为0
    if (t0.y == t1.y && t0.y == t2.y)
        return;
    // 根据y的大小对坐标进行排序
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    // 以高度差作为循环控制变量，此时不需要考虑斜率，因为着色完后每行都会被填充
    for (int i = 0; i < total_height; i++)
    {
        // 根据t1将三角形分割为上下两部分
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
        // 计算A,B两点的坐标
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x > B.x)
            std::swap(A, B);
        // 根据A,B和当前高度对tga着色
        for (int j = A.x; j <= B.x; j++)
        {
            image->set(j, t0.y + i, color);
        }
    }
}


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
    auto &ras = rst::rasterizer::get_instance(width, height);

    Vec3f light_dir{0, 0, -1}; // define light_dir

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3]; // 新加入一个数组用于存放三个顶点的世界坐标
        Triangle screen_triangle;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i{(v.x + 1.) * width / 2., (v.y + 1.) * height / 2.};
            screen_triangle.setVertex(j, Vec3f{static_cast<float>(screen_coords[j].x), static_cast<float>(screen_coords[j].y), v.z});
            world_coords[j] = v; // 世界坐标    即模型坐标
        }
        // 用世界坐标计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir; // 光照强度=法向量*光照方向   即法向量和光照方向重合时，亮度最高
        if (intensity > 0)
        {
            auto colorColor = Color(rand() % 255, rand() % 255, rand() % 255);
            auto lightColor = Color(intensity * 255, intensity * 255, intensity * 255, 255);
            ras.rasterize_triangle(screen_triangle, lightColor);
            // triangle(screen_coords[0], screen_coords[1], screen_coords[2], image.get(), TGAColor(rand() % 255, rand() % 255, rand() % 255, 255, TGAImage::RGB));
        }
    }

    // image->flip_vertically(); // 图像上下翻转
    std::string filename = "output";
    ras.image->write_file(filename);
    return 0;
}