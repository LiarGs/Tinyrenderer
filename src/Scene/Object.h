#pragma once
#include "Vec.h"
#include "Triangle.h"

enum MaterialType
{
    DIFFUSE_AND_GLOSSY,
    REFLECTION_AND_REFRACTION,
    REFLECTION
};

struct Material
{
    // 基础属性
    Vec3f Ka;               // 环境光颜色（12 字节）
    Vec3f Kd;               // 漫反射颜色（12 字节）
    Vec3f Ks;               // 镜面反射颜色（12 字节）
    float specularExponent; // 高光指数（specular exponent），控制高光反射的集中程度
                            // 值越大，高光范围越小，反射光越集中；值越小，高光范围越大，反射光越分散
                            // 典型取值范围为 1 到 1000，具体值取决于材质的粗糙程度
    float ior = 1.f;              // 折射率（4 字节）
    float alpha = 1.f;             // 透明度（4 字节）

    // 枚举和整数类型
    MaterialType name; // 材质名称（通常是 4 字节）

    // 纹理贴图
    std::string map_Ka;   // 环境光贴图
    std::string map_Kd;   // 漫反射贴图
    std::string map_Ks;   // 镜面反射贴图
    std::string map_Ns;   // 镜面高光贴图
    std::string map_d;    // 透明度贴图
    std::string map_bump; // 凹凸贴图
};

class Object
{
public:
    // 构造函数，接受 MaterialType 参数
    Object(MaterialType type = DIFFUSE_AND_GLOSSY)
    {
        // 根据材质类型初始化 material
        switch (type)
        {
        case DIFFUSE_AND_GLOSSY:
            material.name = DIFFUSE_AND_GLOSSY;
            material.Ka = Vec3f{0.1f, 0.1f, 0.1f}; // 环境光颜色
            material.Kd = Vec3f{0.8f, 0.8f, 0.8f}; // 漫反射颜色
            material.Ks = Vec3f{0.2f, 0.2f, 0.2f}; // 镜面反射颜色
            material.specularExponent = 25.0f;     // 镜面反射指数
            material.ior = 1.0f;                   // 折射率
            material.alpha = 1.0f;                 // 完全不透明
            break;

        case REFLECTION_AND_REFRACTION:
            material.name = REFLECTION_AND_REFRACTION;
            material.Ka = Vec3f{0.1f, 0.1f, 0.1f}; // 环境光颜色
            material.Kd = Vec3f{0.0f, 0.0f, 0.0f}; // 漫反射颜色（无漫反射）
            material.Ks = Vec3f{0.9f, 0.9f, 0.9f}; // 镜面反射颜色
            material.specularExponent = 100.0f;    // 高镜面反射指数
            material.ior = 1.5f;                   // 折射率（例如玻璃）
            material.alpha = 0.5f;                 // 半透明
            break;

        case REFLECTION:
            material.name = REFLECTION;
            material.Ka = Vec3f{0.1f, 0.1f, 0.1f}; // 环境光颜色
            material.Kd = Vec3f{0.0f, 0.0f, 0.0f}; // 漫反射颜色（无漫反射）
            material.Ks = Vec3f{0.9f, 0.9f, 0.9f}; // 镜面反射颜色
            material.specularExponent = 100.0f;    // 高镜面反射指数
            material.ior = 1.0f;                   // 折射率（无折射）
            material.alpha = 1.0f;                 // 完全不透明
            break;
        }
    }

    // 构造函数，接受 Material 对象
    Object(const Material &mat) : material(mat) {}

    virtual ~Object() = default;

    // virtual bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &, Vec2f &) const = 0;

    const Material &getSurfaceProperties() const { return material; }

    // material properties
    Material material;
};

class MeshTriangle : public Object
{
public:
    // 构造函数，接受三角形列表和 MaterialType
    MeshTriangle(std::vector<Triangle> &TriangleList, MaterialType type = DIFFUSE_AND_GLOSSY)
        : Object(type), Triangles(TriangleList) {}

    // 构造函数，接受三角形列表和 Material 对象
    MeshTriangle(std::vector<Triangle> &TriangleList, const Material &mat)
        : Object(mat), Triangles(TriangleList) {}

    std::vector<Triangle> &Triangles;
};
