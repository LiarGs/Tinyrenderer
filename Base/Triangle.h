#pragma once
# include "Vec.h"
# include <array>

class Triangle
{
public:
    Vec3f vertex[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    Vec3f color[3];      // color at each vertex;
    Vec2f tex_coords[3]; // texture u,v
    Vec3f normal[3];     // normal vector for each vertex

    // Texture *tex;
    Triangle();
    Triangle(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2);
    bool insideTriangle(const Vec3f& p) const;
    void setVertex(int ind, const Vec3f &vertex);     /*set i-th vertex coordinates */
    void setNormal(int ind, const Vec3f &normal);     /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b);   /*set i-th vertex color (r: red, g: green, b: blue)*/
    Vec3f getColor() const { return color[0] * 255; } // Only one color per triangle.
    std::array<int, 4> getBoundingBox() const;
    std::array<float, 3> computeBarycentric2D(const Vec2f&) const;
    void setTexCoord(int ind, float s, float t); /*set i-th vertex texture coordinate (s: u coordinate, t: v coordinate)*/
    std::array<Vec4f, 3> toVector4() const;
};