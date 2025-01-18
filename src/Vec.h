#pragma once
#include <cfloat>
#include <cmath>
#include <vector>
#include <iostream>
#include <array>
constexpr double MY_PI = 3.14159;

template <class T, size_t N>
class Vec;
template <class T, size_t M, size_t N>
class Matrix;

using Vec2f = Vec<float, 2>;
using Vec2i = Vec<int, 2>;
using Vec3f = Vec<float, 3>;

using Vec3i = Vec<int, 3>;
using Vec4f = Vec<float, 4>;

using Matrix4f = Matrix<float, 4, 4>;
using Matrix4i = Matrix<int, 4, 4>;
using Matrix3f = Matrix<float, 3, 3>;

// 基础 Vec 类模板
template <class T, size_t N>
class Vec
{
public:
    union
    {
        T raw[N]; // 存储向量数据的数组
        struct
        {
            T x, y, z;
        };
    };

    // 默认构造函数
    Vec()
    {
        std::fill(std::begin(raw), std::end(raw), T(0));
    }

    // 构造函数
    Vec(T value)
    {
        std::fill(std::begin(raw), std::end(raw), value);
    }

    // {}构造函数
    Vec(std::initializer_list<T> values)
    {
        std::copy(values.begin(), values.begin() + N, raw);
    }

    // 新增的类型转换构造函数
    template <typename U>
    Vec(std::initializer_list<U> values)
    {
        size_t i = 0;
        for (auto val : values)
        {
            raw[i++] = static_cast<T>(val); // 类型转换
        }
    }

    const T &w() const
    {
        static_assert(4 == N, "Vec has no w element.");
        return raw[3];
    }

    // 实现 head 方法
    template <size_t M>
    inline auto head() const
    {
        static_assert(M <= N, "Cannot extract more elements than the vector has.");
        Vec<T, M> result;
        std::copy(raw, raw + M, result.raw);
        return result;
    }

    // 向量加法
    inline Vec<T, N> operator+(const Vec<T, N> &v) const
    {
        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = raw[i] + v.raw[i];
        }
        return result;
    }

    // 重载 += 运算符
    inline Vec<T, N> &operator+=(const Vec<T, N> &v)
    {
        for (size_t i = 0; i < N; ++i)
        {
            raw[i] += v.raw[i];
        }
        return *this;
    }

    // 向量减法
    inline Vec<T, N> operator-(const Vec<T, N> &v) const
    {
        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = raw[i] - v.raw[i];
        }
        return result;
    }

    inline Vec<T, N> &operator-=(const Vec<T, N> &v)
    {
        for (size_t i = 0; i < N; ++i)
        {
            raw[i] -= v.raw[i];
        }
        return *this;
    }

    // 标量乘法
    inline Vec<T, N> operator*(T f) const
    {
        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = raw[i] * f;
        }
        return result;
    }

    inline Vec<T, N> &operator*=(T scalar)
    {
        for (size_t i = 0; i < N; ++i)
        {
            raw[i] *= scalar;
        }
        return *this;
    }

    friend Vec<T, N> operator*(T f, const Vec<T, N> &v)
    {
        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = v.raw[i] * f;
        }
        return result;
    }

    // 标量除法
    inline Vec<T, N> operator/(T f) const
    {
        if (f == 0)
        {
            return *this;
        }

        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = this->raw[i] / f;
        }
        return result;
    }

    inline Vec<T, N> &operator/=(T scalar)
    {
        if (scalar == 0)
        {
            throw std::runtime_error("Division by zero");
        }
        for (size_t i = 0; i < N; ++i)
        {
            raw[i] /= scalar;
        }
        return *this;
    }


    friend Vec<T, N> operator/(T f, const Vec<T, N> &v)
    {
        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = f / v.raw[i];
        }
        return result;
    }

    // 点积
    inline T operator*(const Vec<T, N> &v) const
    {
        T result = 0;
        for (size_t i = 0; i < N; ++i)
        {
            result += raw[i] * v.raw[i];
        }
        return result;
    }

    // 逐元素乘法
    Vec<T, N> cwiseProduct(const Vec<T, N> &v) const
    {
        Vec<T, N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result.raw[i] = raw[i] * v.raw[i];
        }
        return result;
    }

    // 叉乘：仅适用于三维向量
    inline Vec<T, 3> operator^(const Vec<T, N> &v) const
    {
        static_assert(N == 3, "Cross product is only defined for 3D vectors.");

        Vec<T, 3> result;
        result.raw[0] = raw[1] * v.raw[2] - raw[2] * v.raw[1]; // x
        result.raw[1] = raw[2] * v.raw[0] - raw[0] * v.raw[2]; // y
        result.raw[2] = raw[0] * v.raw[1] - raw[1] * v.raw[0]; // z
        return result;
    }

    inline bool operator==(const Vec<T, N> &v) const
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (raw[i] != v.raw[i]) return false;
        }
        return true;
    }

    inline Vec<T, 4> toVector4(T w) const
    {
        static_assert(N == 3, "toVector4 is only defined for 3D vectors.");

        return Vec<T, 4>{x, y, z, w};
    }

    // 求模
    inline T norm() const
    {
        T sum = 0;
        for (size_t i = 0; i < N; ++i)
        {
            sum += raw[i] * raw[i];
        }
        return std::sqrt(sum);
    }

    // 归一化（原地修改）
    inline Vec<T, N> &normalize(T l = 1)
    {
        T n = norm();
        if (n == 0)
            return *this; // 防止除以零

        for (size_t i = 0; i < N; ++i)
        {
            raw[i] = raw[i] * (l / n);
        }
        return *this;
    }

    // 重载输出流
    friend std::ostream &operator<<(std::ostream &s, const Vec<T, N> &v)
    {
        s << "(";
        for (size_t i = 0; i < N; ++i)
        {
            s << v.raw[i];
            if (i < N - 1)
                s << ", ";
        }
        s << ")";
        return s;
    }
};

