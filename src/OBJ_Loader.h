#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Log.hpp"
#include "Triangle.h"

namespace objl {
    // Structure: Vertex
    //
    // Description: Model Vertex object that holds
    //	a Position, Normal, and Texture Coordinate
    struct Vertex
    {
        // Position Vector
        Vec3f Position;

        // Normal Vector
        Vec3f Normal;

        // Texture Coordinate Vector
        Vec2f TextureCoordinate;
    };

    struct Material
    {
        Material()
        {
            name;
            Ns = 0.0f;
            Ni = 0.0f;
            d = 0.0f;
            illum = 0;
        }
        std::string name; // 材质名称
        
        // 基础属性
        Vec3f Ka;  // 环境光颜色
        Vec3f Kd;  // 漫反射颜色
        Vec3f Ks;  // 镜面反射颜色
        float Ns;  // 镜面反射指数
        float Ni;  // 折射率
        float d;   // 透明度
        int illum; // 光照模型

        // 纹理贴图
        std::string map_Ka;   // 环境光贴图
        std::string map_Kd;   // 漫反射贴图
        std::string map_Ks;   // 镜面反射贴图
        std::string map_Ns;   // 镜面高光贴图
        std::string map_d;    // 透明度贴图
        std::string map_bump; // 凹凸贴图
    };

    // Structure: Mesh
    //
    // Description: A Simple Mesh Object that holds
    //	a name, a vertex list, and an index list
    struct Mesh
    {
        // Default Constructor
        Mesh() {}
        // Variable Set Constructor
        Mesh(std::vector<Vertex> &_Vertices) : Vertices(_Vertices) {}

        // Mesh Name
        std::string MeshName;
        // Vertex List
        std::vector<Vertex> Vertices;

        // Index List
        // std::vector<unsigned int> Indices;

        // Material
        // Material MeshMaterial;
    };

    class Loader
    {
    private:

        std::vector<Vec3f> _Vertices;
        std::vector<Vec2f> _Texture;
        std::vector<Vec3f> _Normals;

    public:
        Loader() = default;

        bool Load(const std::string &);

        ~Loader() = default;

        size_t nverts() { return _Vertices.size(); }; // 顶点数量
        size_t nfaces() { return LoadedMeshes.size(); }; // 面数量

        // Loaded Mesh Objects
        std::vector<Mesh> LoadedMeshes;

        // Loaded Index Positions
        // std::vector<unsigned int> LoadedIndices;
        // Loaded Material Objects
        // std::vector<Material> LoadedMaterials;
    };

    std::vector<Triangle> LoadTriangleList(const Loader &loader);
}
