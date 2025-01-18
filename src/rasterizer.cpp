#include <algorithm>
#include "rasterizer.h"

rst::rasterizer::rasterizer(int w, int h, const std::string &format) : width(w), height(h)
{
    image = std::make_unique<Image>(w, h, format);
    depth_buf.resize(w * h, -std::numeric_limits<float>::infinity()); // 初始化为负无穷大
}

rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Vec3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Vec3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Vec3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_normals(const std::vector<Vec3f> &normals)
{
    auto id = get_next_id();
    nor_buf.emplace(id, normals);

    normal_id = id;

    return {id};
}

void rst::rasterizer::set_model(const Vec3f &translate, const Vec3f &rotate, const Vec3f &scale)
{
    // 将角度转换为弧度
    float rx = static_cast<float> (rotate.x * MY_PI / 180.f);
    float ry = static_cast<float> (rotate.y * MY_PI / 180.f);
    float rz = static_cast<float> (rotate.z * MY_PI / 180.f);

    // 绕 X 轴旋转
    Matrix4f rotate_x{
        1, 0, 0, 0,
        0, cos(rx), -sin(rx), 0,
        0, sin(rx), cos(rx), 0,
        0, 0, 0, 1};

    // 绕 Y 轴旋转
    Matrix4f rotate_y{
        cos(ry), 0, sin(ry), 0,
        0, 1, 0, 0,
        -sin(ry), 0, cos(ry), 0,
        0, 0, 0, 1};

    // 绕 Z 轴旋转
    Matrix4f rotate_z{
        cos(rz), -sin(rz), 0, 0,
        sin(rz), cos(rz), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

    // 缩放矩阵
    Matrix4f scale_mat{
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1};

    // 平移矩阵
    Matrix4f translate_mat{
        1, 0, 0, translate.x,
        0, 1, 0, translate.y,
        0, 0, 1, translate.z,
        0, 0, 0, 1};

    // 组合变换：先缩放，再旋转，最后平移
    model = translate_mat * rotate_z * rotate_y * rotate_x * scale_mat;
}

void rst::rasterizer::set_view(const Vec3f &eye_pos, const Vec3f &target_pos, const Vec3f &up_dir)
{
    // 计算相机的三个轴
    Vec3f Z = (eye_pos - target_pos).normalize(); // 相机的前向向量 (看向-z轴)
    Vec3f X = (up_dir ^ Z).normalize();           // 相机的右向向量
    Vec3f Y = (Z ^ X).normalize();           // 相机的上向向量

    // 视图矩阵
    view = Matrix4f{
        X.x, X.y, X.z, -(X * eye_pos),
        Y.x, Y.y, Y.z, -(Y * eye_pos),
        Z.x, Z.y, Z.z, -(Z * eye_pos),
        0, 0, 0, 1};
}

void rst::rasterizer::set_projection(float eye_fov, const float &aspect_ratio, const float &zNear, const float &zFar)
{
    // 将角度转换为弧度
    eye_fov *= static_cast<float> (MY_PI / 180.f);

    // 计算投影矩阵的参数
    float ty = 1 / tan(eye_fov / 2); // 垂直方向的缩放因子
    float tx = ty / aspect_ratio;    // 水平方向的缩放因子
    float zRange = zFar - zNear; // 深度范围

    // 透视投影矩阵
    projection = Matrix4f{
        tx, 0, 0, 0,
        0, ty, 0, 0,
        0, 0,  - (zNear + zFar) / zRange, (-2 * zNear * zFar) / zRange,
        0, 0, -1, 0};
}

void rst::rasterizer::set_pixel(const Vec2i &point, const Vec3f &color)
{
    const int &x = point.x;
    const int &y = point.y;
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    image->frame_buf[get_index(x, y)] = color;
}

void rst::rasterizer::clearBuff(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(image->frame_buf.begin(), image->frame_buf.end(), Vec3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), -std::numeric_limits<float>::infinity());
    }
}

void rst::rasterizer::show() const {
    // 检查 cv_image 是否为空
    if (image->cv_image.empty())
    {
        LOGE("Image data is empty. Cannot display.");
        return;
    }

    cv::imshow("image", image->cv_image);
}

