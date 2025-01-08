#include "pngimage.h"

bool PNGImage::read_file(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp)
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
    {
        fclose(fp);
        throw std::runtime_error("Failed to create PNG read structure");
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Failed to create PNG info structure");
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        throw std::runtime_error("Error during PNG read");
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    w = png_get_image_width(png, info);
    h = png_get_image_height(png, info);
    bpp = png_get_channels(png, info);

    png_byte bit_depth = png_get_bit_depth(png, info);
    if (bit_depth == 16)
    {
        png_set_strip_16(png);
    }
    if (png_get_color_type(png, info) == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }
    if (png_get_color_type(png, info) == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    {
        png_set_expand_gray_1_2_4_to_8(png);
    }

    png_read_update_info(png, info);

    data.resize(w * h * bpp);
    std::vector<png_bytep> row_pointers(h);
    for (int y = 0; y < h; ++y)
    {
        row_pointers[y] = &data[y * w * bpp];
    }

    png_read_image(png, row_pointers.data());
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);

    return true;
}

// 保存 PNG 文件
bool PNGImage::write_file(const std::string &filename, const bool vflip = true, const bool rle = true) const
{
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
    {
        fclose(fp);
        throw std::runtime_error("Failed to create PNG write structure");
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        throw std::runtime_error("Failed to create PNG info structure");
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        throw std::runtime_error("Error during PNG write");
    }

    png_init_io(png, fp);

    png_set_IHDR(
        png, info, w, h, 8,
        (bpp == 4 ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB),
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    std::vector<png_bytep> row_pointers(h);
    for (int y = 0; y < h; ++y)
    {
        row_pointers[y] = const_cast<png_bytep>(&data[y * w * bpp]);
    }

    png_write_image(png, row_pointers.data());
    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(fp);

    return true;
}

// 设置指定位置的像素
void PNGImage::set(const int x, const int y, const Color &color)
{
    size_t offset = calculate_offset(x, y);
    for (size_t i = 0; i < bpp; ++i)
    {
        data[offset + i] = color.getChannel(i);
    }
}

std::unique_ptr<Color> PNGImage::get(const int x, const int y) const
{
    size_t offset = calculate_offset(x, y);
    std::vector<std::uint8_t> channels(data.begin() + offset, data.begin() + offset + bpp);
    return std::make_unique<Color>(channels);
}