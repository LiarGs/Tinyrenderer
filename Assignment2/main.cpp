#include "tgaimage.h"
#include "model.h"
#include "Triangle.h"
#include <cfloat>
#include <tuple>

// 按线绘制三角形(坐标1，坐标2，坐标3，tga指针，颜色)
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, const TGAColor& color)
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
            image.set(j, t0.y + i, color);
        }
    }
}

// 判断点是否在三角形内
static bool insideTriangle(const Vec3f p, const Vec3f *_v)
{
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Vec3f AB = _v[1] - _v[0];
    Vec3f BC = _v[2] - _v[1];
    Vec3f CA = _v[0] - _v[2];

    Vec3f Ap = p - _v[0];
    Vec3f Bp = p - _v[1];
    Vec3f Cp = p - _v[2];

    auto crossproduct = [](const Vec3f &l, const Vec3f &r) -> bool
    {
        return l.x * r.y - l.y * r.x > 0;
    };

    return (crossproduct(AB, Ap) == crossproduct(BC, Bp)) && (crossproduct(BC, Bp) == crossproduct(CA, Cp));
}

// 计算重心坐标
static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vec3f *v)
{
    float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) / (v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y - v[2].x * v[1].y);
    float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) / (v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y - v[0].x * v[2].y);
    float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) / (v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y - v[1].x * v[0].y);
    return {c1, c2, c3};
}

// 栅格化三角形
// void rasterize_triangle(const Triangle &t, TGAImage &image, const TGAColor& color) 
// {
//     // TODO : Find out the bounding box of current triangle.
//     // iterate through the pixel and find if the current pixel is inside the triangle
//     float min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN;
//     for (const auto &point : t.v)
//     {
//         min_x = std::min(min_x, point.x);
//         min_y = std::min(min_y, point.y);
//         max_x = std::max(max_x, point.x);
//         max_y = std::max(max_y, point.y);
//     }

//     for (int x = floor(min_x); x < ceil(max_x); ++x)
//     {
//         for (int y = floor(min_y); y < ceil(max_y); ++y)
//         {
//             if (insideTriangle(Vec3f(static_cast<float>(x), static_cast<float>(y), 1.0f), t.v))
//             {
//                 image.set(x, y, color);
//             }
//         }
//     }
// }

void rasterize_triangle(const Triangle &t, TGAImage &image, const TGAColor &color)
{
    // Find the bounding box of the current triangle
    float min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN;
    for (const auto &point : t.v)
    {
        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    // Iterate through the pixels and find if the current pixel is inside the triangle
    for (int x = floor(min_x); x < ceil(max_x); ++x)
    {
        for (int y = floor(min_y); y < ceil(max_y); ++y)
        {
            // Super-sampling for anti-aliasing
            float coverage = 0.0f;
            const int samples = 4; // 2x2 super-sampling
            for (int i = 0; i < samples; ++i)
            {
                for (int j = 0; j < samples; ++j)
                {
                    float sub_x = x + (i + 0.5f) / samples;
                    float sub_y = y + (j + 0.5f) / samples;
                    if (insideTriangle(Vec3f(sub_x, sub_y, 1.0f), t.v))
                    {
                        coverage += 1.0f / (samples * samples);
                    }
                }
            }

            // Set the pixel color based on coverage
            if (coverage > 0.0f)
            {
                TGAColor blended_color = color;
                blended_color[3] = static_cast<unsigned char>(color.bgra[3] * coverage); // Adjust alpha based on coverage
                image.set(x, y, blended_color);
            }
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
    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0, 0, -1); // define light_dir

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3]; // 新加入一个数组用于存放三个顶点的世界坐标
        Triangle screen_triangle;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            screen_triangle.setVertex(j, Vec3f(screen_coords[j].x, screen_coords[j].y, 1.0f));
            world_coords[j] = v; // 世界坐标    即模型坐标
        }
        // 用世界坐标计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir; // 光照强度=法向量*光照方向   即法向量和光照方向重合时，亮度最高
        if (intensity > 0)
        {
            rasterize_triangle(screen_triangle, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255, TGAImage::RGB));
        }
        // triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255, TGAImage::RGB));
    }

    image.write_tga_file("output.tga");
    return 0;
}