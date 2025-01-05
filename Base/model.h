#pragma once

#include <vector>
#include "geometry.h"

class Model
{
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int>> faces_;

public:
    Model(const char *filename);
    ~Model();
    int nverts(); // number of vertices
    int nfaces(); // number of faces
    Vec3f vert(int i);
    std::vector<int> face(int idx);
};