#pragma once
#include "Image.h"

// BMP特定的颜色类，继承自 Color 基类
class BMPColor : public Color
{
public:
    // 默认构造函数，支持3或4个颜色通道
    BMPColor() : Color() { bytespp = 3; } // 默认为24位BMP图像
    BMPColor(const std::uint8_t r,const std::uint8_t g,const std::uint8_t b) : Color({r, g, b}) // 对于24位BMP图像
    {
        bytespp = 3; // 每像素字节数为3（RGB）
    }

    BMPColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : Color({r, g, b, a}) // 对于32位BMP图像
    {
        bytespp = 4; // 每像素字节数为4（RGBA）
    }
    BMPColor(const Color &c) : Color(c) {}

    // 重写获取颜色通道值方法
    const std::uint8_t& getRed() const override { return getChannel(0); }
    const std::uint8_t &getGreen() const override { return getChannel(1); }
    const std::uint8_t &getBlue() const override { return getChannel(2); }
    const std::uint8_t &getAlpha() const override
    {
        if (bytespp == 4) // 如果有Alpha通道
        {
            return getChannel(3);
        }
        return 255; // 默认返回255（完全不透明）
    }

    // 设置颜色通道值
    void setRed(const std::uint8_t &value) override { setChannel(0, value); }
    void setGreen(const std::uint8_t &value) override { setChannel(1, value); }
    void setBlue(const std::uint8_t &value) override { setChannel(2, value); }
    void setAlpha(const std::uint8_t &value) override
    {
        if (bytespp == 4) // 如果是RGBA格式
        {
            setChannel(3, value);
        }
    }


    // 比较两个颜色是否相等
    bool operator==(const BMPColor &other) const
    {
        return channels == other.channels;
    }

};

// BMP图像格式的实现类
class BMPImage : public Image
{
public:
    BMPImage() : Image() { format = ".bmp"; }
    BMPImage(int w, int h, int bpp = RGBA) : Image(w, h, bpp) { format = ".bmp"; }

    // 重载基类方法来读取BMP文件
    bool read_file(const std::string &filename) override;

    // 重载基类方法来写入BMP文件
    bool write_file(std::string &filename, const bool vflip = true, const bool rle = false) const override;

    const std::uint8_t &bytes_per_pixel() const override { return bpp; }
    const std::string &get_format() const override { return format; }

    // 设置像素颜色 (通过位置设置)
    void set(const int x, const int y, const Color &color) override;

    // 获取像素颜色 (通过位置获取)
    std::unique_ptr<Color> get(const int x, const int y) const override;

private:
    struct BMPHeader
    {
        std::uint16_t file_type; // 'BM' 标识
        std::uint32_t file_size; // 文件大小
        std::uint16_t reserved1;
        std::uint16_t reserved2;
        std::uint32_t offset; // 图像数据的偏移
    };

    struct BMPInfoHeader
    {
        std::uint32_t size; // 信息头大小
        std::int32_t width;
        std::int32_t height;
        std::uint16_t planes;
        std::uint16_t bits_per_pixel;
        std::uint32_t compression;
        std::uint32_t image_size;
        std::int32_t x_pixels_per_meter;
        std::int32_t y_pixels_per_meter;
        std::uint32_t colors_used;
        std::uint32_t colors_important;
    };
};

// 初始化预定义的颜色
const auto RED = BMPColor(255, 0, 0);      // 红色
const auto GREEN = BMPColor(0, 255, 0);   // 绿色
const auto BLUE = BMPColor(0, 0, 255);    // 蓝色
const auto WHITE = BMPColor(255, 255, 255); // 白色
