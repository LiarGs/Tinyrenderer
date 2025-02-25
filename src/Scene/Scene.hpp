#pragma once
#include "Object.hpp"

struct Light
{
    Light(const Vec3f &p, const Vec3f &i) : position(p), intensity(i) {}

    Vec3f position;
    Vec3f intensity;
};

class Camera
{
private:
    // 相机的局部坐标系
    Vec3f Z; // 前向向量（看向 -Z 轴）
    Vec3f X; // 右向向量
    Vec3f Y; // 上向向量

public:
    Vec3f eye_pos;    // 相机位置
    Vec3f target_pos; // 目标位置（相机看向的点）
    Vec3f up_dir;     // 相机的上方向

    // 构造函数
    Camera(const Vec3f &eye = {0.f, 0.f, 0.f}, const Vec3f &target = {0.f, 0.f, -1.f}, const Vec3f &up = {0.f, 1.f, 0.f})
        : eye_pos(eye), target_pos(target), up_dir(up)
    {
        update(); // 初始化时更新局部坐标系
    }

    // 更新相机的局部坐标系
    void update()
    {
        Z = (eye_pos - target_pos).normalize(); // 前向向量（看向 -Z 轴）
        X = (up_dir ^ Z).normalize();           // 右向向量
        Y = (Z ^ X).normalize();                // 上向向量
    }
};

class Scene
{
public:
    // 获取单例实例
    static Scene &get_instance(size_t w, size_t h, float fov = 45.f)
    {
        static Scene instance(w, h, fov);
        return instance;
    }

    // 删除拷贝构造函数和赋值操作符，确保单例
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    Vec3f amb_light_intensity; // 环境光强度
    void set_amb_light_intensity(const Vec3f &light) { amb_light_intensity = light; }

    // 设置场景中的物体（这个函数确保场景中只有一个物体）
    void set_obj(std::unique_ptr<Object> object)
    {
        clear_objects();        // 清空场景中的物体
        add(std::move(object)); // 添加新的物体
    }
    void add(std::unique_ptr<Object> object) { objects.push_back(std::move(object)); }
    void add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }
    void set_camera(std::shared_ptr<Camera> _camera) { camera = std::move(_camera); }

    // [[nodiscard]] 是C++17标准中引入的一个属性，用于告诉编译器，函数的返回值不应该被忽略。
    // 如果一个函数被标记为 [[nodiscard]]，而调用者没有使用它的返回值，编译器将产生一个警告。
    [[nodiscard]] const std::vector<std::unique_ptr<Object>> &get_objects() const { return objects; }
    [[nodiscard]] const std::vector<std::unique_ptr<Light>> &get_lights() const { return lights; }
    [[nodiscard]] const std::shared_ptr<Camera> &get_camera() const { return camera; }

    float get_filedofView() const { return filedofView; }
    float get_aspect_ratio() const { return aspect_ratio; }
    void clear_objects() { objects.clear(); }

private:
    Scene(size_t w, size_t h, float fov) : width(w), height(h), filedofView(fov) { aspect_ratio = static_cast<float>(w / h); }

    size_t width;
    size_t height;
    float aspect_ratio;
    float filedofView;
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    std::shared_ptr<Camera> camera;
};