#include <algorithm>
#include "rasterizer.h"

rst::rasterizer::rasterizer(int w, int h, const std::string &format) : width(w), height(h)
{
    image = std::make_unique<Image>(w, h, format);
    depth_buf.resize(w * h, -std::numeric_limits<float>::infinity()); // 初始化为负无穷大
    texture = std::nullopt;
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

rst::col_buf_id rst::rasterizer::load_normals(const std::vector<Vec3f>& normals)
{
    auto id = get_next_id();
    nor_buf.emplace(id, normals);

    normal_id = id;

    return {id};
}

void rst::rasterizer::set_model(const Matrix4f &m) { model = m; }

void rst::rasterizer::set_view(const Matrix4f &v) { view = v; }

void rst::rasterizer::set_projection(const Matrix4f &p) { projection = p; }

void rst::rasterizer::set_texture(const Texture& tex) { texture = tex; }

void rst::rasterizer::set_antialiasing(std::function<Vec3f(int x, int y, const Triangle &t)> aa) { antiAliasing = aa; };

// void rst::rasterizer::set_vertex_shader(std::function<Vec3f(vertex_shader_payload)> vert_shader)
// {
//     vertex_shader = vert_shader;
// }

// void rst::rasterizer::set_fragment_shader(std::function<Vec3f(fragment_shader_payload)> frag_shader)
// {
//     fragment_shader = frag_shader;
// }

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

// Bresenham's line drawing algorithm
void rst::rasterizer::draw_line(const Vec3f& begin, const Vec3f& end, const Color &color)
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

    if (dx > dy) {
        int err = dy * 2 - dx;
        for (; x != x_end; x += sx) {
            image->set(x, y, color);
            if (err > 0) {
                y += sy;
                err -= dx * 2;
            }
            err += dy * 2;
        }
        image->set(x, y, color);
    } else {
        int err = dx * 2 - dy;
        for (; y != y_end; y += sy) {
            image->set(x, y, color);
            if (err > 0) {
                x += sx;
                err -= dy * 2;
            }
            err += dx * 2;
        }
        image->set(x, y, color);
    }

    // float x0 = begin.x;
    // float y0 = begin.y;
    // float x1 = end.x;
    // float y1 = end.y;

    // if (x0 == x1 && y0 == y1) // 线段起点和终点重合，直接绘制一个点
    // {
    //     image->set(static_cast<int>(x0), static_cast<int>(y0), color);
    //     return;
    // }

    // // Breseham 算法绘制线段
    // bool steep = false; // 标记当前斜率的绝对值是否大于1
    // if (std::abs(x0 - x1) < std::abs(y0 - y1))
    // {
    //     // 斜率绝对值>1了，此时将线段端点各自的x,y坐标对调。
    //     std::swap(x0, y0);
    //     std::swap(x1, y1);
    //     steep = true;
    // }
    // if (x0 > x1)
    // { // x0>x1时，对线段端点坐标进行对调
    //     std::swap(x0, x1);
    //     std::swap(y0, y1);
    // }

    // int dx = static_cast<int>(x1 - x0);
    // int dy = static_cast<int>(y1 - y0);
    // int derror2 = std::abs(dy) * 2;
    // int error2 = 0;

    // for (int x = x0, y = y0; x <= x1; x++)
    // {
    //     if (steep)
    //     {
    //         // 如果线段是斜率大于1的，那么线段上的点原本坐标应该是(y,x)
    //         image->set(y, x, color);
    //     }
    //     else
    //     {
    //         image->set(x, y, color);
    //     }
    //     error2 += derror2;
    //     if (error2 > dx)
    //     {
    //         y += (y1 > y0 ? 1 : -1);
    //         error2 -= dx * 2;
    //     }
    // }
}

void rst::rasterizer::draw(std::vector<Triangle *> &TriangleList) {

    float f1 = (50 - 0.1f) / 2.0f;
    float f2 = (50 + 0.1f) / 2.0f;

    Matrix4f mvp = projection * view * model;
    for (const auto& t:TriangleList)
    {
        Triangle newtri = *t;

        auto t_Vec4 = t->toVector4(t->vertex, 1.f);
        std::array<Vec4f, 3> mm{
            (view * model * t_Vec4[0]),
            (view * model * t_Vec4[1]),
            (view * model * t_Vec4[2])};

        std::array<Vec3f, 3> viewspace_pos;
        std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto& v) {
            return v.template head<3>();
        });

        std::array<Vec4f, 3> v = {
            mvp * t_Vec4[0],
            mvp * t_Vec4[1],
            mvp * t_Vec4[2]};

        //Homogeneous division
        for (auto &vec : v)
        {
            vec.x/=vec.w;
            vec.y/=vec.w;
            vec.z/=vec.w;
        }

        Matrix4f inv_trans = (view * model).inverse().transpose();
        std::array<Vec4f, 3> n = t->toVector4(t->normal, 0.f);

        //Viewport transformation
        for (auto &vert : v)
        {
            vert.x = static_cast<float>(0.5 * width * (vert.x + 1.0));
            vert.y = static_cast<float>(0.5 * height * (vert.y + 1.0));
            vert.z = static_cast<float>(vert.z * f1 + f2);
        }

        for (int i = 0; i < 3; ++i)
        {
            //screen space coordinates
            newtri.setVertex(i, v[i].head<3>());
        }

        for (int i = 0; i < 3; ++i)
        {
            //view space normal
            newtri.setNormal(i, n[i].head<3>());
        }

        newtri.setColor(0, 148,121.0,92.0);
        newtri.setColor(1, 148,121.0,92.0);
        newtri.setColor(2, 148,121.0,92.0);

        // Also pass view space vertice position
        // rasterize_triangle(newtri, viewspace_pos);
    }
}


//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle &t,  Color &color)
{
    // Find the bounding box of the current triangle
    auto [min_x, min_y, max_x, max_y] = t.getBoundingBox();

    // Iterate through the pixels and find if the current pixel is inside the triangle
    for (int x = min_x; x < max_x; ++x)
    {
        for (int y = min_y; y < max_y; ++y)
        {
            auto [alpha, beta, gamma] = t.computeBarycentric2D(Vec2f{static_cast<float>(x), static_cast<float>(y)}); // 计算当前像素点的重心坐标
            auto z_interpolated = t.vertex[0].z * alpha + t.vertex[1].z * beta + t.vertex[2].z * gamma; // 插值计算该点的深度值

            // NO Anti-Aliasing
            // auto ind = get_index(x, y);
            // if (z_interpolated < depth_buf[ind])
            // {
            //     continue;
            // }

            // if (!t.insideTriangle(Vec3f{static_cast<float>(x), static_cast<float>(y), 1.0f})) {
            //     continue;
            // }
            // depth_buf[ind] = z_interpolated; // 更新z-buffer
            // image->set(x, y, color);
            
            //Super-sampling for anti-aliasing
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
            if (coverage > 0.0f)
            {
                auto ind = get_index(x, y);
                if (z_interpolated < depth_buf[ind]) {
                    continue;
                }
                color.setAlpha(color.getAlpha() * coverage);
                depth_buf[ind] = z_interpolated; // 更新z-buffer
                image->set(x, y, color);
            }
        }
    }
}
