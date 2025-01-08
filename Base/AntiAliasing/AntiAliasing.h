#pragma once
# include "Triangle.h"
#include "tgaimage.h"


// 抗锯齿基类
class AntiAliasing
{
protected:
    int samples;

public:
    AntiAliasing(int samples = 1) : samples(samples) {}
    virtual ~AntiAliasing() = default;

    // 每种抗锯齿算法的计算方式
    virtual void antialiasing(const Triangle &t, int x, int y, Image *image, Color &color) const = 0;

    void setSamples(int samples) { this->samples = samples; }
};

class NOAA : public AntiAliasing {
    NOAA() : AntiAliasing() {}
    float getCoverage(const Triangle &t, int x, int y) const { return 0; }
};

// SSAA 抗锯齿
class SSAA : public AntiAliasing
{
public:
    SSAA(int samples = 4) : AntiAliasing(samples) {}
    // 计算覆盖度
    void antialiasing(const Triangle &t, int x, int y, Image *image, Color &color) const override;
};

// MSAA (Multi-Sample Anti-Aliasing) 5x5 示例
class MSAA : public AntiAliasing
{
public:
    MSAA(int samples = 25) : AntiAliasing(samples) {} // 默认25个样本（5x5）

    void antialiasing(const Triangle &t, int x, int y, Image *image, Color &color) const override;
};

// 可以添加其他抗锯齿方法，比如 MSAA、FXAA...