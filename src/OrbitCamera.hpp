#pragma once
#include <opencv2/opencv.hpp>
#include "Vec.hpp"
#include "Scene.hpp"

class OrbitCameraController
{

private:
    std::shared_ptr<Camera> camera; // 相机对象

    // 控制参数
    float rotationSpeed = 0.3f; // 旋转速度
    float zoomSpeed = 0.01f;       // 缩放速度
    float panSpeed = 0.005f;      // 平移速度
    float minDistance = 1.f;     // 最小距离
    float maxDistance = 50.f;    // 最大距离

public:
    Vec3f rotateVec{0.f, 0.f, 0.f}; // 旋转向量
    OrbitCameraController(std::shared_ptr<Camera> _camera) : camera(_camera) {}

    // 旋转相机
    void rotate(float deltaX, float deltaY)
    {
        // 将鼠标位移转换为旋转角度
        auto delta = Vec3f{deltaY, deltaX, 0.f} * rotationSpeed;
        rotateVec += delta;
    }

    // 缩放相机
    void zoom(float delta)
    {
        Vec3f forward = (camera->target_pos - camera->eye_pos).normalize();

        // 计算新的距离
        auto new_pos = camera->eye_pos + forward * delta * zoomSpeed;
        auto newforward = camera->target_pos - new_pos;
        auto newDistance = newforward.norm();

        if (newDistance < minDistance || newDistance > maxDistance || newforward * forward < 0)
            return;

        camera->eye_pos = new_pos;
    }

    // 平移相机
    void pan(float deltaX, float deltaY)
    {
        auto delta = panSpeed * Vec3f{deltaX, -deltaY, 0.f};

        camera->eye_pos -= delta;
        camera->target_pos -= delta;
    }

};

// 鼠标回调函数
void mouse_callback(int event, int x, int y, int flags, void *userdata)
{
    static int prevX = x;
    static int prevY = y;

    OrbitCameraController *controller = static_cast<OrbitCameraController *>(userdata);

    if (event == cv::EVENT_LBUTTONDOWN)
    { // 左键按下
        prevX = x;
        prevY = y;
    }
    else if (event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON))
    { // 左键拖动
        int deltaX = x - prevX;
        int deltaY = y - prevY;
        prevX = x;
        prevY = y;

        controller->rotate(deltaX, deltaY);
    }
    else if (event == cv::EVENT_RBUTTONDOWN)
    { // 右键按下
        prevX = x;
        prevY = y;
    }
    else if (event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_RBUTTON))
    { // 右键拖动
        int deltaX = x - prevX;
        int deltaY = y - prevY;
        prevX = x;
        prevY = y;

        controller->pan(deltaX, deltaY);
    }
    else if (event == cv::EVENT_MOUSEWHEEL)
    {                                              // 鼠标滚轮事件
        int delta = cv::getMouseWheelDelta(flags); // 获取滚轮滚动量
        controller->zoom(delta);
    }
}