// Bresenham's line drawing algorithm
void rst::rasterizer::draw_line(const Vec3f &begin, const Vec3f &end, const Color &color)
{
    int x_start = static_cast<int>(begin.x + 0.5f);
    int y_start = static_cast<int>(begin.y + 0.5f);
    int x_end = static_cast<int>(end.x + 0.5f);
    int y_end = static_cast<int>(end.y + 0.5f);

    int dx = abs(x_end - x_start);
    int dy = abs(y_end - y_start);
    int sx = (x_start < x_end) ? 1 : -1;
    int sy = (y_start < y_end) ? 1 : -1;

    int x = x_start;
    int y = y_start;

    if (dx > dy)
    {
        int err = dy * 2 - dx;
        for (; x != x_end; x += sx)
        {
            image->set(x, y, color);
            if (err > 0)
            {
                y += sy;
                err -= dx * 2;
            }
            err += dy * 2;
        }
        image->set(x, y, color);
    }
    else
    {
        int err = dx * 2 - dy;
        for (; y != y_end; y += sy)
        {
            image->set(x, y, color);
            if (err > 0)
            {
                x += sx;
                err -= dy * 2;
            }
            err += dx * 2;
        }
        image->set(x, y, color);
    }
}

// Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle &t)
{
    // 此时三角形已经是屏幕空间三角形
    auto [min_x, min_y, max_x, max_y] = t.getBoundingBox(); // Find the bounding box of the current triangle
    auto &view_pos = t.viewspace_pos; // 顶点在视空间中的坐标
    auto interpolate = [](float alpha, float beta, float gamma, const auto &array)
    { return (alpha * array[0] + beta * array[1] + gamma * array[2]); }; // 对三角形各项属性做插值

    // Iterate through the pixels and find if the current pixel is inside the triangle
    for (int x = min_x; x < max_x; ++x)
    {
        for (int y = min_y; y < max_y; ++y)
        {
            if (x < 0 || x >= width || y < 0 || y >= height)
                continue;
            
            auto [alpha, beta, gamma] = t.computeBarycentric2D(Vec2f{static_cast<float>(x), static_cast<float>(y)}); // 计算当前像素点的重心坐标
            float z_corrected = 1.0f / (alpha / view_pos[0].z + beta / view_pos[1].z + gamma / view_pos[2].z);       // view_pos[i].z is the vertex view space depth value z.
            // 对重心坐标做透视校正
            float a_corrected = alpha / view_pos[0].z * z_corrected;
            float b_corrected = beta / view_pos[1].z * z_corrected;
            float g_corrected = gamma / view_pos[2].z * z_corrected;
            // 不做透视校正
            // float a_corrected = alpha;
            // float b_corrected = beta;
            // float g_corrected = gamma;

            float z_interpolated = z_corrected;
            auto ind = get_index(x, y);

            // NO Anti-Aliasing

            // if (z_interpolated < depth_buf[ind])
            // {
            //     continue;
            // }

            // if (!t.insideTriangle(Vec3f{static_cast<float>(x), static_cast<float>(y), 1.0f})) {
            //     continue;
            // }
            // depth_buf[ind] = z_interpolated; // 更新z-buffer

            // auto interpolated_color = t.interpolate(alpha, beta, gamma, t.color);
            // auto interpolated_normal = t.interpolate(alpha, beta, gamma, t.normal);
            // auto interpolated_texcoords = t.interpolate(alpha, beta, gamma, t.tex_coords);

            // fragment_shader_payload payload(interpolated_color, interpolated_normal.normalize(), interpolated_texcoords, texture ? &*texture : nullptr);
            // auto pixel_color = fragment_shader(payload);

            // image->set(x, y, Color(pixel_color));

            // Super-sampling for anti-aliasing
            auto ssaa = [](const Triangle &t, int x, int y, int samples) -> float
            {
                float coverage = 0.0f;
                for (int i = 0; i < samples; ++i)
                {
                    for (int j = 0; j < samples; ++j)
                    {
                        float sub_x = x + (i + 0.5f) / samples;
                        float sub_y = y + (j + 0.5f) / samples;
                        if (t.insideTriangle(Vec3f{sub_x, sub_y, 1.0f}))
                        {
                            coverage += 1.0f / (samples * samples);
                        }
                    }
                }
                return coverage;
            };

            const int samples = 4; // 2x2 super-sampling
            float coverage = ssaa(t, x, y, samples);
            // Set the pixel color based on coverage
            if (coverage > 0.0f && z_interpolated > depth_buf[ind])
            {
                depth_buf[ind] = z_interpolated; // 更新z-buffer

                payload.color = interpolate(a_corrected, b_corrected, g_corrected, t.color);
                payload.normal = interpolate(a_corrected, b_corrected, g_corrected, t.normal).normalize(); // 确保法线是单位向量
                payload.tex_coords = interpolate(a_corrected, b_corrected, g_corrected, t.tex_coords);
                payload.view_pos = interpolate(a_corrected, b_corrected, g_corrected, view_pos);

                auto pixel_color = fragment_shader(payload);
                image->set(x, y, Color(pixel_color));
            }
        }
    }
}

