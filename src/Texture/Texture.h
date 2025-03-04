#pragma once
#include <opencv2/opencv.hpp>
#include "Vec.hpp"
#include "Log.hpp"

class Texture{
private:
    cv::Mat image_data;
    int width, height;
    bool valid = false;

public:
    Texture() = default;
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        if (image_data.empty())
        {
            LOGE("Failed to load texture: %s", name.c_str());
            throw std::runtime_error("Failed to load texture");
        }
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
        valid = true;
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isvalid() const { return valid; }

    Vec3f getColor(float u, float v);
};
