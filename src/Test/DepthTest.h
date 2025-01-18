#include "Test.h"

class DepthTest : public Test
{
public:
    virtual ~DepthTest() = default;
    bool test(float interpolatedZ, float currentZ, int x, int y) const override
    {
        return testDepth(interpolatedZ, currentZ);
    }

    // 返回当前像素是否通过深度测试
    virtual bool testDepth(float interpolatedZ, float currentZ) const = 0;
};

// 简单的深度测试（当前像素深度是否大于 z-buffer 中存储的深度）
class SimpleDepthTest : public DepthTest
{
private:
    bool testDepth(float interpolatedZ, float currentZ) const override
    {
        return interpolatedZ > currentZ; // 深度大则通过
    }
};

// 阴影深度测试（这里只是一个简单的示例，实际的阴影可以有更复杂的计算）
class ShadowDepthTest : public DepthTest
{
public:
    bool testDepth(float interpolatedZ, float currentZ) const override
    {
        return interpolatedZ < currentZ; // 阴影中，较小的深度值通过
    }
};

// 屏幕空间反射（SSR）深度测试（可以在SSR中使用深度进行反射计算）
class SSRDepthTest : public DepthTest
{
public:
    bool testDepth(float interpolatedZ, float currentZ) const override
    {
        // 假设在反射中，我们要对深度做一些复杂的计算，这里仅做一个示例
        return interpolatedZ < currentZ; // 比较当前深度和z-buffer中的深度
    }
};
