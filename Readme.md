# TinyRender - 从零实现的软渲染器

一个基于C++17实现的现代软渲染器，支持obj模型加载、PBR材质、静态光照、实时相机控制及多种渲染模式。从矩阵变换到片元着色，不依赖图形API，深入理解计算机图形学核心原理。

## ✨ 核心功能

### 🎛️ 可编程渲染管线
- **顶点着色器**  
  完整实现MVP矩阵变换与法线变换（`vertex_shader`函数）
- **片元着色器框架**  
  支持6种可编程着色器：
  - 法线可视化 (`normal_fragment_shader`)
  - Phong光照 (`phong_fragment_shader`)
  - 纹理映射 (`texture_fragment_shader`)
  - 凹凸/位移贴图 (`bump/displacement_fragment_shader`)
  - 纯色/白模渲染 (`white_fragment_shader`)

### 🛠️ 依赖项
- OpenCV 4.x（渲染结果查看与保存）
- C++17兼容编译器（C++20支持的日志`Log.h`）

### 📊 实时调试信息
- **三角形面数统计**  
  窗口标题实时显示渲染三角面数（`rasterizer::show()`）
- **帧率监控**  
  画面左上角显示实时FPS（`main.cpp`中的帧计时逻辑）

### 📦 模型与材质系统
- **OBJ模型加载**  
  支持带UV和法线的OBJ模型解析（见`OBJ_Loader.cpp`）
- **PBR材质管线**  
  实现环境光/漫反射/镜面反射贴图（`Materials.h`中金属、皮肤、玻璃等材质）可自定义添加材质
- **纹理映射**  
  UV采样与透视校正（`Texture.cpp`）

### 💡 光照与着色
- **Phong光照模型**  
  完整实现环境/漫反射/镜面反射分量（`phong_fragment_shader`）
- **法线/位移贴图**  
  TBN矩阵变换实现凹凸效果（`bump/displacement_fragment_shader`）
- **多光源支持**  
  点光源衰减与强度计算（`Shader.cpp`中的光照累加）

### 🎥 交互式相机
- **轨道控制器**  
  鼠标拖拽旋转/右键平移/滚轮缩放（`OrbitCamera.h`实现）
- **透视投影**  
  可调FOV和裁剪平面（`rasterizer.cpp`中的投影矩阵）
- **模型变换**  
  支持模型平移/旋转/缩放组合变换（`set_model`方法）

![Orbit Camera Control](../demo/CameraOrbit.gif)

### 🖌️ 多样化渲染
- **多模式切换**  
  `F`键面渲染/`E`键线框模式/`V`键顶点模式（`rasterizer.cpp`渲染管线）
- **抗锯齿方案**  
  4x SSAA超级采样（注释部分可启用）

![Rendering Mode Switch](../demo/Rendermodelchange.gif)

## 🛠️ 交互控制

| 按键         | 功能         |
| ------------ | ------------ |
| 鼠标左键拖拽 | 旋转模型     |
| 鼠标右键拖拽 | 平移场景     |
| 滚轮        | 缩放相机    |
| E/F/V      | 切换渲染模式 |

## 🚀 性能优化
- **包围盒剪裁**：三角形快速剔除（`getBoundingBox`）
- **背面剔除**：背面三角形渲染优化(有向三角形面积`double_area2D`管理)

## 📚 实现亮点
- **自定义矩阵向量类**：矩阵运算向量化（`Vec.h`模板类）
- **可编程片着色器**：自定义片着色器（`Shader.cpp`）
- **透视校正**：深度感知的重心坐标插值（`rasterize_triangle`）
- **材质系统**：基于物理的反射率计算（`Material`结构体）

## 📸 效果展示
| 着色器切换 | 模型展示 |
|-----------|---------|
|法线可视化| ![Normal result](../demo/normal.png) | 
|Phong光照| ![Phone result](../demo/phone.png) |
|纹理映射| ![Texture result](../demo/texture.png) | 
|凹凸贴图| ![Bump result](../demo/bump.png) |
|位移渲染| ![Displacement result](../demo/displacement.png) | 
|白模渲染| ![White result](../demo/white.png) |


## 🌟 未来计划
- [ ] 阴影映射
- [ ] 延迟渲染管线
- [ ] GPU加速计算

---

> 🚧 项目持续开发中，欢迎提交Issue和PR！  