// 基础 Matrix 类模板
template <class T, size_t M, size_t N>
class Matrix
{
public:
    T m[M][N]; // MxN矩阵的元素

    // 默认构造函数（若M==N 则初始化为单位矩阵,否则为全零矩阵）
    Matrix()
    {
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                m[i][j] = (M == N && i == j) ? static_cast<T>(1) : static_cast<T>(0); // 单位矩阵
            }
        }
    }

    // 自定义构造函数
    Matrix(T v)
    {
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                m[i][j] = v; // 所有元素初始化为v
            }
        }
    }
    Matrix(const std::initializer_list<T> &values)
    {
        if (values.size() != M * N)
        {
            throw std::invalid_argument("Initializer list size does not match matrix size.");
        }

        auto it = values.begin(); // 获取迭代器
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < N; ++j)
            {
                m[i][j] = *it; // 从 initializer_list 中提取值
                ++it;          // 移动到下一个值
            }
        }
    }
    // 按列向量填充矩阵
    Matrix(const std::array<Vec<T, M>, N> &vecs)
    {
        for (size_t j = 0; j < N; ++j)
        {
            for (size_t i = 0; i < M; ++i)
            {
                m[i][j] = vecs[j].raw[i]; // 将每个 Vec 的值填充到矩阵的对应列
            }
        }
    }

    // 重载加法操作符
    auto operator+(const Matrix<T, M, N> &other) const
    {
        Matrix<T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = m[i][j] + other.m[i][j];
            }
        }
        return result;
    }

    // 重载减法操作符
    auto operator-(const Matrix<T, M, N> &other) const
    {
        Matrix<T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = m[i][j] - other.m[i][j];
            }
        }
        return result;
    }

    // 重载标量乘法操作符
    auto operator*(T scalar) const
    {
        Matrix<T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }

    friend auto operator*(T scalar, const Matrix<T,M,N> &other)
    {
        Matrix<T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = scalar * m[i][j];
            }
        }
        return result;
    }

    // 重载标量除法操作符
    auto operator/(T scalar) const
    {
        Matrix<T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = m[i][j] / scalar;
            }
        }
        return result;
    }

    friend auto operator/(T scalar, const Matrix<T, M, N> &other)
    {
        Matrix<T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = scalar / m[i][j];
            }
        }
        return result;
    }

    // 重载矩阵乘法操作符
    auto operator*(const Matrix<T, M, N> &other) const
    {
        Matrix <T, M, N> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[i][j] = 0;
                for (int k = 0; k < N; ++k)
                {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // 矩阵与Vec的乘法
    auto operator*(const Vec<T, N> &v) const
    {
        Vec<T, M> result;
        for (int i = 0; i < M; ++i)
        {
            result.raw[i] = 0;
            for (int j = 0; j < N; ++j)
            {
                result.raw[i] += m[i][j] * v.raw[j];
            }
        }
        return result;
    }

    auto transpose() const
    {
        Matrix<T, N, M> result;
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result.m[j][i] = m[i][j];
            }
        }
        return result;
    }

    // 求逆矩阵
    auto inverse() const
    {
        static_assert(M == N, "Matrix must be square to compute inverse.");

        Matrix<T, M, N> result;
        Matrix<T, M, N> temp = *this;

        // 高斯-约当消元法
        for (int i = 0; i < M; ++i)
        {
            // 寻找主元
            T pivot = temp.m[i][i];
            if (pivot == 0)
            {
                throw std::runtime_error("Matrix is singular and cannot be inverted.");
            }

            // 将主元所在行归一化
            for (int j = 0; j < N; ++j)
            {
                temp.m[i][j] /= pivot;
                result.m[i][j] /= pivot;
            }

            // 消去其他行的主元
            for (int k = 0; k < M; ++k)
            {
                if (k != i)
                {
                    T factor = temp.m[k][i];
                    for (int j = 0; j < N; ++j)
                    {
                        temp.m[k][j] -= factor * temp.m[i][j];
                        result.m[k][j] -= factor * result.m[i][j];
                    }
                }
            }
        }

        return result;
    }

    // 重载输出运算符
    friend std::ostream &operator<<(std::ostream &s, Matrix<T, M, N> &matrix)
    {
        for (int i = 0; i < M; ++i)
        {
            s << "| ";
            for (int j = 0; j < N; ++j)
            {
                s << matrix.m[i][j] << " ";
            }
            s << "|" << std::endl;
        }
        return s;
    }
};