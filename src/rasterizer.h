#pragma once
#include <optional>
#include "ThreadPool.hpp"
#include "Triangle.h"
#include "Image.h"
#include "Texture.h"
#include "Scene.hpp"

namespace rst
{
    class rasterizer;
    struct vertex_shader_payload
    {
        Matrix4f model;
        Matrix4f view;
        Matrix4f projection;
        Matrix4f mvp;
        Matrix4f inv_trans;
    };

    struct fragment_shader_payload
    {
        Vec3f view_pos;
        Vec3f color;
        Vec3f normal;
        Vec3f amb_light_intensity;
        Vec2f tex_coords;
    };

    using FragmentShader = std::function<Vec3f(const fragment_shader_payload &, rasterizer&)>;
    using VertexShader = std::function<void(vertex_shader_payload &, Triangle *)>;

    enum RenderMode
    {
        FACE,
        EDGE,
        VERTEX
    };
    
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

        void set_vertex_shader(VertexShader vert_shader) { vertex_shader = vert_shader; }
        void set_fragment_shader(FragmentShader frag_shader) { fragment_shader = frag_shader; }

        void set_scene(Scene &scene) { this->scene = &scene; }
        void set_material(Material mat) { 
            material = mat;
            if (mat.map_Kd.valid)
                texture = mat.map_Kd;
        }
        void set_pixel(const Vec2i &point, const Vec3f &color);  // 渲染区用
        void set_rendermode(RenderMode mode) { renderMode = mode; }
        void clearBuff(Buffers buff);
        void show(std::string) const;

        void draw_point(const Vec2f p, const Color &color) {
            set_pixel({p.x, p.y}, color.getVec());
        }
        void draw_point_triangle(Triangle &t);
        void draw_line(const Vec3f &begin, const Vec3f &end, const Color &color);
        void draw_triangle_line(Triangle &t);
        void rasterize_triangle(Triangle &t);
        void rasterize_triangle_list(std::vector<Triangle> &triangles);
        void draw_obj(const std::unique_ptr<Object> &obj);
        void draw();

        Scene *scene;
        std::optional<Material> material;
        std::optional<Texture>  texture;
        bool anti_Aliasing = false;
        const int samples = 2;
        bool multithreading = false; // 是否启用多线程
    private:
        // 多线程使用
        std::mutex queueMutex; // 任务队列锁
        std::mutex depth_mutex;

    private:
        rasterizer(int w, int h, const std::string &format);
        RenderMode renderMode{FACE};
        size_t triangleCount = 0;
        
        fragment_shader_payload fragment_payload;
        vertex_shader_payload vertex_payload;
        FragmentShader fragment_shader;
        VertexShader vertex_shader;

        std::vector<float> depth_buf;
        std::vector<Vec3f> back_buf = {}; // 渲染用

        // ssaa抗锯齿用
        std::vector<float> super_depth_buf;
        std::vector<Vec3f> super_back_buf = {};
        int get_index(int x, int y) const
        {
            return (height - y - 1) * width + x;
        }

        int width, height;
    };
}