void rst::rasterizer::draw_obj(const std::unique_ptr<Object> &obj)
{
    // 获取物体的三角形列表
    auto mesh = dynamic_cast<MeshTriangle *>(obj.get());
    auto TriangleList = mesh->Triangles;

    Matrix4f mvp = projection * view * model;
    for (auto &t : TriangleList)
    {
        auto t_Vec4 = t.toVector4(t.vertex, 1.f);

        t.viewspace_pos = {  
            (view * model * t_Vec4[0]).head<3>(),
            (view * model * t_Vec4[1]).head<3>(),
            (view * model * t_Vec4[2]).head<3>()};

        // 计算法线矩阵（模型矩阵的逆转置矩阵）
        Matrix4f inv_trans = (view * model).inverse().transpose();
        for (int i = 0; i < 3; ++i)
        {
            // 将顶点从模型空间转换到裁剪空间
            Vec4f v = mvp * t.vertex[i].toVector4(1.0f);
            Vec4f n = inv_trans * t.normal[i].toVector4(0.0f); // 对法线进行变换
            // 透视除法，将裁剪空间坐标转换到标准化设备坐标（NDC）
            v = v / v.w();
            v.x = (v.x + 1.0f) * 0.5f * width;
            v.y = (v.y + 1.0f) * 0.5f * height;
            // v.z没啥作用了

            // 将 NDC 坐标转换到屏幕空间
            t.setVertex(i, v.head<3>());
            t.setNormal(i, n.head<3>());
        }

        // 调用 rasterize_triangle 函数进行光栅化
        rasterize_triangle(t);
    }
}

void rst::rasterizer::draw() {
    if (scene == nullptr) {
        LOGE("No scene loaded!");
        return;
    }

    // 获取场景中的相机
    const auto &camera = scene->get_camera();
    if (!camera)
    {
        LOGE("No camera in the scene!");
        return;
    }
    // 深拷贝 lights 和 camera
    payload.camera = *(scene->get_camera());
    payload.lights.clear();
    for (const auto &light : scene->get_lights())
    {
        payload.lights.push_back(*light);
    }

    // 设置视图变换
    set_view(camera->eye_pos, camera->target_pos, camera->up_dir);

    // 设置投影矩阵
    set_projection(scene->get_filedofView(), scene->get_aspect_ratio(), -1.f, -50.0f);

    // 遍历场景中的所有物体
    for (const auto &obj : scene->get_objects())
    {
        // 设置片段着色器载体
        payload.material = obj->material;
        payload.texture = texture.has_value() ? &texture.value() : nullptr;
        draw_obj(obj);
    }

    // 将图像结果写入image
    auto data = image->frame_buf.data();
    image->cv_image = cv::Mat(height, width, CV_32FC3, reinterpret_cast<float *>(data));

    image->cv_image.convertTo(image->cv_image, CV_8UC3, 255.0f);

    cv::cvtColor(image->cv_image, image->cv_image, cv::COLOR_RGB2BGR);
}