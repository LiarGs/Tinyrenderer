#include "Triangle.h"
#include "Log.h"

// Constructor
Triangle::Triangle() {
    for (int i = 0; i < 3; i++) {
        vertex[i] = Vec3f(0, 0, 0);
        color[i] = Vec3f(1, 1, 1); // default color white
        tex_coords[i] = Vec2f(0, 0);
        normal[i] = Vec3f(0, 0, 0);
    }
}

Triangle::Triangle(const Vec3f& v0, const Vec3f& v1,const Vec3f& v2) {
    vertex[0] = v0;
    vertex[1] = v1;
    vertex[2] = v2;
    for (int i = 0; i < 3; i++) {
        color[i] = Vec3f(1, 1, 1); // default color white
        tex_coords[i] = Vec2f(0, 0);
        normal[i] = Vec3f(0, 0, 0);
    }
}

// Check if a point is inside the triangle
bool Triangle::insideTriangle(const Vec3f& p) const
{
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Vec3f AB = vertex[1] - vertex[0];
    Vec3f BC = vertex[2] - vertex[1];
    Vec3f CA = vertex[0] - vertex[2];

    Vec3f Ap = p - vertex[0];
    Vec3f Bp = p - vertex[1];
    Vec3f Cp = p - vertex[2];

    // Check if the point is on the same side of the line formed by the vertices of the triangle
    auto crossproduct = [](const Vec3f &l, const Vec3f &r) -> bool
    {
        return l.x * r.y - l.y * r.x > 0;
    };

    return (crossproduct(AB, Ap) == crossproduct(BC, Bp)) && (crossproduct(BC, Bp) == crossproduct(CA, Cp));
}

// Set i-th vertex coordinates
void Triangle::setVertex(int ind, const Vec3f& ver)
{
    if (ind >= 0 && ind < 3) {
        vertex[ind] = ver;
    }
}

// Set i-th vertex normal vector
void Triangle::setNormal(int ind, const Vec3f& n) {
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

// Get the bounding box of the triangle
std::array<int, 4> Triangle::getBoundingBox() const
{
    float min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN;
    for (const auto &point : vertex)
    {
        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    return {static_cast<int>(floor(min_x)), static_cast<int>(floor(min_y)), static_cast<int>(ceil(max_x)), static_cast<int>(ceil(max_y))};
}

// Compute barycentric coordinates
std::array<float, 3> Triangle::computeBarycentric2D(const Vec2f& p) const {

    // Define a lambda for the common formula
    auto computeCoefficient = [&](int i, int j, int k) -> float
    {
        auto denominator = (vertex[i].x * (vertex[j].y - vertex[k].y) + (vertex[k].x - vertex[j].x) * vertex[i].y +
                            vertex[j].x * vertex[k].y - vertex[k].x * vertex[j].y);
        return (p.x * (vertex[j].y - vertex[k].y) + (vertex[k].x - vertex[j].x) * p.y +
                vertex[j].x * vertex[k].y - vertex[k].x * vertex[j].y) / denominator;
    };

    // Use the lambda function to compute the barycentric coordinates
    float c1 = computeCoefficient(0, 1, 2);
    float c2 = computeCoefficient(1, 2, 0);
    float c3 = 1-c1-c2; // c3 = computeCoefficient(2, 0, 1);

    return {c1, c2, c3};
}

// Convert to vector of Vec4f
std::array<Vec4f, 3> Triangle::toVector4() const
{
    std::array<Vec4f, 3> out;
    // std::transform(std::begin(v), std::end(v), std::begin(out), [](const Vec3f &v) {
    //     return Vec4f(v.x, v.y, v.z, 1.0f);
    // });
    for (int i = 0; i < 3; i++) {
        out[i] = Vec4f(vertex[i].x, vertex[i].y, vertex[i].z, 1.0f);
    }
    return out;
}