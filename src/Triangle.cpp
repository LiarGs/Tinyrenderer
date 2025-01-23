#include "Triangle.h"
#include "Log.h"

// Constructor
Triangle::Triangle() {
    for (int i = 0; i < 3; i++) {
        vertex[i] = Vec3f{0, 0, 0};
        color[i] = Vec3f{1, 1, 1};
        normal[i] = Vec3f{0, 0, 0};
    }
    update();
}

Triangle::Triangle(const Vec3f& v0, const Vec3f& v1,const Vec3f& v2) {
    vertex[0] = v0;
    vertex[1] = v1;
    vertex[2] = v2;
    update();
    for (int i = 0; i < 3; i++)
    {
        color[i] = Vec3f{1, 1, 1};
        tex_coords[i] = Vec2f{0, 0};
        normal[i] = Vec3f{0, 0, 0};
    }
}

void Triangle::setVertex(int ind, const Vec3f& ver)
{
    if (ind >= 0 && ind < 3) {
        vertex[ind] = ver;
    }
}

void Triangle::setNormal(int ind, const Vec3f& n) {
    if (ind >= 0 && ind < 3) {
        normal[ind] = n;
    }
}

void Triangle::setColor(int ind, const Vec3f &c)
{
    auto r = c.x;
    auto g = c.y;
    auto b = c.z;
    if ((r < 0.0) || (r > 255.) ||
        (g < 0.0) || (g > 255.) ||
        (b < 0.0) || (b > 255.))
    {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[ind] = Vec3f{(float)r / 255.f, (float)g / 255.f, (float)b / 255.f};
    return;
}

void Triangle::setTexCoord(int ind, const Vec2f &uv)
{
    if (ind >= 0 && ind < 3) {
        tex_coords[ind] = uv;
    }
}

bool Triangle::insideTriangle(const Vec3f &p) const
{
    auto crossproduct = [](const Vec3f &l, const Vec3f &r) -> bool
    {
        return l.x * r.y - l.y * r.x >= 0;
    };

    Vec3f Ap = p - vertex[0];
    Vec3f Bp = p - vertex[1];
    Vec3f Cp = p - vertex[2];

    auto crossAB = crossproduct(AB, Ap);
    auto crossBC = crossproduct(BC, Bp);
    auto crossCA = crossproduct(CA, Cp);

    return (crossAB && crossBC && crossCA);
    // return (crossAB == crossBC) && (crossBC == crossCA);
    // return (crossproduct(AB, Ap) == crossproduct(BC, Bp)) && (crossproduct(BC, Bp) == crossproduct(CA, Cp));
}
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


std::array<float, 3> Triangle::computeBarycentric2D(const Vec2f &p) const
{
    // TODO->计算屏幕空间中的重心坐标
    auto denominator = double_area2D;

    auto computeCoefficient = [&](int j, int k) -> float
    {
        return (p.x * (vertex[j].y - vertex[k].y) + (vertex[k].x - vertex[j].x) * p.y +
                    vertex[j].x * vertex[k].y - vertex[k].x * vertex[j].y);
    };

    float c1 = computeCoefficient(1, 2) / denominator;
    float c2 = computeCoefficient(2, 0) / denominator;
    float c3 = 1-c1-c2; // c3 = computeCoefficient(2, 0, 1);

    return {c1, c2, c3};
}

std::array<Vec4f, 3> Triangle::toVector4(const std::array<Vec3f, 3> &points, const float &w) const
{
    std::array<Vec4f, 3> out;
    // std::transform(std::begin(vertex), std::end(vertex), std::begin(out), [](const Vec3f &v)
    //                { return vertex[i].toVector4(w); });
    for (int i = 0; i < 3; i++) {
        out[i] = points[i].toVector4(w);
    }
    return out;
}