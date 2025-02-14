#include <algorithm>
#include <barrier>
#include "rasterizer.h"

rst::rasterizer::rasterizer(int w, int h, const std::string &format) : width(w), height(h)
{
    image = std::make_unique<Image>(w, h, format);
    depth_buf.resize(w * h, -std::numeric_limits<float>::infinity()); // 初始化为负无穷大
    back_buf.resize(w * h, 0);

    super_depth_buf.resize(w * h * samples * samples, -std::numeric_limits<float>::infinity());
    super_back_buf.resize(w * h * samples * samples, 0);
}

void rst::rasterizer::set_model(const Vec3f &translate, const Vec3f &rotate, const Vec3f &scale)
{
    // 将角度转换为弧度
    float rx = static_cast<float>(rotate.x * MY_PI / 180.f);
    float ry = static_cast<float>(rotate.y * MY_PI / 180.f);
    float rz = static_cast<float>(rotate.z * MY_PI / 180.f);

    // 计算三角函数值
    float cos_x = cos(rx), sin_x = sin(rx);
    float cos_y = cos(ry), sin_y = sin(ry);
    float cos_z = cos(rz), sin_z = sin(rz);

    // 计算组合变换矩阵的元素
    float m11 = cos_y * cos_z * scale.x;
    float m12 = (sin_x * sin_y * cos_z - cos_x * sin_z) * scale.y;
    float m13 = (cos_x * sin_y * cos_z + sin_x * sin_z) * scale.z;
    float m14 = translate.x;

    float m21 = cos_y * sin_z * scale.x;
    float m22 = (sin_x * sin_y * sin_z + cos_x * cos_z) * scale.y;
    float m23 = (cos_x * sin_y * sin_z - sin_x * cos_z) * scale.z;
    float m24 = translate.y;

    float m31 = -sin_y * scale.x;
    float m32 = sin_x * cos_y * scale.y;
    float m33 = cos_x * cos_y * scale.z;
    float m34 = translate.z;

    // 构建最终的组合变换矩阵
    vertex_payload.model = Matrix4f{
        m11, m12, m13, m14,
        m21, m22, m23, m24,
        m31, m32, m33, m34,
        0, 0, 0, 1};

    // // 绕 X 轴旋转
    // Matrix4f rotate_x{
    //     1, 0, 0, 0,
    //     0, cos(rx), -sin(rx), 0,
    //     0, sin(rx), cos(rx), 0,
    //     0, 0, 0, 1};

    // // 绕 Y 轴旋转
    // Matrix4f rotate_y{
    //     cos(ry), 0, sin(ry), 0,
    //     0, 1, 0, 0,
    //     -sin(ry), 0, cos(ry), 0,
    //     0, 0, 0, 1};

    // // 绕 Z 轴旋转
    // Matrix4f rotate_z{
    //     cos(rz), -sin(rz), 0, 0,
    //     sin(rz), cos(rz), 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1};

    // // 缩放矩阵
    // Matrix4f scale_mat{
    //     scale.x, 0, 0, 0,
    //     0, scale.y, 0, 0,
    //     0, 0, scale.z, 0,
    //     0, 0, 0, 1};

    // // 平移矩阵
    // Matrix4f translate_mat{
    //     1, 0, 0, translate.x,
    //     0, 1, 0, translate.y,
    //     0, 0, 1, translate.z,
    //     0, 0, 0, 1};

    // // 组合变换：先缩放，再旋转，最后平移
    // vertex_payload.model = translate_mat * rotate_z * rotate_y * rotate_x * scale_mat;
}

void rst::rasterizer::set_view(const Vec3f &eye_pos, const Vec3f &target_pos, const Vec3f &up_dir)
{
    // 计算相机的三个轴
    Vec3f Z = (eye_pos - target_pos).normalize(); // 相机的前向向量 (看向-z轴)
    Vec3f X = (up_dir ^ Z).normalize();           // 相机的右向向量
    Vec3f Y = (Z ^ X).normalize();                // 相机的上向向量

    // 视图矩阵
    vertex_payload.view = Matrix4f{
        X.x, X.y, X.z, -(X * eye_pos),
        Y.x, Y.y, Y.z, -(Y * eye_pos),
        Z.x, Z.y, Z.z, -(Z * eye_pos),
        0, 0, 0, 1};
}

