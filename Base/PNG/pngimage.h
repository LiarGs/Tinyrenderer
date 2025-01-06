#pragma once

#include "Image.h"
#include <png.h>
#include <stdexcept>

class PNGColor : public Color {
public:
    PNGColor() : Color() { bytespp = 4; }
    PNGColor(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) : Color({r, g, b}) {
        bytespp = 3;
    }

    PNGColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : Color({r, g, b, a}) {
        bytespp = 4;
    }

    PNGColor(const Color &c) : Color(c) {}

    const std::uint8_t &getRed() const override { return getChannel(0); }
    const std::uint8_t &getGreen() const override { return getChannel(1); }
    const std::uint8_t &getBlue() const override { return getChannel(2); }
    const std::uint8_t &getAlpha() const override {
        if (bytespp == 4) {
            return getChannel(3);
        }
        return 255;
    }

    void setRed(const std::uint8_t &value) override { setChannel(0, value); }
    void setGreen(const std::uint8_t &value) override { setChannel(1, value); }
    void setBlue(const std::uint8_t &value) override { setChannel(2, value); }
    void setAlpha(const std::uint8_t &value) override {
        if (bytespp == 4) {
            setChannel(3, value);
        }
    }

    bool operator==(const PNGColor &other) const {
        return channels == other.channels;
    }

};

class PNGImage : public Image
{
public:
    PNGImage() : Image() {format = ".png";};
    PNGImage(const int w, const int h, const int bpp) : Image(w, h, bpp) {format = ".png";};
    ~PNGImage() override = default;

    // 读取 PNG 文件
    bool read_file(const std::string &filename) override;

    // 保存 PNG 文件
    bool write_file(const std::string &filename, const bool vflip = true, const bool rle = true) const override;

    // 设置指定位置的像素
    void set(const int x, const int y, const Color &color) override;

    // 获取指定位置的像素
    std::unique_ptr<Color> get(const int x, const int y) const override;

private:
    auto calculate_offset(const int x, const int y) const { return (y * w + x) * bpp; }
};
