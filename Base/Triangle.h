#pragma once
# include "Vec.h"
# include <array>

class Triangle
{
public:
    std::array<Vec3f, 3> vertex;
    std::array<Vec3f, 3> color;
    std::array<Vec2f, 3> tex_coords;
    std::array<Vec3f, 3> normal;
    // std::shared_ptr<Texture> tex = nullptr;

    Triangle();
    Triangle(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2);
    bool insideTriangle(const Vec3f& p) const;

    auto a() const { return vertex[0]; }
    auto b() const { return vertex[1]; }
    auto c() const { return vertex[2]; }

    void setVertex(int ind, const Vec3f &vertex);     /*set i-th vertex coordinates */
    void setNormal(int ind, const Vec3f &normal);     /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b);   /*set i-th vertex color */
    void setTexCoord(int ind, const Vec2f &uv);          /*set i-th vertex texture coordinate*/

    void setNormals(const std::array<Vec3f, 3> &normals) { normal = normals; };
    void setColors(const std::array<Vec3f, 3> &colors) { color = colors; };

    std::array<int, 4> getBoundingBox() const;
    std::array<float, 3> computeBarycentric2D(const Vec2f&) const;

    // 模板函数声明
    std::array<Vec4f, 3> toVector4(const std::array<Vec3f, 3> &points, const float &w) const;
    inline Vec3f interpolate(float alpha, float beta, float gamma, float weight = 1.f) { return (alpha * vertex[0] + beta * vertex[1] + gamma * vertex[2]) / weight; }
};