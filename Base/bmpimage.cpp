#include "bmpimage.h"

// 重载基类方法来读取BMP文件
bool BMPImage::read_file(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        LOGE("Failed to open file: %s", filename.c_str());
        return false;
    }

    // 读取BMP文件头
    BMPHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (file.gcount() != sizeof(header) || header.file_type != 0x4D42) // 'BM'标识
    {
        LOGE("Invalid BMP header");
        return false;
    }

    // 读取信息头
    BMPInfoHeader info;
    file.read(reinterpret_cast<char *>(&info), sizeof(info));
    if (file.gcount() != sizeof(info))
    {
        LOGE("Failed to read BMP info header");
        return false;
    }

    w = info.width;
    h = info.height;
    bpp = info.bits_per_pixel / 8; // 每像素字节数

    // 判断是否支持24或32位 BMP
    if (bpp != 3 && bpp != 4)
    {
        LOGE("Unsupported BMP bpp: %d", bpp * 8);
        return false;
    }

    size_t data_size = w * h * bpp;
    data.resize(data_size);

    // 跳过调色板（如果存在）
    if (header.offset < sizeof(BMPHeader) + sizeof(BMPInfoHeader))
    {
        file.seekg(header.offset);
    }

    // 读取图像数据
    file.read(reinterpret_cast<char *>(data.data()), data_size);

    if (!file.good())
    {
        LOGE("Error reading BMP data");
        return false;
    }

    // BMP文件数据从下到上排列，需要翻转
    flip_vertically();
    return true;
}

// 重载基类方法来写入BMP文件
bool BMPImage::write_file(std::string &filename, const bool vflip, const bool rle) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        LOGE("Failed to open file: %s", filename.c_str());
        return false;
    }

    BMPHeader header = {};
    header.file_type = 0x4D42; // 'BM'标识
    header.file_size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + data.size();
    header.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);

    // 文件头写入
    file.write(reinterpret_cast<const char *>(&header), sizeof(header));

    BMPInfoHeader info_header = {};
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = w;
    info_header.height = h;
    info_header.planes = 1;
    info_header.bits_per_pixel = bpp * 8;
    info_header.compression = 0; // 无压缩

    // 信息头写入
    file.write(reinterpret_cast<const char *>(&info_header), sizeof(info_header));

    // 写入图像数据
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    LOGI("write bmp file: %s", filename.c_str());

    if (!file.good())
    {
        LOGE("Error writing BMP file");
        return false;
    }

    return true;
}

// 设置像素颜色 (通过位置设置)
void BMPImage::set(const int x, const int y, const Color &color)
{
    if (x < 0 || x >= w || y < 0 || y >= h)
    {
        return;
    }

    // 根据BMP的存储方式，找到对应的像素位置
    int index = (h - y - 1) * w * bpp + x * bpp;
    auto bmpColor = BMPColor(color);
    memcpy(data.data() + index, bmpColor.getChannels(), bpp);
}

// 获取像素颜色 (通过位置获取)
std::unique_ptr<Color> BMPImage::get(const int x, const int y) const
{
    if (x < 0 || x >= w || y < 0 || y >= h)
    {
        return std::make_unique<BMPColor>(); // 默认颜色
    }

    int index = (h - y - 1) * w * bpp + x * bpp;
    auto ret = std::make_unique<BMPColor>(0, 0, 0, 0);
    const std::uint8_t *p = data.data() + index;
    std::copy(p, p + bpp, ret->getChannels());

    return ret;
}