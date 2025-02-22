#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Log.hpp"
#include "Triangle.h"

namespace objl {
    struct Vertex
    {
        Vec3f Position;

        Vec3f Normal;

        Vec2f TextureCoordinate;
    };

    struct Mesh
    {
        Mesh(std::vector<Vertex> &_Vertices) : Vertices(_Vertices) {}

        std::vector<Vertex> Vertices;
    };

    class Loader
    {
    private:
        std::vector<Vec3f> _Vertices;
        std::vector<Vec2f> _Texture;
        std::vector<Vec3f> _Normals;

    public:
        bool Load(const std::string &);

        std::vector<Mesh> LoadedMeshes;
    };

    std::vector<Triangle> LoadTriangleList(const Loader &loader);
}
