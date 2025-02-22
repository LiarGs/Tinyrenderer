#pragma once
# include "Vec.hpp"
# include <array>

class Triangle
{
public:
    std::array<Vec3f, 3> vertex; // 三角形在模型空间中的坐标
    std::array<Vec3f, 3> viewspace_pos; // 三角形在视图空间中的坐标
    std::array<Vec3f, 3> color;
    std::array<Vec2f, 3> tex_coords;
    std::array<Vec3f, 3> normal;

    Vec3f AB, BC, CA;
    float double_area2D;

    Triangle();
    Triangle(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2);

    const auto &a() const { return vertex[0]; }
    const auto &b() const { return vertex[1]; }
    const auto &c() const { return vertex[2]; }

    void update() {
        AB = vertex[1] - vertex[0];
        BC = vertex[2] - vertex[1];
        CA = vertex[0] - vertex[2];
        double_area2D = getDoubleArea2D();
    }

    void setVertex(int ind, const Vec3f &vertex);     /*set i-th vertex coordinates */
    void setNormal(int ind, const Vec3f &normal);     /*set i-th vertex normal vector*/
    void setColor(int ind, const Vec3f &color);      /*set i-th vertex color */
    void setTexCoord(int ind, const Vec2f &uv);          /*set i-th vertex texture coordinate*/

    void setNormals(const std::array<Vec3f, 3> &normals) { normal = normals; };
    void setColors(const std::array<Vec3f, 3> &colors) { color = colors; };

    bool insideTriangle(const Vec3f&) const;
    std::array<int, 4> getBoundingBox() const;
    float getDoubleArea2D() const { return (AB.x * (-CA.y) - (-CA.x) * AB.y); } // 计算三角形有向面积的两倍
    std::array<float, 3> computeBarycentric2D(const Vec2f&) const;

    std::array<Vec4f, 3> toVector4(const std::array<Vec3f, 3> &points, const float &w) const;
};