void rst::rasterizer::set_projection(float eye_fov, const float &aspect_ratio, const float &zNear, const float &zFar)
{
    // 将角度转换为弧度
    eye_fov *= static_cast<float>(MY_PI / 180.f);

    // 计算投影矩阵的参数
    float ty = 1 / tan(eye_fov / 2); // 垂直方向的缩放因子
    float tx = ty / aspect_ratio;    // 水平方向的缩放因子
    float zRange = zFar - zNear;     // 深度范围

    // 透视投影矩阵
    vertex_payload.projection = Matrix4f{
        tx, 0, 0, 0,
        0, ty, 0, 0,
        0, 0, -(zNear + zFar) / zRange, (-2 * zNear * zFar) / zRange,
        0, 0, -1, 0};
}

void rst::rasterizer::set_pixel(const Vec2i &point, const Vec3f &color)
{
    const int &x = point.x;
    const int &y = point.y;
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    back_buf[get_index(x, y)] = color;
}

void rst::rasterizer::clearBuff(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(image->frame_buf.begin(), image->frame_buf.end(), Vec3f{0, 0, 0});
        std::fill(back_buf.begin(), back_buf.end(), Vec3f{0, 0, 0});
        std::fill(super_back_buf.begin(), super_back_buf.end(), Vec3f{0, 0, 0});
        triangleCount = 0;
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), -std::numeric_limits<float>::infinity());
        std::fill(super_depth_buf.begin(), super_depth_buf.end(), -std::numeric_limits<float>::infinity());
    }
}

void rst::rasterizer::show() const
{
    // 检查 cv_image 是否为空
    if (image->frame_buf.empty())
    {
        LOGE("Image data is empty. Cannot display.");
        return;
    }


    // 创建窗口标题，显示三角形面数
    std::string windowTitle = "Render Window - Triangles: " + std::to_string(triangleCount);

    // 更新窗口标题
    cv::setWindowTitle("Render Window", windowTitle);

    cv::imshow("Render Window", image->cv_image);
}

void rst::rasterizer::draw_point_triangle(Triangle &t)
{
    vertex_payload.triangle = &t;
    vertex_shader(vertex_payload);
    for (int i = 0; i < 3; ++i)
    {
        // 将裁剪空间坐标转换到标准化设备坐标（NDC）
        t.vertex[i].x = (t.vertex[i].x + 1.0f) * 0.5f * width;
        t.vertex[i].y = (t.vertex[i].y + 1.0f) * 0.5f * height;
    }

    auto color = Color{255, 255, 255};
    draw_point(t.vertex[0].head<2>(), color);
    draw_point(t.vertex[1].head<2>(), color);
    draw_point(t.vertex[2].head<2>(), color);
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
            {
                std::lock_guard<std::mutex> lock_image(image_mutex); // image_mutex是保护帧缓冲区的互斥锁
                set_pixel({x, y}, color.getVec());
            }
            if (err > 0)
            {
                y += sy;
                err -= dx * 2;
            }
            err += dy * 2;
        }
        {
            std::lock_guard<std::mutex> lock_image(image_mutex); // image_mutex是保护帧缓冲区的互斥锁
            set_pixel({x, y}, color.getVec());
        }
    }
    else
    {
        int err = dx * 2 - dy;
        for (; y != y_end; y += sy)
        {
            {
                std::lock_guard<std::mutex> lock_image(image_mutex); // image_mutex是保护帧缓冲区的互斥锁
                set_pixel({x, y}, color.getVec());
            }
            if (err > 0)
            {
                x += sx;
                err -= dy * 2;
            }
            err += dx * 2;
        }
        {
            std::lock_guard<std::mutex> lock_image(image_mutex); // image_mutex是保护帧缓冲区的互斥锁
            set_pixel({x, y}, color.getVec());
        }
    }
}

