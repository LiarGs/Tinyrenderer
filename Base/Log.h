#pragma once

#include <time.h>
#include <string>
#include <vector>
#include <iostream>

static std::string getTime()
{
    const char *time_fmt = "%Y-%m-%d %H:%M:%S";
    time_t t = time(nullptr);
    char time_str[64];
    struct tm time_info;
    localtime_s(&time_info, &t);
    strftime(time_str, sizeof(time_str), time_fmt, &time_info);

    return time_str;
}

static std::string getFile(std::string file)
{
#ifndef WIN32
    std::string pattern = "/";
#else
    std::string pattern = "\\";
#endif // !WIN32

    std::string::size_type pos;
    std::vector<std::string> result;
    file += pattern; // 扩展字符串以方便操作
    size_t file_size = file.size();
    for (size_t i = 0; i < file_size; i++)
    {
        pos = file.find(pattern, i);
        if (pos < file_size)
        {
            std::string s = file.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result.back();
}

// 以下是编译器预定义宏
//  __FILE__ 获取源文件的相对路径和名字
//  __LINE__ 获取该行代码在文件中的行号
//  __func__ 或 __FUNCTION__ 获取函数名

// #define LOGI(format_str, ...) std::cerr << "[INFO]" << getTime() << " [" << getFile(__FILE__) << ":" << __LINE__ << "] " << format_str << "\n", ##__VA_ARGS__
// #define LOGE(format_str, ...) std::cerr << "[ERROR]" << getTime() << " [" << getFile(__FILE__) << ":" << __LINE__ << "] " << format_str << "\n", ##__VA_ARGS__

#define LOGI(format_str, ...) fprintf(stderr, "[INFO]%s [%s:%d] " format_str "\n", getTime().data(), getFile(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOGE(format_str, ...) fprintf(stderr, "[ERROR]%s [%s:%d] " format_str "\n", getTime().data(), getFile(__FILE__), __LINE__, ##__VA_ARGS__)