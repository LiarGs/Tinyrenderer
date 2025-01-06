#pragma once
#include <cfloat>
#include <cmath>
#include <iostream>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Vec2
{
public:
    union
    {
        struct
        {
            T u, v;
        };
        struct
        {
            T x, y;
        };
        T raw[2];
    };


    Vec2() : u(0), v(0) {}
    Vec2(T _u, T _v) : u(_u), v(_v) {}
    inline Vec2<T> operator+(const Vec2<T> &V) const { return Vec2<T>(u + V.u, v + V.v); }
    inline Vec2<T> operator-(const Vec2<T> &V) const { return Vec2<T>(u - V.u, v - V.v); }
    inline Vec2<T> operator*(T f) const { return Vec2<T>(u * f, v * f); }
    inline T operator*(const Vec2<T> &V) const { return u * V.u + v * V.v; }
    template <class>
    friend std::ostream &operator<<(std::ostream &s, Vec2<T> &v);
};

template <class T>
class Vec3
{
public:
    union
    {
        struct
        {
            T x, y, z;
        };
        struct
        {
            T ivert, iuv, inorm;
        };
        T raw[3];
    };

    Vec3() : x(0), y(0), z(0) {}
    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

    inline Vec3<T> operator^(const Vec3<T> &v) const { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    inline Vec3<T> operator+(const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
    inline Vec3<T> operator-(const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
    inline Vec3<T> operator*(T f) const { return Vec3<T>(x * f, y * f, z * f); }
    inline T operator*(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
    float norm() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3<T> &normalize(T l = 1)
    {
        *this = (*this) * (l / norm());
        return *this;
    }
    template <class>
    friend std::ostream &operator<<(std::ostream &s, Vec3<T> &v);
};

template <class T>
class Vec4
{
public:
    union
    {
        struct
        {
            T x, y, z, w;
        };
        T raw[4];
    };

    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

    inline Vec4<T> operator+(const Vec4<T> &v) const { return Vec4<T>(x + v.x, y + v.y, z + v.z, w + v.w); }
    inline Vec4<T> operator-(const Vec4<T> &v) const { return Vec4<T>(x - v.x, y - v.y, z - v.z, w - v.w); }
    inline Vec4<T> operator*(T f) const { return Vec4<T>(x * f, y * f, z * f, w * f); }
    template <class>
    friend std::ostream &operator<<(std::ostream &s, Vec4<T> &v); // 重载输出运算符
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec4<float> Vec4f;

template <class T>
std::ostream &operator<<(std::ostream &s, Vec2<T> &v)
{
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class T>
std::ostream &operator<<(std::ostream &s, Vec3<T> &v)
{
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

template <class T>
std::ostream &operator<<(std::ostream &s, Vec4<T> &v)
{
    s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
    return s;
}