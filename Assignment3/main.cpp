#include "tgaimage.h"
#include "model.h"
#include "Triangle.h"
#include "AntiAliasing.h"
#include "DepthTest.h"

// Bresenham's line algorithm
void line(int x0, int y0, int x1, int y1, Image *image, const Color &color)
{
    if (!image) // 图像为空，直接返回
    {
        return;
    }

    if (x0 == x1 && y0 == y1) // 线段起点和终点重合，直接绘制一个点
    {
        image->set(x0, y0, color);
        return;
    }

    // Breseham 算法绘制线段
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

    for (int x = x0, y = y0; x <= x1; x++)
    {
        if (steep)
        {
            // 如果线段是斜率大于1的，那么线段上的点原本坐标应该是(y,x)
            image->set(y, x, color);
        }
        else
        {
            image->set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

// 按线填充三角形(坐标1，坐标2，坐标3，tga指针，颜色)
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, Image *image, const Color &color)
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

// SSAA for anti-aliasing
auto ssaa = [](const Triangle &t, int x, int y, int samples) -> float
{
    float coverage = 0.0f;
    for (int i = 0; i < samples; ++i)
    {
        for (int j = 0; j < samples; ++j)
        {
            float sub_x = x + (i + 0.5f) / samples;
            float sub_y = y + (j + 0.5f) / samples;
            if (t.insideTriangle(Vec3f(sub_x, sub_y, 1.0f)))
            {
                coverage += 1.0f / (samples * samples);
            }
        }
    }
    return coverage;
};

void rasterize_triangle(const Triangle &t, Image *image, Color &color, std::vector<float> &z_buffer, const AntiAliasing &aa, const Test& test)
{
    // Find the bounding box of the current triangle
    auto [min_x, min_y, max_x, max_y] = t.getBoundingBox();

    // Iterate through the pixels and find if the current pixel is inside the triangle
    for (int x = min_x; x < max_x; ++x)
    {
        for (int y = min_y; y < max_y; ++y)
        {
            auto [alpha, beta, gamma] = t.computeBarycentric2D(Vec2f(x, y)); // 计算当前像素点的重心坐标
            auto z_interpolated = t.vertex[0].z * alpha + t.vertex[1].z * beta + t.vertex[2].z * gamma; // 插值计算该点的深度值
            // 深度测试
            if (!test.test(z_interpolated, z_buffer[x + y * image->width()], x, y)) {
                continue;
            }
            
            // Super-sampling for anti-aliasing
            const int samples = 4; // 2x2 super-sampling
            float coverage = ssaa(t, x, y, samples);

            // Set the pixel color based on coverage
            if (coverage > 0.0f)
            {
                auto blended_color = TGAColor(color);
                blended_color.setAlpha(static_cast<uint8_t>(blended_color.getAlpha() * coverage));
                z_buffer[x + y * image->width()] = z_interpolated; // 更新z-buffer
                image->set(x, y, blended_color);
            }

            // // 使用抗锯齿算法
            // aa.antialiasing(t, x, y,image, color);
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
    auto image = std::make_unique<TGAImage>(width, height, Image::RGBA);

    Vec3f light_dir(0, 0, -1); // define light_dir
    // 选择初始化 z_buffer为负无穷
    std::vector<float> z_buffer(width * height, -std::numeric_limits<float>::infinity());
    SSAA ssaa(4);
    // MSAA msaa(4);

    // 创建单独的测试策略
    SimpleDepthTest depthTest;

    // // 将多个测试策略组合成复合测试
    // std::vector<Test> tests;
    // tests.emplace_back(depthTest);

    // CompositeTest compositeTest(tests);

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
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            screen_triangle.setVertex(j, Vec3f(screen_coords[j].x, screen_coords[j].y, 1.0f));
        }
        // 用世界坐标计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir; // 光照强度=法向量*光照方向
        if (intensity > 0)
        {
            auto tgaColor = TGAColor(static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), 255, TGAImage::RGBA);
            rasterize_triangle(screen_triangle, image.get(), tgaColor, z_buffer, ssaa, depthTest);
        }
    }

    // image->flip_vertically(); // 图像上下翻转
    std::string filename = "output";
    image->write_file(filename);
    return 0;
}