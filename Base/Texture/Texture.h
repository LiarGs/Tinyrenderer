#pragma once
#include <opencv2/opencv.hpp>

class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Vec3f getColor(float u, float v)
    {
        if (u<0)
            u = 0;
        if (u > 1)
            u = 1;
        if (v < 0)
            v = 0;
        if (v > 1)
            v = 1;
        auto u_img = static_cast<int>(u * width);
        auto v_img = static_cast<int>((1 - v) * height);
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Vec3f{static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2])};
    }

};
