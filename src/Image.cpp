#include "Image.h"

bool Image::read_file(const std::string &filename, const std::string& image_format)
{
    cv_image = cv::imread(filename, cv::IMREAD_COLOR);
    if (cv_image.empty())
        return false;

    w = cv_image.cols;
    h = cv_image.rows;
    bpp = cv_image.channels();

    frame_buf.resize(w * h, Vec3f());

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const auto &pixel = cv_image.at<cv::Vec3b>(y, x);
            frame_buf[get_index(x, y)] = Vec3f{pixel[2] / 255.0f, pixel[1] / 255.0f, pixel[0] / 255.0f};
        }
    }

    format = image_format;
    return true;
}

bool Image::write_file(const std::string &filename, const bool vflip)
{
    // 检查 cv_image 是否为空
    if (cv_image.empty())
    {
        LOGE("Image data is empty. Cannot write to file: {}", filename);
        return false;
    }

    if (vflip) cv::flip(cv_image, cv_image, 0);
    return cv::imwrite(filename + format, cv_image);
}

void Image::set(const int &x, const int &y, const Color &color)
{
    if (x < 0 || x >= w || y < 0 || y >= h)
        return;
    frame_buf[get_index(x, y)] = color.getVec();
}

Color Image::get(const int &x, const int &y) const
{
    if (x < 0 || x >= w || y < 0 || y >= h)
        return Color();

    const auto &color = frame_buf[get_index(x, y)];
    return Color(color.x, color.y, color.z);
}