#pragma once
#include <optional>
#include <algorithm>
// #include <tbb/parallel_for.h>
// #include <tbb/blocked_range.h>
#include "Vec.h"
// #include "Test.h"
#include "Triangle.h"
#include "Image.h"
#include "Texture.h"
#include "Shader.h"
#include "Scene.h"
// #include "AntiAliasing.h"

namespace rst
{
    enum RenderMode
    {
        FACE,
        EDGE,
        VERTEX
    };
    
    using FragmentShader = std::function<Vec3f(const fragment_shader_payload &)>;
    using VertexShader = std::function<void(vertex_shader_payload &)>;
    
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

    class rasterizer
    {
    public:

        static rasterizer &get_instance(int w, int h, const std::string &image_format = ".png")
        {
            static rasterizer instance(w, h, image_format); // 懒汉模式，static 保证线程安全
            return instance;
        }

        // 删除拷贝构造函数和赋值操作符，确保单例
        rasterizer(const rasterizer &) = delete;
        rasterizer &operator=(const rasterizer &) = delete;

        std::unique_ptr<Image> image;

        void set_model(const Vec3f &translate, const Vec3f &rotate, const Vec3f &scale);
        void set_view(const Vec3f &target_pos, const Vec3f &eye_dir, const Vec3f &up_dir);
        void set_projection(float eye_fov, const float &aspect_ratio, const float &zNear, const float &zFar);

        // void set_antialiasing(std::function<Vec3f(int x, int y, const Triangle &t)> aa) { antiAliasing = aa; }

        void set_vertex_shader(VertexShader vert_shader) { vertex_shader = vert_shader; }
        void set_fragment_shader(FragmentShader frag_shader) { fragment_shader = frag_shader; }

        void set_scene(Scene &scene) { this->scene = &scene; }
        void set_texture(Texture tex) { texture = tex; }
        void set_pixel(const Vec2i &point, const Vec3f &color);
        void set_rendermode(RenderMode mode) { renderMode = mode; }
        void clearBuff(Buffers buff);
        auto &frame_buffer() { return image->frame_buf; }
        void show() const;

        void draw_point(const Vec2f p, const Color &color) { image->set(p.x, p.y, color); }
        void draw_point_triangle(Triangle &t);
        void draw_line(const Vec3f &begin, const Vec3f &end, const Color &color);
        void draw_triangle_line(Triangle &t);
        void rasterize_triangle(Triangle &t);
        void rasterize_triangle_list(const std::vector<Triangle> &triangles);
        void draw_obj(const std::unique_ptr<Object> &obj);
        void draw();

    private:

        // VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

    private:

        rasterizer(int w, int h, const std::string &format);
        RenderMode renderMode{FACE};
        size_t triangleCount = 0;

        std::map<int, std::vector<Vec3f>> pos_buf;
        std::map<int, std::vector<Vec3i>> ind_buf;
        std::map<int, std::vector<Vec3f>> col_buf;
        std::map<int, std::vector<Vec3f>> nor_buf;

        Scene *scene;
        std::optional<Texture> texture;
        fragment_shader_payload fragment_payload;
        vertex_shader_payload vertex_payload;
        FragmentShader fragment_shader;
        VertexShader vertex_shader;

        // std::function<Vec3f(int x, int y, const Triangle &t)> antiAliasing;

        std::vector<float> depth_buf;
        std::mutex depth_mtx;

        int get_index(int x, int y) const { return (height - y - 1) * width + x; }

        int width, height;

        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
}
