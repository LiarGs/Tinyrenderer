#include "Shader.h"

Vec3f normal_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &ras)
{
    Vec3f result_color = (payload.normal + Vec3f{1.0f, 1.0f, 1.0f}) / 2;

    return result_color * 255;
}

Vec3f white_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &ras)
{
    Vec3f white_color = Vec3f{255.f, 255.f, 255.f};

    // 获取法线和光照信息
    const Vec3f &normal = payload.normal;
    Vec3f result_color = {0.f, 0.f, 0.f};

    // 遍历所有光源
    for (const auto &light : ras.scene->get_lights())
    {
        // 计算光照方向
        Vec3f light_dir = (light->position - payload.view_pos).normalize();

        // 计算漫反射强度
        float diffuse = std::max(0.f, normal * light_dir);

        // 计算光照衰减（假设光源强度随距离平方衰减）
        float r_squared = (light->position - payload.view_pos).norm() * (light->position - payload.view_pos).norm();
        Vec3f light_intensity = light->intensity / r_squared;

        // 叠加光照效果
        result_color += white_color.cwiseProduct(light_intensity) * diffuse;
    }

    // 添加环境光
    result_color += ras.material->Ka.cwiseProduct(payload.amb_light_intensity);

    return result_color;
}

Vec3f phong_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &ras)
{
    Vec3f ka = ras.material->Ka;
    Vec3f kd = payload.color;
    Vec3f ks = ras.material->Ks;

    Vec3f amb_light_intensity = payload.amb_light_intensity; // 环境光强度
    auto eye_pos = ras.scene->get_camera()->eye_pos;         // 相机位置
    float p = ras.material->specularExponent;                // 高光指数

    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    Vec3f result_color = {0, 0, 0};
    for (auto &light : ras.scene->get_lights())
    {
        // 光照方向
        Vec3f light_dir = (light->position - point).normalize();
        // 视线方向
        Vec3f view_dir = (eye_pos - point).normalize();
        // 半程向量
        Vec3f half_dir = (light_dir + view_dir).normalize();

        // 光照衰减
        float r_r = (light->position - point) * (light->position - point);

        // 漫反射
        float diffuse = std::max(0.0f, normal * light_dir);

        // 镜面反射
        float specular = std::pow(std::max(0.0f, normal * half_dir), p);

        // diffuse
        Vec3f ld = kd.cwiseProduct(light->intensity / r_r) * diffuse;
        // specular
        Vec3f ls = ks.cwiseProduct(light->intensity / r_r) * specular;
        // ambient
        Vec3f la = ka.cwiseProduct(amb_light_intensity);

        result_color += (la + ld + ls);
    }

    return result_color * 255;
}

Vec3f texture_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &ras)
{
    Vec3f texture_color = ras.texture.has_value() ? ras.texture->getColor(payload.tex_coords.x, payload.tex_coords.y) : Vec3f{0, 0, 0};

    // 材质属性
    Vec3f ka = ras.material->Ka;      // 环境光系数
    Vec3f kd = texture_color / 255.f; // 漫反射系数
    Vec3f ks = ras.material->Ks;      // 镜面反射系数

    // 相机属性
    Vec3f amb_light_intensity = payload.amb_light_intensity;      // 环境光强度
    auto eye_pos = ras.scene->get_camera()->eye_pos;                // 相机位置
    float p = ras.material->specularExponent; // 高光指数

    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    Vec3f result_color = {0, 0, 0};

    for (auto &light : ras.scene->get_lights())
    {
        // 光照方向
        Vec3f light_dir = (light->position - point).normalize();
        // 视线方向
        Vec3f view_dir = (eye_pos - point).normalize();
        // 半程向量
        Vec3f half_dir = (light_dir + view_dir).normalize();

        // 光照衰减
        float r_r = (light->position - point) * (light->position - point);

        // 漫反射
        float diffuse = std::max(0.0f, normal * light_dir);

        // 镜面反射
        float specular = std::pow(std::max(0.0f, normal * half_dir), p);

        // diffuse
        Vec3f ld = kd.cwiseProduct(light->intensity / r_r) * diffuse;
        // specular
        Vec3f ls = ks.cwiseProduct(light->intensity / r_r) * specular;
        // ambient
        Vec3f la = ka.cwiseProduct(amb_light_intensity);

        result_color += (la + ld + ls);
    }

    return result_color * 255;
}

