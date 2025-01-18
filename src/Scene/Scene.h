#pragma once

#include <vector>
#include <memory>
#include "Object.h"

class Light
{
public:
    Light(const Vec3f &p, const Vec3f &i)
        : position(p), intensity(i) {}
    virtual ~Light() = default;
    Vec3f position;
    Vec3f intensity;
};

struct Camera
{
    Vec3f eye_pos;
    Vec3f target_pos;
    Vec3f up_dir;
};

class Scene
{
public:
    // 获取单例实例，返回 unique_ptr
    static Scene &get_instance(size_t w, size_t h, float fov = 45)
    {
        static Scene instance(w, h, fov);
        return instance;
    }

    // 删除拷贝构造函数和赋值操作符，确保单例
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    // setting up options
    Vec3f backgroundColor = Vec3f{0.235294f, 0.67451f, 0.843137f};
    Vec3f amb_light_intensity{10, 10, 10}; // 环境光强度
    float floatEpsilon = 0.00001f;
    int maxDepth = 5;

    void add(std::unique_ptr<Object> object) { objects.push_back(std::move(object)); }
    void add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }
    void set_camera(std::unique_ptr<Camera> _camera) { camera = std::move(_camera); }

    void set_amb_light_intensity(const Vec3f &light) { amb_light_intensity = light; }

    // [[nodiscard]] 是C++17标准中引入的一个属性，用于告诉编译器，函数的返回值不应该被忽略。
    // 如果一个函数被标记为 [[nodiscard]]，而调用者没有使用它的返回值，编译器将产生一个警告。
    [[nodiscard]] const std::vector<std::unique_ptr<Object>> &get_objects() const { return objects; }
    [[nodiscard]] const std::vector<std::unique_ptr<Light>> &get_lights() const { return lights; }
    [[nodiscard]] const std::unique_ptr<Camera> &get_camera() const { return camera; }

    float get_filedofView() const { return filedofView; }
    float get_aspect_ratio() const { return aspect_ratio; }

private:
    Scene(size_t w, size_t h, float fov) : width(w), height(h), filedofView(fov) { aspect_ratio = static_cast<float>(w / h); }

    size_t width;
    size_t height;
    float aspect_ratio;
    float filedofView;
    // creating the scene (adding objects and lights)
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    std::unique_ptr<Camera> camera;
};