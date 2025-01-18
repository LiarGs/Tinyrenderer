#include "OBJ_Loader.h"
#include "rasterizer.h"
#include "Materials.h"

int main(int argc, char **argv)
{
    std::string obj_path = "F:/Computer Graphic/Tinyrenderer/obj";

    objl::Loader model;
    if (2 == argc)
    {
        model.Load(argv[1]);
    }
    else
    {
        // model.Load(obj_path + "/cow/spot_triangulated_good.obj");
        model.Load(obj_path + "/african_head/african_head.obj");
    }

    int width = 800;
    int height = 800;

    std::vector<Triangle> TriangleList; // 要渲染的三角形
    // Load .obj File
    for (const auto &mesh : model.LoadedMeshes)
    {
        for (int i = 0; i < mesh.Vertices.size(); i += 3)
        {
            Triangle t;
            for (int j = 0; j < 3; j++)
            {
                t.setVertex(j, Vec3f{mesh.Vertices[i + j].Position.x, mesh.Vertices[i + j].Position.y, mesh.Vertices[i + j].Position.z});
                t.setNormal(j, Vec3f{mesh.Vertices[i + j].Normal.x, mesh.Vertices[i + j].Normal.y, mesh.Vertices[i + j].Normal.z});
                t.setTexCoord(j, Vec2f{mesh.Vertices[i + j].TextureCoordinate.x, mesh.Vertices[i + j].TextureCoordinate.y});
                t.setColor(j, {148, 121.0, 92.0});
            }
            TriangleList.push_back(t);
        }
    }
    // 创建一个材质的 Material 对象
    Material headMaterial = Materials::HeadMaterial(obj_path + "/Texture/african_head_diffuse.jpg");
    Material cowMaterial = Materials::cowMaterial(obj_path + "/cow/spot_texture.png");

    auto l1 = Light{{20.f, 20.f, 20.f}, {500.f, 500.f, 500.f}};
    auto l2 = Light{{-20.f, 20.f, 0.f}, {500.f, 500.f, 500.f}};
    auto camera = Camera{
        Vec3f{0.f, 0.f, 0.f}, // 相机位置
        Vec3f{.0f, 0.f, -1.0f}, // 看向哪个点
        Vec3f{0.0f, 1.0f, 0.0f} // 上方向
    };

    // 获取 Scene 实例
    auto &scene = Scene::get_instance(width, height);

    scene.add(std::make_unique<MeshTriangle>(TriangleList, headMaterial));
    scene.add(std::make_unique<Light>(l1));
    scene.add(std::make_unique<Light>(l2));
    scene.set_camera(std::make_unique<Camera>(camera));

    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height, ".png");
    ras.set_scene(scene); // 将 Scene 实例传递给 rasterizer

    // 设置片着色器类型
    // rst::FragmentShader active_shader = normal_fragment_shader;
    // rst::FragmentShader active_shader = phong_fragment_shader;
    rst::FragmentShader active_shader = texture_fragment_shader;
    // rst::FragmentShader active_shader = bump_fragment_shader;
    // rst::FragmentShader active_shader = displacement_fragment_shader;

    ras.set_fragment_shader(active_shader);
    ras.set_texture(Texture(headMaterial.map_Kd));

    // ras.set_vertex_shader(vertex_shader);

    float angle = 140.f;
    int key = 0;

    while (key != 27) {

        ras.clearBuff(rst::Buffers::Color | rst::Buffers::Depth);

        // 设置模型变换
        ras.set_model(
            Vec3f{0.f, .0f, -3.f},    // 平移
            Vec3f{0.f, angle, 0.f}, // 旋转
            Vec3f{1.f, 1.f, 1.f}      // 缩放
        );

        ras.draw();

        ras.show();

        key = cv::waitKey(10);
        if (key == 'a')
        {
            angle -= 30.f;  //单位:度
        }
        else if (key == 'd')
        {
            angle += 30.f;
        }
    }

    std::string filename = "output";
    ras.image->write_file(filename);

    return 0;
}