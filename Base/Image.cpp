#include "Image.h"

void Image::flip_horizontally()
{
    // TODO
}

void Image::flip_vertically()
{
    //TODO
}

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

    format = "." + image_format;
    return true;
}

bool Image::write_file(const std::string &filename, const bool vflip) const
{
    // 创建一维 float 数组
    std::vector<float> data(frame_buf.size() * 3);
    float *data_ptr = data.data();

    // 使用指针增量操作填充数据
    for (const auto &v : frame_buf)
    {
        *data_ptr++ = v.x;
        *data_ptr++ = v.y;
        *data_ptr++ = v.z;
    }
    
    // // 将frame_buf的数据转换为一维浮点数数组
    // std::vector<float> data(frame_buf.size() * 3);
    // for (size_t i = 0; i < frame_buf.size(); ++i)
    // {
    //     data[i * 3 + 0] = frame_buf[i].x; // R
    //     data[i * 3 + 1] = frame_buf[i].y; // G
    //     data[i * 3 + 2] = frame_buf[i].z; // B
    // }

    // 创建cv::Mat，类型为CV_32FC3
    cv::Mat cv_image_out(h, w, CV_32FC3, data.data());

    cv_image_out.convertTo(cv_image_out, CV_8UC3, 255.0f);

    if (vflip)
        cv::flip(cv_image_out, cv_image_out, 0);

    cv::cvtColor(cv_image_out, cv_image_out, cv::COLOR_RGB2BGR);


    return cv::imwrite(filename + format, cv_image_out);
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