void rst::rasterizer::draw_triangle_line(Triangle &t) {
    vertex_payload.triangle = &t;
    vertex_shader(vertex_payload);
    for (int i = 0; i < 3; ++i)
    {
        // 将裁剪空间坐标转换到标准化设备坐标（NDC）
        t.vertex[i].x = (t.vertex[i].x + 1.0f) * 0.5f * width;
        t.vertex[i].y = (t.vertex[i].y + 1.0f) * 0.5f * height;
    }

    auto color = Color{255, 255, 255};
    draw_line(t.a(), t.b(), color);
    draw_line(t.b(), t.c(), color);
    draw_line(t.c(), t.a(), color);
    ++triangleCount;
}

// Screen space rasterization
void rst::rasterizer::rasterize_triangle(Triangle &t)
{
    vertex_payload.triangle = &t;
    vertex_shader(vertex_payload); // 顶点着色器
    
    for (int i = 0; i < 3; ++i)
    {
        // 将裁剪空间坐标转换到标准化设备坐标（NDC）
        t.vertex[i].x = (t.vertex[i].x + 1.0f) * 0.5f * width;
        t.vertex[i].y = (t.vertex[i].y + 1.0f) * 0.5f * height;
    }
    t.update();       // 更新三角形的边向量

    if (t.double_area2D <= 1e-3f) // 三角形面积为负，背面剔除
        return;

    // 此时三角形已经是屏幕空间三角形
    auto [min_x, min_y, max_x, max_y] = t.getBoundingBox();
    auto &view_pos = t.viewspace_pos; // 顶点在视空间中的坐标

    auto interpolate = [](float alpha, float beta, float gamma, const auto &array)
    { return (alpha * array[0] + beta * array[1] + gamma * array[2]); }; // 对三角形各项属性做插值

    auto pixel_render = [&](float x, float y, int ind) -> Vec3f
    {
        if (!t.insideTriangle(Vec3f{x, y, 1.0f}))
        {
            return {0.f, 0.f, 0.f};
        }
        auto [alpha, beta, gamma] = t.computeBarycentric2D(Vec2f{static_cast<float>(x), static_cast<float>(y)}); // 计算像素点的重心坐标
        float z_corrected = 1.0f / (alpha / view_pos[0].z + beta / view_pos[1].z + gamma / view_pos[2].z);

        // 对重心坐标做透视校正
        float a_corrected = alpha / view_pos[0].z * z_corrected;
        float b_corrected = beta / view_pos[1].z * z_corrected;
        float g_corrected = gamma / view_pos[2].z * z_corrected;

        float z_interpolated = z_corrected;

        if (!anti_Aliasing)
        {
            std::lock_guard<std::mutex> lock_image(depth_mutex); // 保护深度缓冲区
            if (z_interpolated < depth_buf[ind])
            {
                return {0.f, 0.f, 0.f};
            }
            depth_buf[ind] = z_interpolated; // 更新z-buffer
        }else {
            std::lock_guard<std::mutex> lock_image(depth_mutex); // 保护深度缓冲区
            if (z_interpolated < super_depth_buf[ind])
            {
                return {0.f, 0.f, 0.f};
            }
            super_depth_buf[ind] = z_interpolated; // 更新z-buffer
        }


        fragment_payload.color = interpolate(a_corrected, b_corrected, g_corrected, t.color);
        fragment_payload.normal = interpolate(a_corrected, b_corrected, g_corrected, t.normal).normalize(); // 确保法线是单位向量
        fragment_payload.tex_coords = interpolate(a_corrected, b_corrected, g_corrected, t.tex_coords);
        fragment_payload.view_pos = interpolate(a_corrected, b_corrected, g_corrected, view_pos);

        return fragment_shader(fragment_payload);
    };

    for (int y = min_y; y < max_y; ++y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            if (x < 0 || x >= width || y < 0 || y >= height)
                continue;
            Vec3f pixel_color{0.f, 0.f, 0.f};
            if (!anti_Aliasing) {
                // NO Anti-Aliasing
                pixel_color = pixel_render(x + 0.5f, y + 0.5f, get_index(x, y));
                if (pixel_color.x == 0.f)
                    continue;

            }else {
                // 在这里实现抗锯齿
                samples = 2; // 2x2 mutil-sampling
                auto ind = get_index(x, y) * samples * samples;
  
                for (int i = 0; i < samples; ++i)
                {
                    for (int j = 0; j < samples; ++j)
                    {
                        int index = ind + i * samples + j;
                        float sub_x = x + (i + 0.5f) / samples;
                        float sub_y = y + (j + 0.5f) / samples;
                        auto sub_pixel_color = pixel_render(sub_x, sub_y, index);
                        if (sub_pixel_color.x == 0.f) {
                            continue;
                        }
                        super_back_buf[index] = sub_pixel_color;
                    }
                }
                pixel_color = (super_back_buf[ind] + super_back_buf[ind + 1] + super_back_buf[ind + 2] + super_back_buf[ind + 3]) / 4;
            }
            
            {
                std::lock_guard<std::mutex> lock_image(image_mutex); // 保护帧缓冲区
                set_pixel({x, y}, pixel_color / 255.f);
            }
        }
    }
    ++triangleCount;
}

