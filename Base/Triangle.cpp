#include "Triangle.h"

// Constructor
Triangle::Triangle() {
    for (int i = 0; i < 3; i++) {
        v[i] = Vec3f(0, 0, 0);
        color[i] = Vec3f(1, 1, 1); // default color white
        tex_coords[i] = Vec2f(0, 0);
        normal[i] = Vec3f(0, 0, 0);
    }
}

Triangle::Triangle(const Vec3f& v0, const Vec3f& v1,const Vec3f& v2) {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    for (int i = 0; i < 3; i++) {
        color[i] = Vec3f(1, 1, 1); // default color white
        tex_coords[i] = Vec2f(0, 0);
        normal[i] = Vec3f(0, 0, 0);
    }
}


// Set i-th vertex coordinates
void Triangle::setVertex(int ind, Vec3f ver) {
    if (ind >= 0 && ind < 3) {
        v[ind] = ver;
    }
}

// Set i-th vertex normal vector
void Triangle::setNormal(int ind, Vec3f n) {
    if (ind >= 0 && ind < 3) {
        normal[ind] = n;
    }
}

// Set i-th vertex color
void Triangle::setColor(int ind, float r, float g, float b) {
    if ((r < 0.0) || (r > 255.) ||
        (g < 0.0) || (g > 255.) ||
        (b < 0.0) || (b > 255.))
    {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[ind] = Vec3f((float)r / 255.f, (float)g / 255.f, (float)b / 255.f);
    return;
}

// Set i-th vertex texture coordinate
void Triangle::setTexCoord(int ind, float s, float t) {
    if (ind >= 0 && ind < 3) {
        tex_coords[ind] = Vec2f(s, t);
    }
}

// Convert to vector of Vec4f
std::array<Vec4f, 3> Triangle::toVector4() const {
    std::array<Vec4f, 3> out;
    // std::transform(std::begin(v), std::end(v), std::begin(out), [](const Vec3f &v) {
    //     return Vec4f(v.x, v.y, v.z, 1.0f);
    // });
    for (int i = 0; i < 3; i++) {
        out[i] = Vec4f(v[i].x, v[i].y, v[i].z, 1.0f);
    }
    return out;
}