#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <format>
#include <iostream>

namespace
{

    inline std::string getTime()
    {
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 转换为秒级精度的时间点
        auto now_seconds = std::chrono::floor<std::chrono::seconds>(now);
        // 格式化时间，精确到秒
        return std::format("{:%Y-%m-%d %H:%M:%S}", now_seconds);
    }

    inline std::string getFile(std::string_view file)
    {
        // 使用 std::filesystem::path 处理路径
        std::filesystem::path path(file);
        // 返回文件名部分，使用 u8string() 确保编码正确
        return path.filename().string();
    }

} // anonymous namespace

#define LOGI(format_str, ...) std::cerr << std::format("[INFO]{} [{}:{}] " format_str "\n", getTime(), getFile(__FILE__), __LINE__, ##__VA_ARGS__)

#define LOGE(format_str, ...) std::cerr << std::format("[ERROR]{} [{}:{}] " format_str "\n", getTime(), getFile(__FILE__), __LINE__, ##__VA_ARGS__)