#pragma once

#include "Object.hpp"

// 定义材质属性
namespace Materials
{
    // 默认材质
    Material DefaultMaterial()
    {
        Material material;
        material.name = DIFFUSE_AND_GLOSSY;
        material.Ka = Vec3f{0.1f, 0.1f, 0.1f}; // 环境光
        material.Kd = Vec3f{0.8f, 0.8f, 0.8f}; // 漫反射
        material.Ks = Vec3f{0.2f, 0.2f, 0.2f}; // 镜面反射
        material.specularExponent = 25.f;      // 高光指数
        return material;
    }

    // 皮肤材质
    Material SkinMaterial(const std::string &texturePath)
    {
        Material material;
        material.name = DIFFUSE_AND_GLOSSY;
        material.Ka = Vec3f{0.01f, 0.01f, 0.01f}; // 环境光，稍微提高一点以增加皮肤的自然感
        material.Kd = Vec3f{0.9f, 0.7f, 0.6f};    // 漫反射，调整为肉色
        material.Ks = Vec3f{0.1f, 0.1f, 0.1f};    // 镜面反射，降低以模拟皮肤的粗糙表面
        material.specularExponent = 10.f;         // 高光指数，降低以使高光区域更加柔和
        material.map_Kd = Texture(texturePath);            // 漫反射贴图路径
        return material;
    }

    // 创建牛牛的 Material 对象
    Material cowMaterial(const std::string &texturePath) {
        Material material;
        material.name = DIFFUSE_AND_GLOSSY;
        material.Ka = Vec3f{0.005f, 0.005f, 0.005f};
        material.Kd = Vec3f{0.8f, 0.8f, 0.8f};
        material.Ks = Vec3f{0.7937f, 0.7937f, 0.7937f};
        material.specularExponent = 150.f;
        material.map_Kd = Texture(texturePath); // 漫反射贴图路径
        return material;
    }

    // 金属材质
    Material MetalMaterial()
    {
        Material material;
        material.name = REFLECTION;
        material.Ka = Vec3f{0.1f, 0.1f, 0.1f}; // 环境光
        material.Kd = Vec3f{0.0f, 0.0f, 0.0f}; // 漫反射（金属没有漫反射）
        material.Ks = Vec3f{0.9f, 0.9f, 0.9f}; // 镜面反射
        material.specularExponent = 100.f;     // 高光指数
        return material;
    }

    // 玻璃材质
    Material GlassMaterial()
    {
        Material material;
        material.name = REFLECTION_AND_REFRACTION;
        material.Ka = Vec3f{0.1f, 0.1f, 0.1f}; // 环境光
        material.Kd = Vec3f{0.0f, 0.0f, 0.0f}; // 漫反射（玻璃没有漫反射）
        material.Ks = Vec3f{0.9f, 0.9f, 0.9f}; // 镜面反射
        material.specularExponent = 100.f;     // 高光指数
        material.ior = 1.5f;                   // 折射率（玻璃的折射率）
        material.alpha = 0.5f;                 // 透明度
        return material;
    }

} // namespace Materials