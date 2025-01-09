/*
#pragma once
#include "Image.h"

#pragma pack(push, 1)
// TGAHeader 结构体用于表示 TGA 图像的文件头
struct TGAHeader
{
    std::uint8_t idlength = 0;        // 标识符长度
    std::uint8_t colormaptype = 0;    // 调色板类型
    std::uint8_t datatypecode = 0;    // 数据类型码
    std::uint16_t colormaporigin = 0; // 调色板原点
    std::uint16_t colormaplength = 0; // 调色板长度
    std::uint8_t colormapdepth = 0;   // 调色板深度
    std::uint16_t x_origin = 0;       // X 起始坐标
    std::uint16_t y_origin = 0;       // Y 起始坐标
    std::uint16_t width = 0;          // 图像宽度
    std::uint16_t height = 0;         // 图像高度
    std::uint8_t bitsperpixel = 0;    // 每像素位数
    std::uint8_t imagedescriptor = 0; // 图像描述符
};
#pragma pack(pop)

class TGAColor : public Color
{
public:
    TGAColor() : Color() { bytespp = 4; }
    TGAColor(const std::uint8_t &B, const std::uint8_t &G, const std::uint8_t &R, const std::uint8_t &A = 255, const std::uint8_t &bpp = 4)
        : Color({B, G, R, A})
    {
        bytespp = bpp;
    }

    TGAColor(const Color& c) : Color(c) {}

    // 获取颜色通道值
    const std::uint8_t &getRed() const override { return getChannel(2); }
    const std::uint8_t &getGreen() const override { return getChannel(1); }
    const std::uint8_t &getBlue() const override { return getChannel(0); }
    const std::uint8_t &getAlpha() const override { return getChannel(3); }

    // 设置颜色通道值
    
    void setRed(const std::uint8_t &value) override { setChannel(2, value); }
    void setGreen(const std::uint8_t &value) override { setChannel(1, value); }
    void setBlue(const std::uint8_t &value) override { setChannel(0, value); }
    void setAlpha(const std::uint8_t &value) override
    {
        if (bytespp == 4) // 如果是RGBA格式
        {
            setChannel(3, value);
        }
    }

    // 比较两个颜色是否相等
    bool operator==(const TGAColor &other) const
    {
        return channels == other.channels;
    }
};

// TGAImage 类是 TGA 图像的具体实现
class TGAImage
{
public:
    // 通道格式
    enum Channel_Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    TGAImage() { format = ".tga"; }
    TGAImage(const int w, const int h, const int bpp) : w(w), h(h), bpp(bpp), data(w * h * bpp, 0) {}
    TGAImage(const TGAImage &img) : w(img.w), h(img.h), bpp(img.bpp), format(img.format), data(img.data) { }

    bool read_file(const std::string& filename);
    bool write_file(std::string& filename, const bool vflip = true, const bool rle = true) const;

    const std::uint8_t &bytes_per_pixel() const { return bpp; }
    const std::string &get_format() const { return format; }

    void set(const int x, const int y, const Color& color);
    std::unique_ptr<Color> get(const int x, const int y) const;

    void flip_horizontally();
    void flip_vertically();

private:
    int w = 0;
    int h = 0;
    std::uint8_t bpp;                  // 每像素字节数
    std::string format;                // 图像格式
    std::vector<std::uint8_t> data = {}; //

    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;
};

// 初始化预定义的颜色
const auto RED = TGAColor(0, 0, 255, 255, TGAImage::RGBA);                 // 红色
const auto GREEN = TGAColor(0, 255, 0, 255, TGAImage::RGBA);               // 绿色
const auto BLUE = TGAColor(255, 0, 0, 255, TGAImage::RGBA);                 // 蓝色
const auto WHITE = TGAColor(255, 255, 255, 255, TGAImage::RGBA);            // 白色
*/