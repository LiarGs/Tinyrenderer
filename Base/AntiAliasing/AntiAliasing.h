#pragma once
#include "Triangle.h"
#include "Image.h"

class NOAA{
    NOAA() {}
    float getCoverage(const Triangle &t, int x, int y) const { return 0; }
};

// SSAA 抗锯齿
class SSAA
{
private:
    int samples;

public:
    SSAA(int samples = 4){}
    // 计算覆盖度
    void antialiasing(const Triangle &t, int x, int y, Color &color) const;
};

// MSAA (Multi-Sample Anti-Aliasing) 5x5 示例
class MSAA
{
public:
    MSAA(int samples = 25)  {} // 默认25个样本（5x5）

    void antialiasing(const Triangle &t, int x, int y, Color &color) const;
};

// 可以添加其他抗锯齿方法，比如 MSAA、FXAA...