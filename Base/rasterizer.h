#pragma once
#include <optional>
#include <algorithm>
#include "Vec.h"
#include "Test.h"
#include "Triangle.h"
#include "Image.h"
#include "DepthTest.h"
#include "Texture.h"
#include "AntiAliasing.h"

namespace rst
{
    enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    inline Buffers operator|(Buffers a, Buffers b)
    {
        return Buffers((int)a | (int)b);
    }

    inline Buffers operator&(Buffers a, Buffers b)
    {
        return Buffers((int)a & (int)b);
    }

    enum class Primitive // 原始数据类型
    {
        Line,
        Triangle
    };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments. These two structs
     * make sure that if you mix up with their orders, the compiler won't compile it.
     * Aka : Type safety
     * */
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

    class rasterizer
    {
    public:
        static rasterizer &get_instance(int w, int h, const std::string& image_format = ".png")
        {
            static rasterizer instance(w, h, image_format); // 懒汉模式，static 保证线程安全
            return instance;
        }

        // 删除拷贝构造函数和赋值操作符，确保单例
        rasterizer(const rasterizer &) = delete;
        rasterizer &operator=(const rasterizer &) = delete;
        std::unique_ptr<Image> image;

        pos_buf_id load_positions(const std::vector<Vec3f>& positions);
        ind_buf_id load_indices(const std::vector<Vec3i>& indices);
        col_buf_id load_colors(const std::vector<Vec3f>& colors);
        col_buf_id load_normals(const std::vector<Vec3f>& normals);

        void set_model(const Matrix4f& m);
        void set_view(const Matrix4f& v);
        void set_projection(const Matrix4f& p);
        void set_texture(const Texture& tex);

        void set_antialiasing(std::function<Vec3f(int x, int y, const Triangle &t)> aa);

        // void set_vertex_shader(std::function<Vec3f(vertex_shader_payload)> vert_shader);
        // void set_fragment_shader(std::function<Vec3f(fragment_shader_payload)> frag_shader);

        void set_pixel(const Vec2i &point, const Vec3f &color);

        void clearBuff(Buffers buff);

        // void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);
        void draw(std::vector<Triangle *> &TriangleList);

        auto& frame_buffer() { return image->get_data(); }

        void draw_line(const Vec3f &begin, const Vec3f &end, const Color &color);
        void rasterize_triangle(const Triangle &t, Color &color);

    private:

        // VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

    private:
        rasterizer(int w, int h, const std::string& format);

        Matrix4f model;
        Matrix4f view;
        Matrix4f projection;

        int normal_id = -1;
 
        std::map<int, std::vector<Vec3f>> pos_buf;
        std::map<int, std::vector<Vec3i>> ind_buf;
        std::map<int, std::vector<Vec3f>> col_buf;
        std::map<int, std::vector<Vec3f>> nor_buf;

        std::optional<Texture> texture;

        std::function<Vec3f(int x, int y, const Triangle &t)> antiAliasing;
        // std::function<Vec3f(fragment_shader_payload)> fragment_shader;
        // std::function<Vec3f(vertex_shader_payload)> vertex_shader;

        std::vector<float> depth_buf;
        int get_index(int x, int y) const { return (height - y - 1) * width + x; }

        int width, height;

        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
}
