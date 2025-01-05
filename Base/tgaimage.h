#pragma once
#include <cstdint>
#include <fstream>
#include <vector>
#include "Log.h"
#pragma pack(push, 1)
// TGAHeader 结构体用于表示 TGA 图像的文件头
struct TGAHeader
{
    std::uint8_t idlength = 0; // 标识符长度
    std::uint8_t colormaptype = 0; // 调色板类型
    std::uint8_t datatypecode = 0; // 数据类型码
    std::uint16_t colormaporigin = 0; // 调色板原点
    std::uint16_t colormaplength = 0; // 调色板长度
    std::uint8_t colormapdepth = 0; // 调色板深度
    std::uint16_t x_origin = 0; // X 起始坐标
    std::uint16_t y_origin = 0; // Y 起始坐标
    std::uint16_t width = 0; // 图像宽度
    std::uint16_t height = 0; // 图像高度
    std::uint8_t bitsperpixel = 0; // 每像素位数
    std::uint8_t imagedescriptor = 0; // 图像描述符
};
#pragma pack(pop)

// TGAColor 结构体用于表示 TGA 图像的像素颜色
struct TGAColor
{
    TGAColor() = default;
    TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A, const std::uint8_t bpp) : bgra{B, G, R, A}, bytespp(bpp) {}
    std::uint8_t bgra[4] = {0, 0, 0, 0}; // 蓝色、绿色、红色和 Alpha 通道
    std::uint8_t bytespp = 4; // 每像素字节数
    std::uint8_t &operator[](const int i) { return bgra[i]; }
};


// TGAImage 结构体用于表示 TGA 图像
class TGAImage
{
public:
    // 图像格式
    enum Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    TGAImage() = default;
    TGAImage(const int w, const int h, const int bpp);
    bool read_tga_file(const std::string& filename);
    bool write_tga_file(const std::string& filename, const bool vflip = true, const bool rle = true) const;
    void flip_horizontally(); // 水平翻转
    void flip_vertically(); // 垂直翻转
    TGAColor get(const int x, const int y) const;
    void set(const int x, const int y, const TGAColor &c);
    int width() const;
    int height() const;

private:
    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;

    int w = 0;
    int h = 0;
    std::uint8_t bpp = 0; // 每像素字节数
    std::vector<std::uint8_t> data = {}; // 图像数据
};

const TGAColor white = TGAColor(255, 255, 255, 255, TGAImage::RGB);
static const TGAColor red = TGAColor(255, 0, 0, 255, TGAImage::RGB);
static const TGAColor green = TGAColor(0, 255, 0, 255, TGAImage::RGB);