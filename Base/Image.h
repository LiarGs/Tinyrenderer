#pragma once
#include <memory>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Log.h"
#include "Vec.h"
namespace rst {
    class rasterizer;
}

class Color
{
private:
    float r, g, b, a;

public:

    Color() = default;
    Color(float R, float G, float B, float A = 0.0f) : r(R / 255.f), g(G / 255.f), b(B / 255.f), a(A / 255.f) {}

    auto getVec() const { return Vec3f{r, g, b}; };

    // 单独获取颜色通道
    const auto &getRed() const { return r; }
    const auto &getGreen() const { return g; }
    const auto &getBlue() const { return b; }
    const auto &getAlpha() const { return a; }

    // 单独设置颜色通道
    void setRed(const auto &value) { r = value; }
    void setGreen(const auto &value) { g = value; }
    void setBlue(const auto value){ b = value; }
    void setAlpha(const auto &value) { a = value; }

    // 比较两个颜色是否相同
    bool operator==(const Color &other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

};

// Image 类是所有图像格式的基类
class Image
{
public:
    friend class rst::rasterizer;

    // // 通道格式
    // using GRAYSCALE = float;
    // using RGB = Vec3f;
    // using RGBA = Vec4f;

    Image() = default;
    Image(const int w, const int h, const std::string &format = ".png", const int bpp = 3) : w(w), h(h), format(format), bpp(bpp) { frame_buf.resize(w * h, Vec3f(0.0f)); }
    virtual ~Image() = default;

    bool read_file(const std::string& filename, const std::string &image_format);
    bool write_file(const std::string &filename, const bool vflip = false) const;

    void flip_horizontally();
    void flip_vertically();

    auto width() const { return w; }
    auto height() const { return h; }

    auto &get_data() { return frame_buf; };
    int get_index(const int &x, const int &y) const { return (h - y - 1) * w + x; }
    const std::uint8_t &bytes_per_pixel() const { return bpp; };
    const std::string &get_format() const { return format; };

    void set(const int &x, const int &y, const Color &color);
    Color get(const int &x, const int &y) const;

protected:
    int w = 0;
    int h = 0;
    std::uint8_t bpp;                // 每像素字节数
    std::string format;             // 图像格式png/jpg
    std::vector<Vec3f> frame_buf = {}; // 储存图像数据
    mutable cv::Mat cv_image;
};
