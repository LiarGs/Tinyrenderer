#pragma once
#include <memory>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include "Log.h"

// 通用颜色基类
class Color
{
protected:
    std::vector<std::uint8_t> channels; // 存储颜色
    std::uint8_t bytespp = 0;           // 每像素字节数
public:

    Color() = default;
    Color(std::initializer_list<std::uint8_t> ch) : channels(ch) {}

    virtual ~Color() = default;

    // 获取通道值
    const std::uint8_t& getChannel(size_t index) const
    {
        if (index >= channels.size())
        {
            throw std::out_of_range("Channel index out of range");
        }
        return channels[index];
    }

    auto getChannels() { return channels.data(); }
    std::uint8_t &operator[](const int i) { return channels[i]; }

    // 单独获取颜色通道
    virtual const std::uint8_t &getRed() const = 0;
    virtual const std::uint8_t &getGreen() const = 0;
    virtual const std::uint8_t &getBlue() const = 0;
    virtual const std::uint8_t &getAlpha() const = 0;

    // 设置通道值
    void setChannel(size_t index, std::uint8_t value)
    {
        if (index >= channels.size())
        {
            throw std::out_of_range("Channel index out of range");
        }
        channels[index] = value;
    }


    // 单独设置颜色通道
    virtual void setRed(const std::uint8_t& value) = 0;
    virtual void setGreen(const std::uint8_t& value) = 0;
    virtual void setBlue(const std::uint8_t& value) = 0;
    virtual void setAlpha(const std::uint8_t& value) = 0;
    
    // 比较两个颜色是否相同
    bool operator==(const Color &other) const
    {
        return channels == other.channels;
    }

};

// Image 类是所有图像格式的基类
class Image
{
public:
    // 通道格式
    enum Channel_Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    Image() = default;
    Image(const int w, const int h, const int bpp) : w(w), h(h), bpp(bpp), data(w * h * bpp, 0) {}
    virtual ~Image() = default;

    virtual bool read_file(const std::string& filename) = 0;
    virtual bool write_file(std::string& filename, const bool vflip = true, const bool rle = true) const = 0;
    
    void flip_horizontally();
    void flip_vertically();

    std::uint8_t* get_data() { return data.data(); };
    const std::uint8_t* get_data() const { return data.data(); };
    int width() const { return w; }
    int height() const { return h; }
    virtual const std::uint8_t &bytes_per_pixel() const = 0;
    virtual const std::string &get_format() const = 0;

    virtual void set(const int x, const int y, const Color& color) = 0;
    virtual std::unique_ptr<Color> get(const int x, const int y) const = 0;

protected:
    int w = 0;
    int h = 0;
    std::uint8_t bpp;                // 每像素字节数
    std::string format;             // 图像格式
    std::vector<std::uint8_t> data = {}; // 图像数据
};

