#include "Texture.h"

Vec3f Texture::getColor(float u, float v)
{
    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);
    
    auto u_img = static_cast<int>(u * (width - 1));
    auto v_img = static_cast<int>((1 - v) * (height - 1));

    auto color = image_data.at<cv::Vec3b>(v_img, u_img);
    return Vec3f{static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2])};
}