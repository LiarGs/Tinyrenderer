#include "OBJ_Loader.h"
#include "rasterizer.h"
#include "Materials.h"
#include "OrbitCamera.h"

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
        // model.Load(obj_path + "/cow/spot_triangulated_good.obj");
        model.Load(obj_path + "/boggie/body.obj");
        // model.Load(obj_path + "/diablo3_pose/diablo3_pose.obj");
        // model.Load(obj_path + "/african_head/african_head.obj");
    }

    int width = 960;
    int height = 960;

    auto model_TriangleList = objl::LoadTriangleList(model);

    // 创建一个材质的 Material 对象
    Material blackheadMaterial = Materials::SkinMaterial(obj_path + "/Texture/african_head_diffuse.jpg");
    Material whiteheadMaterial = Materials::SkinMaterial(obj_path + "/boggie/head_diffuse.jpg");
    Material bodyMaterial = Materials::SkinMaterial(obj_path + "/boggie/body_diffuse.jpg");
    Material monsterMaterial = Materials::SkinMaterial(obj_path + "/diablo3_pose/diablo3_pose_diffuse.jpg");
    Material cowMaterial = Materials::cowMaterial(obj_path + "/cow/spot_texture.png");
    Material glassMaterial = Materials::GlassMaterial();

    auto l1 = Light{{20.f, 20.f, 20.f}, {500.f, 500.f, 500.f}};
    auto l2 = Light{{-20.f, 20.f, 0.f}, {500.f, 500.f, 500.f}};
    auto obj_pos = Vec3f{0.f, 0.f, -4.f};

    // 获取 Scene 实例
    auto &scene = Scene::get_instance(width, height);
    scene.add(std::make_unique<MeshTriangle>(model_TriangleList, bodyMaterial));
    scene.add(std::make_unique<Light>(l1));
    scene.add(std::make_unique<Light>(l2));

    // 获取 rasterizer 实例
    auto &ras = rst::rasterizer::get_instance(width, height, ".png");
    ras.set_scene(scene);

    // 设置顶点着色器
    ras.set_vertex_shader(vertex_shader);

    // 设置片着色器类型
    // rst::FragmentShader active_shader = normal_fragment_shader;
    rst::FragmentShader active_shader = white_fragment_shader;
    // rst::FragmentShader active_shader = phong_fragment_shader;
    // rst::FragmentShader active_shader = texture_fragment_shader;
    // rst::FragmentShader active_shader = bump_fragment_shader;
    // rst::FragmentShader active_shader = displacement_fragment_shader;

    ras.set_fragment_shader(active_shader);
    ras.set_texture(scene.get_objects()[0]->material.map_Kd);
    ras.set_rendermode(rst::RenderMode::FACE);

    // 初始化相机控制器
    auto camera = std::make_shared<Camera> (Camera{Vec3f{0.f, 0.f, 0.f}, obj_pos, Vec3f{0.f, 1.f, 0.f}});
    OrbitCameraController cameraController(camera);
    scene.set_camera(camera);

    // 设置 OpenCV 的鼠标回调函数
    cv::namedWindow("Render Window");
    cv::setMouseCallback("Render Window", mouse_callback, &cameraController);

    int key = 0;
    int frame_count = 0;
    auto startTime = cv::getTickCount();

    while (key != 27) {

        ras.clearBuff(rst::Buffers::Color | rst::Buffers::Depth);

        // 设置模型变换
        ras.set_model(
            obj_pos, // 平移
            cameraController.rotateVec,      // 旋转
            Vec3f{1.f, 1.f, 1.f}   // 缩放
        );
        
        ras.draw();

        // 帧计数
        frame_count++;

        // 计算fps
        double currentTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();
        auto fps = frame_count / currentTime;
        std::string fps_str = "FPS: " + std::to_string(static_cast<int>(fps));

        // 在图像上显示FPS
        cv::putText(ras.image->cv_image, fps_str, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

        // 重置计数器和计时器
        frame_count = 0;
        startTime = cv::getTickCount();

        ras.show();

        key = cv::waitKey(1);
        if (key == 'e') {
            ras.set_rendermode(rst::RenderMode::EDGE);
        }else if (key == 'f') {
            ras.set_rendermode(rst::RenderMode::FACE);
        }else if (key == 'v') {
            ras.set_rendermode(rst::RenderMode::VERTEX);
        }
    }

    std::string filename = "output";
    ras.image->write_file(filename);

    return 0;
}