void rst::rasterizer::rasterize_triangle_list(const std::vector<Triangle> &triangles) {

    if (!multithreading)
    {
        for (auto t : triangles)
        {
            if (renderMode == FACE)
                rasterize_triangle(t);
            else if (renderMode == EDGE)
                draw_triangle_line(t);
            else if (renderMode == VERTEX)
                draw_point_triangle(t);
        }
    }
    else
    {
        // 尝试多线程渲染
        std::queue<Triangle> taskQueue;
        // 将带渲染三角形添加到队列
        for (const auto &t : triangles)
        {
            taskQueue.push(t);
        }

        // 工作线程函数
        auto worker = [&]()
        {
            while (true)
            {
                Triangle t;
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    if (taskQueue.empty())
                        break;
                    t = taskQueue.front();
                    taskQueue.pop();
                }
                if (renderMode == FACE)
                    rasterize_triangle(t);
                else if (renderMode == EDGE)
                    draw_triangle_line(t);
                else if (renderMode == VERTEX)
                    draw_point_triangle(t);
            }
        };

        // 启动线程
        // const size_t num_threads = std::thread::hardware_concurrency();
        const size_t num_threads = 8;
        std::vector<std::thread> threads;
        for (size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(worker);
        }

        // 等待线程结束
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    std::swap(back_buf, image->frame_buf);
}

void rst::rasterizer::draw_obj(const std::unique_ptr<Object> &obj)
{
    // 获取物体的三角形列表
    auto mesh = dynamic_cast<MeshTriangle *>(obj.get());
    if (!mesh)
        return; // 确保类型转换成功

    rasterize_triangle_list(mesh->Triangles);
}

void rst::rasterizer::draw()
{
    if (scene == nullptr)
    {
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
    fragment_payload.camera = *(scene->get_camera());
    fragment_payload.lights.clear();
    for (const auto &light : scene->get_lights())
    {
        fragment_payload.lights.push_back(*light);
    }

    // 设置视图变换
    set_view(camera->eye_pos, camera->target_pos, camera->up_dir);

    // 设置投影矩阵
    set_projection(scene->get_filedofView(), scene->get_aspect_ratio(), -1.f, -50.0f);

    vertex_payload.mvp = vertex_payload.projection * vertex_payload.view * vertex_payload.model;
    vertex_payload.inv_trans = (vertex_payload.view * vertex_payload.model).inverse().transpose();

    clearBuff(rst::Buffers::Color | rst::Buffers::Depth); // 清空缓冲区
    // 遍历场景中的所有物体
    for (const auto &obj : scene->get_objects())
    {
        // 设置片段着色器载体
        fragment_payload.material = obj->material;
        fragment_payload.texture = texture.has_value() ? &texture.value() : nullptr;
        draw_obj(obj);
    }

    // 将图像结果写入image
    auto data = image->frame_buf.data();
    image->cv_image = cv::Mat(height, width, CV_32FC3, reinterpret_cast<float *>(data));

    image->cv_image.convertTo(image->cv_image, CV_8UC3, 255.0f);

    cv::cvtColor(image->cv_image, image->cv_image, cv::COLOR_RGB2BGR);
}