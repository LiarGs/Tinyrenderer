#pragma once
#include <vector>
#include <memory>

// 抽象测试基类
class Test
{
public:
    virtual ~Test() = default;

    // 子类需要实现的测试方法
    virtual bool test(float interpolatedZ, float currentZ, int x, int y) const = 0;
};

// // 组合测试类
// class CompositeTest : public Test
// {
// public:
//     // 构造函数，接受多个测试策略
//     CompositeTest(const std::vector<Test> &tests)
//         : tests_(tests) {}

//     // 覆盖 test() 方法，依次执行每个测试
//     bool test(float interpolatedZ, float currentZ, int x, int y) const override
//     {
//         for (const auto &test : tests_)
//         {
//             // 如果任何一个测试失败，返回 false
//             if (!test.test(interpolatedZ, currentZ, x, y))
//                 return false;
//         }
//         return true; // 所有测试通过
//     }

// private:
//     std::vector<Test> tests_; // 存储多个测试策略
// };
