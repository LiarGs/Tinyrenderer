#include "OBJ_Loader.h"
#include "Materials.hpp"
#include "OrbitCamera.hpp"
#include "Shader.h"

int main(int argc, char **argv)
{
    std::string obj_path = "F:/Computer_Graphic/Tinyrenderer/obj";

    objl::Loader model_triangle, model_cow, model_body, model_monster, model_head;
    if (2 == argc)
    {
        model_cow.Load(argv[1]);
    }
    else
    {
        model_triangle.Load(obj_path + "/Triangle/triangle.obj");
        model_cow.Load(obj_path + "/cow/spot_triangulated_good.obj");
        model_body.Load(obj_path + "/boggie/body.obj");
        model_monster.Load(obj_path + "/diablo3_pose/diablo3_pose.obj");
        model_head.Load(obj_path + "/african_head/african_head.obj");
    }

    auto triangles = objl::LoadTriangleList(model_triangle);
    auto cow_TriangleList = objl::LoadTriangleList(model_cow);
    auto body_TriangleList = objl::LoadTriangleList(model_body);
    auto monster_TriangleList = objl::LoadTriangleList(model_monster);
    auto head_TriangleList = objl::LoadTriangleList(model_head);

    // 创建材质 Material 对象
    Material blackheadMaterial = Materials::SkinMaterial(obj_path + "/Texture/diffuse.jpg");
    Material whiteheadMaterial = Materials::SkinMaterial(obj_path + "/boggie/head_diffuse.jpg");
    Material bodyMaterial = Materials::SkinMaterial(obj_path + "/boggie/body_diffuse.jpg");
    Material monsterMaterial = Materials::SkinMaterial(obj_path + "/diablo3_pose/diablo3_pose_diffuse.jpg");
    Material cowMaterial = Materials::cowMaterial(obj_path + "/cow/spot_texture.png");

    const int width = 700;
    const int height = 700;
    // 获取 Scene 实例
    auto &scene = Scene::get_instance(width, height);

    auto l1 = Light{{20.f, 20.f, 20.f}, {500.f, 500.f, 500.f}};
    auto l2 = Light{{-20.f, 20.f, 0.f}, {500.f, 500.f, 500.f}};
    auto obj_pos = Vec3f{0.f, 0.f, -4.f};

    std::vector<MeshTriangle> objects = {
        MeshTriangle(triangles, Material()),
        MeshTriangle(cow_TriangleList, cowMaterial),
        MeshTriangle(body_TriangleList, bodyMaterial),
        MeshTriangle(monster_TriangleList, monsterMaterial),
        MeshTriangle(head_TriangleList, blackheadMaterial)};

    // 当前渲染物体的索引
    size_t current_obj_index = 0;
    scene.set_obj(std::make_unique<MeshTriangle>(objects[current_obj_index]));
    scene.add(std::make_unique<Light>(l1));
    scene.add(std::make_unique<Light>(l2));
    scene.set_amb_light_intensity({1, 1, 1});

    // 初始化相机控制器
    auto camera = std::make_shared<Camera>(Camera{Vec3f{0.f, 0.f, 0.f}, obj_pos, Vec3f{0.f, 1.f, 0.f}});
    OrbitCameraController cameraController(camera);
    scene.set_camera(camera);

    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height, ".png");
    ras.set_scene(scene);

    // 设置顶点着色器
    ras.set_vertex_shader(vertex_shader);

    // 定义片着色器类型数组
    std::vector<rst::FragmentShader> shaders = {
        normal_fragment_shader,
        white_fragment_shader,
        phong_fragment_shader,
        texture_fragment_shader,
        bump_fragment_shader,
        displacement_fragment_shader};
    
    // 当前片着色器的索引
    size_t current_shader_index = 0; // 默认使用 normal_fragment_shader
    ras.set_fragment_shader(shaders[current_shader_index]);

    // 设置 OpenCV 的鼠标回调函数
    cv::namedWindow("Render Window");
    cv::setMouseCallback("Render Window", mouse_callback, &cameraController);

    int key = 0;
    size_t frame_count = 0;

    auto startTime = cv::getTickCount();

    while (key != 27) {
        // 设置模型变换
        ras.set_model(
            obj_pos, // 平移
            cameraController.rotateVec,      // 旋转
            Vec3f{1.f, 1.f, 1.f}   // 缩放
        );
        
        ras.draw();

        // 帧计数
        ++frame_count;
        // 计算fps
        double currentTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();
        auto fps = frame_count / currentTime;
        std::string fps_str = "FPS: " + std::to_string(static_cast<int>(fps));

        ras.show(fps_str);

        // 重置计数器和计时器
        frame_count = 0;
        startTime = cv::getTickCount();

        key = cv::pollKey();
        switch (key)
        {
        case 'e':
            ras.set_rendermode(rst::RenderMode::EDGE);
            break;
        case 'f':
            ras.set_rendermode(rst::RenderMode::FACE);
            break;
        case 'v':
            ras.set_rendermode(rst::RenderMode::VERTEX);
            break;
        case 'm':
            ras.multithreading = !ras.multithreading;
            break;
        case 'a':
            ras.anti_Aliasing = !ras.anti_Aliasing;
            break;
        case 'p':
            cv::waitKey();
            break;
        case Direction::UP: // 方向键上
            current_shader_index = (current_shader_index - 1 + shaders.size()) % shaders.size();
            ras.set_fragment_shader(shaders[current_shader_index]);
            break;
        case Direction::DOWN: // 方向键下
            current_shader_index = (current_shader_index + 1) % shaders.size();
            ras.set_fragment_shader(shaders[current_shader_index]);
            break;
        case Direction::LEFT: // 方向键左
            current_obj_index = (current_obj_index - 1 + objects.size()) % objects.size();
            scene.set_obj(std::make_unique<MeshTriangle>(objects[current_obj_index]));
            break;
        case Direction::RIGHT: // 方向键右
            current_obj_index = (current_obj_index + 1) % objects.size();
            scene.set_obj(std::make_unique<MeshTriangle>(objects[current_obj_index]));
            break;
        default:
            break;
        }
    }

    std::string filename = "output";
    ras.image->write_file(filename);

    return 0;
}