#include "OBJ_Loader.h"
#include "Materials.hpp"
#include "OrbitCamera.hpp"
#include "Shader.h"

int main(int argc, char **argv)
{
    std::string obj_path = "F:/Computer_Graphic/Tinyrenderer/obj";

    objl::Loader model;
    if (2 == argc)
    {
        model.Load(argv[1]);
    }
    else
    {
        // model.Load(obj_path + "/Triangle/triangle.obj");
        // model.Load(obj_path + "/cow/spot_triangulated_good.obj");
        // model.Load(obj_path + "/boggie/body.obj");
        // model.Load(obj_path + "/diablo3_pose/diablo3_pose.obj");
        model.Load(obj_path + "/african_head/african_head.obj");
    }

    auto model_TriangleList = objl::LoadTriangleList(model);

    // 创建材质 Material 对象
    Material blackheadMaterial = Materials::SkinMaterial(obj_path + "/Texture/diffuse.jpg");
    Material whiteheadMaterial = Materials::SkinMaterial(obj_path + "/boggie/head_diffuse.jpg");
    Material bodyMaterial = Materials::SkinMaterial(obj_path + "/boggie/body_diffuse.jpg");
    Material monsterMaterial = Materials::SkinMaterial(obj_path + "/diablo3_pose/diablo3_pose_diffuse.jpg");
    Material cowMaterial = Materials::cowMaterial(obj_path + "/cow/spot_texture.png");
    Material glassMaterial = Materials::GlassMaterial();

    auto l1 = Light{{20.f, 20.f, 20.f}, {500.f, 500.f, 500.f}};
    auto l2 = Light{{-20.f, 20.f, 0.f}, {500.f, 500.f, 500.f}};
    auto obj_pos = Vec3f{0.f, 0.f, -4.f};

    const int width = 700;
    const int height = 700;
    // 获取 Scene 实例
    auto &scene = Scene::get_instance(width, height);

    scene.add(std::make_unique<MeshTriangle>(model_TriangleList, blackheadMaterial));
    scene.add(std::make_unique<Light>(l1));
    scene.add(std::make_unique<Light>(l2));
    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height, ".png");
    ras.set_scene(scene);

    // 设置顶点着色器
    ras.set_vertex_shader(vertex_shader);

    ras.set_rendermode(rst::RenderMode::FACE);

    // 设置片着色器类型
    // rst::FragmentShader active_shader = normal_fragment_shader;
    // rst::FragmentShader active_shader = white_fragment_shader;
    // rst::FragmentShader active_shader = phong_fragment_shader;
    rst::FragmentShader active_shader = texture_fragment_shader;
    // rst::FragmentShader active_shader = bump_fragment_shader;
    // rst::FragmentShader active_shader = displacement_fragment_shader;

    ras.set_fragment_shader(active_shader);

    // 初始化相机控制器
    auto camera = std::make_shared<Camera> (Camera{Vec3f{0.f, 0.f, 0.f}, obj_pos, Vec3f{0.f, 1.f, 0.f}});
    OrbitCameraController cameraController(camera);
    scene.set_camera(camera);

    // 设置 OpenCV 的鼠标回调函数
    cv::namedWindow("Render Window");
    cv::setMouseCallback("Render Window", mouse_callback, &cameraController);

    int key = 0;
    int frame_count = 0;
    int message_show_frame = 60; // 消息显示帧数
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

        ras.show(fps_str, message_show_frame);

        // 重置计数器和计时器
        frame_count = 0;
        startTime = cv::getTickCount();

        key = cv::pollKey();
        if (key == 'e') {
            ras.set_rendermode(rst::RenderMode::EDGE);
        }else if (key == 'f') {
            ras.set_rendermode(rst::RenderMode::FACE);
        }else if (key == 'v') {
            ras.set_rendermode(rst::RenderMode::VERTEX);
        }else if (key == 'm') {
            ras.multithreading = !ras.multithreading;
            message_show_frame = 60;
        }else if (key == 'a') {
            ras.anti_Aliasing = !ras.anti_Aliasing;
            message_show_frame = 60;
        }else if (key == 'p') {
            cv::waitKey();
        }
    }

    std::string filename = "output";
    ras.image->write_file(filename);

    return 0;
}