Vec3f bump_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &ras)
{
    Vec3f normal = payload.normal;

    float kh = 0.2f, kn = 0.1f; // kh 和 kn 是控制凹凸效果的参数

    // 用于从纹理中获取颜色并计算其亮度（norm）
    auto texture_intensity = [&payload, &ras](const float u, const float v)
    {
        return ras.texture.has_value() ? ras.texture->getColor(u, v).norm() : 1.f;
    };

    // 获取当前法线的 x, y, z 分量
    float x = normal.x;
    float y = normal.y;
    float z = normal.z;

    // 获取纹理坐标 u, v
    float u = payload.tex_coords.x;
    float v = payload.tex_coords.y;

    // 获取纹理的宽度和高度
    auto w = ras.texture->width;
    auto h = ras.texture->height;

    // 计算切线向量 tangent
    float sqrt_xz = sqrt(x * x + z * z);
    auto tangent = Vec3f{x * y / sqrt_xz, sqrt_xz, z * y / sqrt_xz};

    // 计算副切线向量 bitangent
    auto bitangent = (normal ^ tangent).normalize();

    // 构建 TBN 矩阵，TBN 矩阵用于将局部坐标系中的向量转换到世界坐标系
    Matrix3f TBN{{tangent, bitangent, normal}};

    // 计算 u 方向的梯度 du，通过纹理坐标的微小变化来计算高度变化
    float du = kh * kn * (texture_intensity(u + 1.f / w, v) - texture_intensity(u, v));

    // 计算 v 方向的梯度 dv
    float dv = kh * kn * (texture_intensity(u, v + 1.f / h) - texture_intensity(u, v));

    // 计算局部法线 ln，根据梯度 du 和 dv 调整法线
    auto ln = Vec3f{-du, -dv, 1.f};
    normal = (TBN * ln).normalize();

    // 返回扰动后的法线向量
    return normal * 255;
}

Vec3f displacement_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &ras)
{
    Vec3f texture_color = ras.texture.has_value() ? ras.texture->getColor(payload.tex_coords.x, payload.tex_coords.y) : Vec3f{0, 0, 0};

    Vec3f ka = ras.material->Ka;
    Vec3f kd = texture_color / 255;
    Vec3f ks = ras.material->Ks;

    // 相机属性
    Vec3f amb_light_intensity = payload.amb_light_intensity; // 环境光强度
    auto eye_pos = ras.scene->get_camera()->eye_pos;         // 相机位置
    float p = ras.material->specularExponent;             // 高光指数

    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    float kh = 0.2f, kn = 0.1f;

    auto texture_intensity = [&payload, &ras](const float u, const float v)
    {
        return ras.texture.has_value() ? ras.texture->getColor(u, v).norm() : 1.f;
    };

    // 获取当前法线的 x, y, z 分量
    float x = normal.x;
    float y = normal.y;
    float z = normal.z;

    // 获取纹理坐标 u, v
    float u = payload.tex_coords.x;
    float v = payload.tex_coords.y;

    // 获取纹理的宽度和高度
    auto w = ras.texture->width;
    auto h = ras.texture->height;

    // 计算切线向量 tangent
    float sqrt_xz = sqrt(x * x + z * z);
    auto tangent = Vec3f{x * y / sqrt_xz, sqrt_xz, z * y / sqrt_xz};

    // 计算副切线向量 bitangent，通过法线和切线的叉积得到
    auto bitangent = (normal ^ tangent).normalize();

    // 构建 TBN 矩阵，TBN 矩阵用于将局部坐标系中的向量转换到世界坐标系
    Matrix3f TBN{{tangent, bitangent, normal}};

    // 计算 u 方向的梯度 du，通过纹理坐标的微小变化来计算高度变化
    float du = kh * kn * (texture_intensity(u + 1.f / w, v) - texture_intensity(u, v));

    // 计算 v 方向的梯度 dv
    float dv = kh * kn * (texture_intensity(u, v + 1.f / h) - texture_intensity(u, v));

    // 计算局部法线 ln，根据梯度 du 和 dv 调整法线
    auto ln = Vec3f{-du, -dv, 1.f};
    normal = (TBN * ln).normalize();

    // 移动顶点高度
    point += kn * normal * texture_intensity(u, v);

    Vec3f result_color = {0, 0, 0};

    for (auto &light : ras.scene->get_lights())
    {
        // 光照方向
        Vec3f light_dir = (light->position - point).normalize();
        // 视线方向
        Vec3f view_dir = (eye_pos - point).normalize();
        // 半程向量
        Vec3f half_dir = (light_dir + view_dir).normalize();

        // 光照衰减
        float r_r = (light->position - point) * (light->position - point);

        // 漫反射
        float diffuse = std::max(0.0f, normal * light_dir);

        // 镜面反射
        float specular = std::pow(std::max(0.0f, normal * half_dir), p);

        // diffuse
        Vec3f ld = kd.cwiseProduct(light->intensity / r_r) * diffuse;
        // specular
        Vec3f ls = ks.cwiseProduct(light->intensity / r_r) * specular;
        // ambient
        Vec3f la = ka.cwiseProduct(amb_light_intensity);

        result_color += (la + ld + ls);
    }

    return result_color * 255;
}

void vertex_shader(rst::vertex_shader_payload &payload, Triangle *t)
{
    auto t_Vec4 = t->toVector4(t->vertex, 1.f);

    t->viewspace_pos = {
        (payload.view * payload.model * t_Vec4[0]).head<3>(),
        (payload.view * payload.model * t_Vec4[1]).head<3>(),
        (payload.view * payload.model * t_Vec4[2]).head<3>()};

    t->normal = {
        (payload.inv_trans * t->normal[0].toVector4(0.0f)).head<3>(),
        (payload.inv_trans * t->normal[1].toVector4(0.0f)).head<3>(),
        (payload.inv_trans * t->normal[2].toVector4(0.0f)).head<3>()};

    for (int i = 0; i < 3; ++i)
    {
        // 将顶点从模型空间转换到裁剪空间（NDC）
        Vec4f v = payload.mvp * t_Vec4[i];
        v /= v.w();

        t->setVertex(i, v.head<3>());
    }
}