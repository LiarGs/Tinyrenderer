#include "AntiAliasing.h"

void SSAA::antialiasing(const Triangle &t, int x, int y, Image *image, Color &color) const
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
                coverage += 1.0f / (samples * samples); // 计算覆盖率
            }
        }
    }
    
    if (coverage > 0.0f) {
        auto blended_color = TGAColor(color);
        blended_color.setAlpha(static_cast<uint8_t>(color.getAlpha() * coverage));
        // z_buffer[x + y * image->width()] = z_interpolated; // 更新z-buffer
        image->set(x, y, blended_color);
    }
}

void MSAA::antialiasing(const Triangle &t, int x, int y, Image *image, Color &color) const
{
    // TO-DO:实现MSAA抗锯齿
}