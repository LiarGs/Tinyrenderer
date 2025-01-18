#include "AntiAliasing.h"
#include "rasterizer.h"

auto ssaa = [](const Triangle &t, int x, int y, int samples) -> float
{
    float coverage = 0.0f;
    for (int i = 0; i < samples; ++i)
    {
        for (int j = 0; j < samples; ++j)
        {
            float sub_x = x + (i + 0.5f) / samples;
            float sub_y = y + (j + 0.5f) / samples;
            if (t.insideTriangle(Vec3f{sub_x, sub_y, 1.0f}))
            {
                coverage += 1.0f / (samples * samples);
            }
        }
    }
    return coverage;
};

// void rst::rasterizer::ssaa(const Triangle &t, int x, int y, Color &color) const
// {
//     //  TO-DO:实现SSAA抗锯齿
//     int samples = 2; // 2x超采样
//     float sample_color[3] = {0.0f, 0.0f, 0.0f};
//     float sample_depth = 0.0f;

//     for (int i = 0; i < samples; ++i)
//     {
//         for (int j = 0; j < samples; ++j)
//         {
//             float sub_x = x + (i + 0.5f) / samples;
//             float sub_y = y + (j + 0.5f) / samples;
//             auto [alpha, beta, gamma] = t.computeBarycentric2D(Vec2f(sub_x, sub_y));
//             float z = t.vertex[0].z * alpha + t.vertex[1].z * beta + t.vertex[2].z * gamma;

//             if (z < depth_buf[get_index(x, y)])
//                 continue;

//             Vec3f color = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
//             sample_color[0] += color.x;
//             sample_color[1] += color.y;
//             sample_color[2] += color.z;
//             sample_depth += z;
//         }
//     }

//     sample_color[0] /= (samples * samples);
//     sample_color[1] /= (samples * samples);
//     sample_color[2] /= (samples * samples);
//     sample_depth /= (samples * samples);

//     depth_buf[get_index(x, y)] = sample_depth;
//     return Vec3f(sample_color[0], sample_color[1], sample_color[2]);
// }

void SSAA::antialiasing(const Triangle &t, int x, int y, Color &color) const
{
    // TO-DO:实现MSAA抗锯齿
}

void MSAA::antialiasing(const Triangle &t, int x, int y, Color &color) const
{
    // TO-DO:实现MSAA抗锯齿
}