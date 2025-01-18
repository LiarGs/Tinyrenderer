#include "OBJ_Loader.h"
#include "rasterizer.h"

int main(int argc, char **argv)
{
    objl::Loader model;
    if (2 == argc)
    {
        model.Load(argv[1]);
    }
    else
    {
        model.Load("../../../obj/african_head/african_head.obj");
    }

    int width = 800;
    int height = 800;

    std::vector<Triangle *> TriangleList; // 要渲染的三角形
    // Load .obj File
    for (const auto &mesh : model.LoadedMeshes)
    {
        for (int i = 0; i < mesh.Vertices.size(); i += 3)
        { 
            Triangle *t = new Triangle();
            for (int j = 0; j < 3; j++)
            {
                t->setVertex(j, Vec3f{mesh.Vertices[i + j].Position.x, mesh.Vertices[i + j].Position.y, mesh.Vertices[i + j].Position.z});
                t->setNormal(j, Vec3f{mesh.Vertices[i + j].Normal.x, mesh.Vertices[i + j].Normal.y, mesh.Vertices[i + j].Normal.z});
                t->setTexCoord(j, Vec2f{mesh.Vertices[i + j].TextureCoordinate.x, mesh.Vertices[i + j].TextureCoordinate.y});
            }
            TriangleList.push_back(t);
        }
    }
    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height, ".png");

    auto texture_path = "../../../obj/Texture/african_head_diffuse.jpg";
    ras.set_texture(Texture(texture_path));

    // rst::FragmentShader active_shader = phong_fragment_shader;

    rst::FragmentShader active_shader = texture_fragment_shader;

    // ras.set_vertex_shader(vertex_shader);
    ras.set_fragment_shader(active_shader);

    ras.clearBuff(rst::Buffers::Color | rst::Buffers::Depth);

    Vec3f light_dir{0, 0, -1}; // define light_dir
    fragment_shader_payload payload;
    payload.texture = new Texture(texture_path);

    for (auto &triangle : TriangleList)
    {
        // 用世界坐标计算法向量
        Vec3f n = (triangle->vertex[2] - triangle->vertex[0]) ^ (triangle->vertex[1] - triangle->vertex[0]);
        n.normalize();
        float intensity = n * light_dir;    // 光照强度=法向量*光照方向
        if (intensity < 0) continue;
        
        auto colorColor = Vec3f{rand() % 255, rand() % 255, rand() % 255};
        auto lightColor = Vec3f{intensity * 255, intensity * 255, intensity * 255};

        std::array<Vec2i, 3> screen_coords; // 三角形三个顶点的屏幕坐标
        triangle->viewspace_pos = triangle->vertex;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = triangle->vertex[j];
            screen_coords[j] = Vec2i{(v.x + 1.) * width / 2., (v.y + 1.) * height / 2.};
            triangle->setVertex(j, Vec3f{static_cast<float>(screen_coords[j].x), static_cast<float>(screen_coords[j].y), v.z});
            triangle->setColor(j, Vec3f{intensity * 255, intensity * 255, intensity * 255});
        }
        ras.rasterize_triangle(*triangle);
    }

    // 将图像结果写入image
    auto data = ras.image->frame_buf.data();
    ras.image->cv_image = cv::Mat(height, width, CV_32FC3, reinterpret_cast<float *>(data));

    ras.image->cv_image.convertTo(ras.image->cv_image, CV_8UC3, 255.0f);

    cv::cvtColor(ras.image->cv_image, ras.image->cv_image, cv::COLOR_RGB2BGR);

    ras.show();
    cv::waitKey();
    std::string filename = "output";
    ras.image->write_file(filename);
    return 0;
}